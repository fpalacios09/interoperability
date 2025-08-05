# Pruebas Básicas de Interoperabilidad de Dispositivos Embebidos

Este proyecto busca demostrar la **interoperabilidad entre dispositivos** (con diferentes variantes de hardware) y una **Raspberry Pi** actuando como servidor de gestión. Forma parte de un sistema experimental orientado a entornos inteligentes (smart environments), donde múltiples dispositivos pueden anunciarse, registrarse y ser monitoreados de forma automatizada.

---

## 📡 Dispositivos Utilizados

Se utilizan tres microcontroladores con conectividad diferente:

- ✅ **ESP32 Olimex PoE** – Conectado por Ethernet.
- ✅ **ESP32-WROOM32** – Conectado por WiFi.
- ✅ **Nano ESP32** – También conectado por WiFi (versión reducida).

Cada uno ejecuta su propio firmware Arduino, el cual se comunica con el servidor para registrarse automáticamente.

---

## 🧠 Descripción del Sistema

- Cada dispositivo, al iniciar, **envía una solicitud HTTP POST** con su información (`id`, `ip`, `hardware`) a un servidor Flask.
- La Raspberry Pi, mediante el script `device_manager.py`, recibe estos datos y guarda el estado de cada dispositivo en un archivo JSON.
- El servidor realiza pings periódicos a los dispositivos para determinar si están **activos o inactivos**, actualizando su estado y última vez visto (`last-seen`).

---

## 📂 Estructura del Repositorio

```
├── arduino/
│ ├── esp32_olimex/ → Código para ESP32 Olimex PoE (Ethernet)
│ ├── esp32_wifi/ → Código para ESP32-WROOM32 (WiFi)
├ └── nano_esp32/ → Código para Nano ESP32 (WiFi)
|
├── LICENSE
├── README.md
├── device_manager.py
└── pinv024_informe
```

## ⚙️ Requisitos
En la Raspberry Pi (o PC que actúe como servidor), se requiere Python con Flask.
Para los Dispositivos Embebidos, se requiere gestor de tarjetas ESP32, librería ArduinoJson by: bblanchon.

## 🚀 Ejecución
### En Raspberry Pi
Definir IP estática en la raspberry

```
sudo nano /etc/dhcpcd.conf
```
Descomentar o añadir lo siguiente (con IP deseada):
```
interface eth0
static ip_address=192.168.100.1/24
```
*Reiniciar*

Ejecutar:
```
python3 device_manager.py
```

### En Placas ESP32
Cargar los respectivos códigos Arduino a cada placa. Cambiar las IP por direcciones dentro de la misma subred que la Raspberry Pi sin repetir direcciones.

### Conexión Wireless
Si la Raspberry Pi no posee conexión wireless para las ESP32 WiFi, se debe utilizar un Router o Access Point conectado al switch, donde se conectarán todos los dispositivos cableados.

## Resultados esperados
Al ejecutar el sistema completo:
- Los dispositivos se registran automáticamente al conectarse.
- El servidor muestra en consola el monitoreo periódico.
- El archivo ```devices.json``` refleja el estado actual de todos los nodos.
- En el archivo ```pinv024_informe....pdf``` se encuentran resultados registrados.











