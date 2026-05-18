# Portafolio
Mis proyectos personales y trabajos enfocados en resolver problemas comunes o simplemente por diversión
--------------------------------------------------------------------------------------------------------

## 🛠️ Proyectos de Sistemas Embebidos e IoT

### 1. Automated Gate Access Control System (`NFC-Access-Control-System`)
* **Descripción:** Sistema automatizado para el control de acceso y apertura de portones mediante autenticación RFID/NFC y gestión de carrera por ultrasonido.
* **Arquitectura de Hardware:** * SoC Principal: ESP32 (gestionando periféricos y lógica de control).
  * Periférico RFID: MFRC522 conectado mediante protocolo SPI.
  * Sensores de Entorno: Sensor ultrasónico de distancia (HC-SR04 o similar).
  * Actuadores e Indicadores: Motor DC de arrastre, módulos de relés para inversión de giro, Buzzer piezoeléctrico y LEDs de estado.
* **Firmware y Lógica de Operación:** El sistema opera bajo una arquitectura bimodal (Encendido/Apagado). Al leer una tarjeta con el UID autorizado, el sistema entra en **Modo Automático**: el sensor ultrasónico monitorea la posición del vehículo u objeto para controlar dinámicamente la apertura y cierre del portón mediante los relés del motor. Al volver a pasar la tarjeta UID, el sistema interrumpe el ciclo y pasa a Modo Suspendido de manera segura.
* **Aplicaciones y Alcances:** Automatización residencial, estacionamientos inteligentes y control de perímetros restringidos.

### 2. Smart Passenger & Object Counter (`Smart-Object-Counter`)
* **Descripción:** Contador bidireccional inteligente de personas u objetos con conectividad inalámbrica, protección industrial y validación por PIN de seguridad.
* **Arquitectura de Hardware:** * Sensores: Dos sensores infrarrojos fotoeléctricos industriales E18-D80NK (alcance de hasta 1 metro).
  * Interfaz de Potencia y Protección: Convertidor DC-DC Buck LM2596 (estabilización de voltaje) y **Diodo de Supresión de Voltaje Transitorio (TVS) de 24V** para la supresión de picos y ruido eléctrico en la alimentación.
  * Comunicación e Interfaz: Módulo Bluetooth (o integrado del SoC) y Buzzer acústico de notificación.
* **Firmware y Lógica de Operación:** La detección bidireccional (entradas/salidas) se procesa mediante una **Máquina de Estados Estructurada (Switch-Case)** que discrimina el orden de activación de los sensores fotoeléctricos. Cuenta con un algoritmo de *antirrebote (debounce)* por software para evitar falsos positivos. Adicionalmente, implementa una rutina de alerta si un sensor es obstruido por tiempo prolongado. El acceso a los datos almacenados desde la aplicación móvil está restringido mediante un firmware de validación por PIN de 4 dígitos.
* **Aplicaciones y Alcances:** Control de aforo en transporte público (buses/metros), auditoría de tiendas retail y automatización industrial en líneas de producción.

### 3. Enterprise Security & IoT Validator (`IoT-Enterprise-Validator`)
* **Descripción:** Dispositivo IoT para la validación de identidad y enlace organizacional en tiempo real conectado a un servidor centralizado.
* **Arquitectura de Hardware:**
  * Procesador: ESP32 (Aprovechando su pila Wi-Fi nativa).
  * Lector RFID/NFC: MFRC522 interconectado por bus de comunicación **SPI**.
  * Notificaciones: Matriz de LEDs indicadores y Buzzer de confirmación.
* **Firmware y Lógica de Operación:** El dispositivo actúa como un cliente IoT. Al leer una tarjeta, el ESP32 empaqueta el identificador único encriptado junto con un **ID único de dispositivo** (firmware que identifica la ubicación exacta del lector) en una trama con formato **JSON**. Esta estructura se transmite mediante peticiones de red hacia un servidor web personal para su validación en bases de datos empresariales.
* **Aplicaciones y Alcances:** Sistemas de cronometraje y asistencia laboral, control de accesos corporativos multi-sede y gestión de membresías en la nube.

### 4. Traffic Light Controller with Audio Accessibility (`Accessible-Traffic-Light`)
* **Descripción:** Controlador de semáforo urbano que integra una interfaz de potencia y un sistema de asistencia por voz para personas con discapacidad visual.
* **Arquitectura de Hardware:**
  * Etapa de Potencia: Optoacopladores y TRIACs/Relés para el manejo de lámparas de semáforo (Verde, Amarillo, Rojo).
  * Módulo de Audio: Transductor/Módulo de reproducción de voz pregrabada (tipo DFPlayer o similar) y altavoz de alta fidelidad.
* **Firmware y Lógica de Operación:** El firmware ejecuta una secuencia cíclica de sincronización vial basada en temporizaciones estructuradas. Al conmutar el estado de las luces de potencia, el microcontrolador envía un disparador al módulo de audio para reproducir comandos de voz claros ("Avance" en verde, "Deténgase" en rojo), garantizando la inclusión en la infraestructura urbana.
* **Aplicaciones y Alcances:** Urbanismo inteligente, entornos urbanos accesibles y mejora de la seguridad vial.

---

## 📐 Diseños y Simulaciones en Proteus

Los siguientes diseños corresponden a circuitos electrónicos analógicos y digitales simulados y validados en el entorno de software **Proteus VSM**.

### 5. Acoustic Actuated Smart Switch (`Acoustic-Smart-Switch`)
* **Descripción:** Interruptor automatizado por eventos sonoros de alta intensidad (aplausos o ruidos secos) para el control de iluminación residencial.
* **Componentes Principales:** Contador de décadas CD4017, Micrófono de electreto (transductor), etapa de preamplificación de señal, Relé electromecánico y carga de CA (Bombilla).
* **Lógica del Circuito:** El micrófono captura la onda sonora y un circuito de acondicionamiento transforma el pico de presión en un pulso digital de reloj (CLK). Este pulso se inyecta al integrado **CD4017**, el cual cambia su estado lógico de salida secuencialmente, conmutando la bobina del relé para abrir o cerrar el circuito de potencia de la luminaria.

### 6. Motion Activated Timer Relay (`Motion-Activated-Timer`)
* **Descripción:** Sistema de temporización automática para iluminación eficiente basado en detección de movimiento físico.
* **Componentes Principales:** Circuito Integrado Temporizador NE555, Sensor de Movimiento (PIR o interfaz equivalente simulada), capacitores de temporización RC, relé e indicador luminoso.
* **Lógica del Circuito:** Al recibir un flanco de disparo proveniente del sensor de movimiento, el **NE555 configurado en modo monoestable** activa su salida (Output High), energizando el relé y encendiendo la bombilla. El tiempo de encendido está determinado estrictamente por la constante de tiempo $\tau = 1.1 \times R \times C$. El sistema corta el flujo de corriente automáticamente una vez transcurrido el tiempo configurado, optimizando el consumo energético.

---
