from flask import Flask, request, jsonify
from datetime import datetime
import json
import os
import threading
import time
import subprocess

app = Flask(__name__)

# Archivo para almacenar los datos
DEVICES_FILE = 'devices.json'

# Tiempo entre verificaciones de estado (en segundos)
CHECK_INTERVAL = 30
# Tiempo de espera para ping (en segundos)
PING_TIMEOUT = 2

def load_devices():
    if not os.path.exists(DEVICES_FILE):
        return {}
    with open(DEVICES_FILE, 'r') as f:
        try:
            return json.load(f)
        except json.JSONDecodeError:
            return {}

def save_devices(devices):
    with open(DEVICES_FILE, 'w') as f:
        json.dump(devices, f, indent=2)

def ping_device(ip):
    try:
        result = subprocess.run(['ping', '-c', '1', '-W', str(PING_TIMEOUT), ip],
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE,
                               text=True)
        return result.returncode == 0
    except:
        return False

def check_devices_status():
    while True:
        print("Verificando dispositivos...")
        devices = load_devices()
        updated = False
        
        for device_id, device_data in list(devices.items()):
            ip = device_data.get('ip')
            if ip:
                is_active = ping_device(ip)
                current_state = device_data.get('state', 'inactive')
                
                if is_active and current_state != 'active':
                    devices[device_id]['state'] = 'active'
                    devices[device_id]['last-seen'] = datetime.now().isoformat()
                    updated = True
                elif not is_active and current_state != 'inactive':
                    devices[device_id]['state'] = 'inactive'
                    updated = True
        
        if updated:
            save_devices(devices)
        
        time.sleep(CHECK_INTERVAL)

@app.route('/api/devices/register', methods=['POST'])
def register_device():
    data = request.get_json()
    if not data or 'id' not in data or 'ip' not in data:
        return jsonify({'error': 'Datos inválidos'}), 400
    
    devices = load_devices()
    device_id = data['id']
    
    # Crear o actualizar el dispositivo
    devices[device_id] = {
        'id': device_id,
        'ip': data['ip'],
        'hardware': data.get('hardware', ''),
        'state': 'active',  # Asumimos activo al registrarse
        'last-seen': datetime.now().isoformat(),
        'registered-at': datetime.now().isoformat()
    }
    
    save_devices(devices)
    return jsonify({'status': 'success', 'device': devices[device_id]})

@app.route('/api/devices', methods=['GET'])
def get_devices():
    devices = load_devices()
    return jsonify(devices)

@app.route('/api/devices/<device_id>', methods=['GET'])
def get_device(device_id):
    devices = load_devices()
    device = devices.get(device_id)
    if device:
        return jsonify(device)
    else:
        return jsonify({'error': 'Dispositivo no encontrado'}), 404

if __name__ == '__main__':
    # Iniciar el hilo para verificar estado de dispositivos
    checker_thread = threading.Thread(target=check_devices_status)
    checker_thread.daemon = True
    checker_thread.start()

    # Iniciar el servidor Flask
    app.run(host='0.0.0.0', port=5000, debug=True)
