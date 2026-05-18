# Portfolio
My personal projects and work focused on solving common problems or just for fun.
--------------------------------------------------------------------------------------------------------

## 🛠️ Embedded Systems & IoT Projects

### 1. Automated Gate Access Control System (`NFC-Access-Control-System`)
* **Description:** An automated system designed for gate control and access management through RFID/NFC authentication and distance-based travel control using ultrasound.
* **Hardware Architecture:**
  * Main SoC: ESP32 (managing control logic and peripherals).
  * RFID Peripheral: MFRC522 interfaced via SPI protocol.
  * Environment Sensors: Ultrasonic distance sensor (HC-SR04 or equivalent).
  * Actuators & Indicators: DC drive motor, relay modules for H-bridge direction control, piezoelectric buzzer, and status LEDs.
* **Firmware & Operating Logic:** The system operates under a dual-mode architecture (ON/OFF). Upon reading a card with an authorized UID, the system enters **Automatic Mode**: the ultrasonic sensor dynamically monitors the vehicle or object's position to control the gate motor's opening and closing cycles via relays. Swiping the authorized card again safely interrupts the cycle and shifts the system back to Suspended Mode.
* **Applications & Scope:** Residential automation, smart parking lots, and secure perimeter access control.

### 2. Smart Passenger & Object Counter (`Smart-Object-Counter`)
* **Description:** A smart bidirectional object/passenger counter featuring wireless connectivity, industrial-grade power protection, and PIN-secured data access.
* **Hardware Architecture:**
  * Sensors: Dual industrial E18-D80NK photoelectric infrared sensors (up to 1-meter range).
  * Power Stage & Protection: LM2596 DC-DC Buck converter for voltage stabilization, and a **24V Transient Voltage Suppressor (TVS) Diode** to suppress power-line voltage spikes and electrical noise.
  * Communication & Interface: Bluetooth module (native SoC or external) and an acoustic notification buzzer.
* **Firmware & Operating Logic:** Bidirectional detection (in/out entry logging) is processed using a structured **State Machine (Switch-Case)** that discriminates the sequential activation order of the photoelectric sensors. It implements a software-based debounce algorithm to prevent false positives and includes a timeout alert routine if a sensor is obstructed for too long. Access to the stored counts via the mobile application is protected by a 4-digit security PIN validation firmware.
* **Applications & Scope:** Passenger counting for public transit (buses/subways), retail foot-traffic auditing, and industrial production line counting.

### 3. Enterprise Security & IoT Validator (`Secure-Pass`)
* **Description:** An IoT edge device for real-time identity validation and organizational mapping linked to a centralized database server.
* **Hardware Architecture:**
  * Processor: ESP32 (utilizing its native Wi-Fi stack).
  * RFID/NFC Reader: MFRC522 interconnected via **SPI** communication bus.
  * Notifications: LED indicator matrix and confirmation buzzer.
* **Firmware & Operating Logic:** The device acts as an IoT network client. When an RFID tag is scanned, the ESP32 encrypts the unique card UID, packages it alongside a **unique device ID** (firmware hardcoded to pinpoint the physical location of the scanner), and formats it into a **JSON** payload. This structure is transmitted via HTTP requests to a personal web server for validation against enterprise databases.
* **Applications & Scope:** Workforce time and attendance tracking, multi-branch corporate access control, and cloud-based membership management.

### 4. Traffic Light Controller with Audio Accessibility (`Traffic-Light-Controller`)
* **Description:** An urban traffic light controller integrating a power stage and a pre-recorded voice assistance system for visually impaired pedestrians.
* **Hardware Architecture:**
  * Power Stage: Optocouplers and TRIACs/Relays for switching traffic lamps (Green, Yellow, Red).
  * Audio Module: Dedicated voice playback module (such as DFPlayer or equivalent) and a high-fidelity speaker.
* **Firmware & Operating Logic:** The firmware executes a cyclic traffic synchronization sequence based on structured timing intervals. When switching the high-voltage power lines for the lights, the microcontroller triggers the audio module to play clear voice commands ("Walk" for green, "Don't walk" or "Stop" for red), ensuring urban infrastructure inclusivity.
* **Applications & Scope:** Smart city planning, accessible urban environments, and pedestrian safety improvement.

---

## 📐 Proteus Simulation & Circuit Designs

The following designs represent analog and digital electronic circuits fully simulated and validated within the **Proteus VSM** software environment.

### 5. Acoustic Actuated Smart Switch (`Encendido-Apagado inteligente`)
* **Description:** An automated switch triggered by high-intensity acoustic events (such as handclaps or sharp noises) for residential lighting control.
* **Key Components:** CD4017 decade counter, electret microphone (transducer), signal pre-amplification stage, electromechanical relay, and AC load (lightbulb).
* **Circuit Logic:** The microphone captures sound waves, and a signal conditioning circuit converts the pressure peak into a digital clock pulse (CLK). This pulse is fed to the **CD4017** IC, which sequentially toggles its logic output state, switching the relay coil to open or close the lightbulb's mains power connection.

### 6. Motion Activated Timer Relay (`Semáforo con integrado`)
* **Description:** An automatic timing system for energy-efficient lighting based on physical motion detection.
* **Key Components:** NE555 timer integrated circuit, PIR motion sensor (or equivalent simulated interface), RC timing capacitors, relay, and light indicator.
* **Circuit Logic:** Upon receiving a trigger edge from the motion sensor, the **NE555 configured in monostable mode** drives its output HIGH, energizing the relay and turning on the lightbulb. The activation duration is strictly determined by the RC time constant formula: $\tau = 1.1 \times R \times C$. The system automatically cuts off current flow once the configured time elapses, optimizing power consumption.

---

# Portafolio
Mis proyectos personales y trabajos enfocados en resolver problemas comunes o simplemente por diversión
--------------------------------------------------------------------------------------------------------

## 🛠️ Proyectos de Sistemas Embebidos e IoT

### 1. Automated Gate Access Control System (`NFC-Access-Control-System`)
* **Descripción:** Sistema automatizado para el control de acceso y apertura de portones mediante autenticación RFID/NFC y gestión de carrera por ultrasonido.
* **Arquitectura de Hardware:**
  * SoC Principal: ESP32 (gestionando periféricos y lógica de control).
  * Periférico RFID: MFRC522 conectado mediante protocolo SPI.
  * Sensores de Entorno: Sensor ultrasónico de distancia (HC-SR04 o similar).
  * Actuadores e Indicadores: Motor DC de arrastre, módulos de relés para inversión de giro, Buzzer piezoeléctrico y LEDs de estado.
* **Firmware y Lógica de Operación:** El sistema opera bajo una arquitectura bimodal (Encendido/Apagado). Al leer una tarjeta con el UID autorizado, el sistema entra en **Modo Automático**: el sensor ultrasónico monitorea la posición del vehículo u objeto para controlar dinámicamente la apertura y cierre del portón mediante los relés del motor. Al volver a pasar la tarjeta UID, el sistema interrumpe el ciclo y pasa a Modo Suspendido de manera segura.
* **Aplicaciones y Alcances:** Automatización residencial, estacionamientos inteligentes y control de perímetros restringidos.

### 2. Smart Passenger & Object Counter (`Smart-Object-Counter`)
* **Descripción:** Contador bidireccional inteligente de personas u objetos con conectividad inalámbrica, protección industrial y validación por PIN de seguridad.
* **Arquitectura de Hardware:**
  * Sensores: Dos sensores infrarrojos fotoeléctricos industriales E18-D80NK (alcance de hasta 1 metro).
  * Interfaz de Potencia y Protección: Convertidor DC-DC Buck LM2596 (estabilización de voltaje) y **Diodo de Supresión de Voltaje Transitorio (TVS) de 24V** para la supresión de picos y ruido eléctrico en la alimentación.
  * Comunicación e Interfaz: Módulo Bluetooth (o integrado del SoC) y Buzzer acústico de notificación.
* **Firmware y Lógica de Operación:** La detección bidireccional (entradas/salidas) se procesa mediante una **Máquina de Estados Estructurada (Switch-Case)** que discrimina el orden de activación de los sensores fotoeléctricos. Cuenta con un algoritmo de *antirrebote (debounce)* por software para evitar falsos positivos. Adicionalmente, implementa una rutina de alerta si un sensor es obstruido por tiempo prolongado. El acceso a los datos almacenados desde la aplicación móvil está restringido mediante un firmware de validación por PIN de 4 dígitos.
* **Aplicaciones y Alcances:** Control de aforo en transporte público (buses/metros), auditoría de tiendas retail y automatización industrial en líneas de producción.

### 3. Enterprise Security & IoT Validator (`Secure-Pass`)
* **Descripción:** Dispositivo IoT para la validación de identidad y enlace organizacional en tiempo real conectado a un servidor centralizado.
* **Arquitectura de Hardware:**
  * Procesador: ESP32 (Aprovechando su pila Wi-Fi nativa).
  * Lector RFID/NFC: MFRC522 interconectado por bus de comunicación **SPI**.
  * Notificaciones: Matriz de LEDs indicadores y Buzzer de confirmación.
* **Firmware y Lógica de Operación:** El dispositivo actúa como un cliente IoT. Al leer una tarjeta, el ESP32 empaqueta el identificador único encriptado junto con un **ID único de dispositivo** (firmware que identifica la ubicación exacta del lector) en una trama con formato **JSON**. Esta estructura se transmite mediante peticiones de red hacia un servidor web personal para su validación en bases de datos empresariales.
* **Aplicaciones y Alcances:** Sistemas de cronometraje y asistencia laboral, control de accesos corporativos multi-sede y gestión de membresías en la nube.

### 4. Traffic Light Controller with Audio Accessibility (`Traffic-Light-Controller`)
* **Descripción:** Controlador de semáforo urbano que integra una interfaz de potencia y un sistema de asistencia por voz para personas con discapacidad visual.
* **Arquitectura de Hardware:**
  * Etapa de Potencia: Optoacopladores y TRIACs/Relés para el manejo de lámparas de semáforo (Verde, Amarillo, Rojo).
  * Módulo de Audio: Transductor/Módulo de reproducción de voz pregrabada (tipo DFPlayer o similar) y altavoz de alta fidelidad.
* **Firmware y Lógica de Operación:** El firmware ejecuta una secuencia cíclica de sincronización vial basada en temporizaciones estructuradas. Al conmutar el estado de las luces de potencia, el microcontrolador envía un disparador al módulo de audio para reproducir comandos de voz claros ("Avance" en verde, "Deténgase" en rojo), garantizando la inclusión en la infraestructura urbana.
* **Aplicaciones y Alcances:** Urbanismo inteligente, entornos urbanos accesibles y mejora de la seguridad vial.

---

## 📐 Diseños y Simulaciones en Proteus

Los siguientes diseños corresponden a circuitos electrónicos analógicos y digitales simulados y validados en el entorno de software **Proteus VSM**.

### 5. Acoustic Actuated Smart Switch (`Encendido-Apagado inteligente`)
* **Descripción:** Interruptor automatizado por eventos sonoros de alta intensidad (aplausos o ruidos secos) para el control de iluminación residencial.
* **Componentes Principales:** Contador de décadas CD4017, Micrófono de electreto (transductor), etapa de preamplificación de señal, Relé electromecánico y carga de CA (Bombilla).
* **Lógica del Circuito:** El micrófono captura la onda sonora y un circuito de acondicionamiento transforma el pico de presión en un pulso digital de reloj (CLK). Este pulso se inyecta al integrado **CD4017**, el cual cambia su estado lógico de salida secuencialmente, conmutando la bobina del relé para abrir o cerrar el circuito de potencia de la luminaria.

### 6. Motion Activated Timer Relay (`Semáforo con integrado`)
* **Descripción:** Sistema de temporización automática para iluminación eficiente basado en detección de movimiento físico.
* **Componentes Principales:** Circuito Integrado Temporizador NE555, Sensor de Movimiento (PIR o interfaz equivalente simulada), capacitores de temporización RC, relé e indicador luminoso.
* **Lógica del Circuito:** Al recibir un flanco de disparo proveniente del sensor de movimiento, el **NE555 configurado en modo monoestable** activa su salida (Output High), energizando el relé y encendiendo la bombilla. El tiempo de encendido está determinado estrictamente por la constante de tiempo $\tau = 1.1 \times R \times C$. El sistema corta el flujo de corriente automáticamente una vez transcurrido el tiempo configurado, optimizando el consumo energético.

---
