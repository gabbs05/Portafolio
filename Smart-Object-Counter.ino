#include <BluetoothSerial.h> 
#include <ArduinoJson.h>
#include <SevSeg.h>

SevSeg sevseg;
BluetoothSerial ESP_BT;

// --- Configuración de Pines ---
byte segmentPins[] = {16, 17, 18, 19, 21, 22, 23}; //11, 7, 4, 2, 1, 10, 5
byte digitPins[] = {26, 27, 33}; // 12, 9, 8

const int SENSOR_FRONTAL = 25;
const int SENSOR_TRASERO = 13;

const int BUZZER = 4;

// --- Variables del Sistema ---
int counter = 0;
const int MAX_COUNT = 999;

// --- Variables Bluetooth y Seguridad ---
String mensaje = "Ingrese el PIN";
String input;
String password = "68c87fe7069f2f93dd7481f4";
bool authenticated = false;

// --- Variables de la Máquina de Estados (FSM) ---
enum EstadoSecuencia 
{
  IDLE,               // Todo libre
  INICIANDO_SUBIDA,   // S1 activo
  VALIDANDO_SUBIDA,   // S1 + S2 activos (persona en medio)
  FINALIZANDO_SUBIDA, // S2 activo (saliendo)
  BLOQUEO_BAJADA      // Secuencia inversa detectada
};
EstadoSecuencia estadoActual = IDLE;

// --- Gestión de Tiempos ---
unsigned long tiempoInicioActividad = 0; // Para alarma de obstrucción
const unsigned long TIEMPO_LIMITE_OBSTRUCCION = 6000;

unsigned long tiempoInicioBeep = 0;
const unsigned long DURACION_BEEP = 150; // 150ms beep
bool beepActivo = false;
bool alarmaActiva = false;

void setup() 
{
  Serial.begin(115200);

  // 1. Configuración Display
  sevseg.begin(COMMON_CATHODE, 3, digitPins, segmentPins, false);
  sevseg.setBrightness(90);

  // 2. Configuración Sensores
  // Usamos INPUT_PULLUP. Cuando el E18 detecta, conecta a Tierra (LOW).
  // Cuando no detecta, la resistencia interna lo sube a 3.3V (HIGH).
  pinMode(SENSOR_FRONTAL, INPUT_PULLUP);
  pinMode(SENSOR_TRASERO, INPUT_PULLUP);

  // 3. Configuración Buzzer
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  // 4. Inicializar Bluetooth
  if (!ESP_BT.begin("Unidad_1")) 
  {
    Serial.println("Error al inicializar Bluetooth");
  } 
  else 
  {
    Serial.println("Bluetooth Listo: Unidad_1");
  }

  Serial.println("--- SISTEMA INICIADO ---");
}

void loop() 
{
  // ============================================================
  // 1. REFRESCO DE PANTALLA (PRIORIDAD CRÍTICA)
  // ============================================================
  // Esta función debe ejecutarse CADA ciclo sin interrupciones
  sevseg.setNumber(counter, 0);
  sevseg.refreshDisplay();

  // ============================================================
  // 2. LECTURA DE SENSORES
  // ============================================================
  // Los E18 son Active LOW (0 = Detecta, 1 = Libre)
  // Invertimos la lectura (!) para trabajar con lógica positiva (true = objeto)
  bool s1 = !digitalRead(SENSOR_FRONTAL); 
  bool s2 = !digitalRead(SENSOR_TRASERO);

  // ============================================================
  // 3. LÓGICA DE CONTEO (FSM) Y ALARMAS
  // ============================================================
  procesarMaquinaEstados(s1, s2);
  gestionarBuzzer();

  // ============================================================
  // 4. COMUNICACIÓN BLUETOOTH (NO BLOQUEANTE)
  // ============================================================
  gestionarBluetooth();
}

// --------------------------------------------------------------
// FUNCIÓN: MÁQUINA DE ESTADOS (CORE LOGIC)
// --------------------------------------------------------------
void procesarMaquinaEstados(bool frontal, bool trasero) {
  
  // Reset de timer de obstrucción si todo está libre
  if (estadoActual == IDLE) 
  {
    tiempoInicioActividad = millis();
  }

  switch (estadoActual) 
  {
    case IDLE:
      if (frontal && !trasero) 
      {
        estadoActual = INICIANDO_SUBIDA;
      } 
      else if (!frontal && trasero) 
      {
        estadoActual = BLOQUEO_BAJADA;
      }
      break;

    case INICIANDO_SUBIDA:
      if (frontal && trasero) estadoActual = VALIDANDO_SUBIDA;
      else if (!frontal && !trasero) estadoActual = IDLE; // Falsa alarma
      else if (!frontal && trasero) estadoActual = FINALIZANDO_SUBIDA; // Salto rápido
      break;

    case VALIDANDO_SUBIDA:
      if (!frontal && trasero) estadoActual = FINALIZANDO_SUBIDA;
      else if (frontal && !trasero) estadoActual = INICIANDO_SUBIDA; // Regresó
      else if (!frontal && !trasero) 
      { // Salto raro completo
        incrementarContador();
        estadoActual = IDLE;
      }
      break;

    case FINALIZANDO_SUBIDA:
      if (!frontal && !trasero) 
      {
        incrementarContador(); // ¡CONTEO REALIZADO!
        estadoActual = IDLE;
      } 
      else if (frontal && trasero) estadoActual = VALIDANDO_SUBIDA; // Regresó
      break;

    case BLOQUEO_BAJADA:
      if (!frontal && !trasero) 
      {
        estadoActual = IDLE; // Ya bajó, sistema libre
      }
      break;
  }
}

// --------------------------------------------------------------
// FUNCIÓN: INCREMENTO Y FEEDBACK
// --------------------------------------------------------------
void incrementarContador() 
{
  if (counter < MAX_COUNT) 
  {
    counter++;
  } 
  else 
  {
    counter = 0; // Opcional: reiniciar o topar
  }
  
  Serial.print("PASAJERO +1. Total: ");
  Serial.println(counter);

  // Disparar Beep (No bloqueante)
  beepActivo = true;
  tiempoInicioBeep = millis();
  digitalWrite(BUZZER, HIGH);
}

// --------------------------------------------------------------
// FUNCIÓN: GESTOR DE BUZZER (ALARMA + BEEP)
// --------------------------------------------------------------
void gestionarBuzzer() 
{
  unsigned long ahora = millis();

  // A. Detectar Obstrucción (> 6 segundos fuera de IDLE)
  if (estadoActual != IDLE) 
  {
    if (ahora - tiempoInicioActividad > TIEMPO_LIMITE_OBSTRUCCION) 
    {
      if (!alarmaActiva) 
      {
        alarmaActiva = true;
        Serial.println("!!! OBSTRUCCION DETECTADA !!!");
      }
    }
  } 
  else 
  {
    alarmaActiva = false;
  }

  // B. Controlar Salida Física
  if (alarmaActiva) 
  {
    digitalWrite(BUZZER, HIGH); // Sonido continuo (molesto)
  } 
  else if (beepActivo) 
  {
    if (ahora - tiempoInicioBeep >= DURACION_BEEP) 
    {
      digitalWrite(BUZZER, LOW);
      beepActivo = false;
    } 
    else 
    {
      digitalWrite(BUZZER, HIGH);
    }
  } 
  else 
  {
    digitalWrite(BUZZER, LOW);
  }
}

// --------------------------------------------------------------
// FUNCIÓN: GESTOR BLUETOOTH
// --------------------------------------------------------------
void gestionarBluetooth() 
{
  if (ESP_BT.available()) 
  {
    // Nota: readStringUntil puede bloquear ligeramente (timeout),
    // pero es aceptable. Para máxima velocidad usaríamos char buffer.
    ESP_BT.println(mensaje);
    input = ESP_BT.readStringUntil('\n');
    input.trim();
    
    // Serializar JSON
    JsonDocument doc;
    doc["conteo"] = counter;
    doc["Unidad 1"] = password;
    
    if (input.equals(password)) 
    {
        authenticated = true;
        ESP_BT.println("PIN CORRECTO");
        ESP_BT.print("Pasajeros del dia: ");
        ESP_BT.println(counter);
        
        // Enviar JSON bonito
        serializeJsonPretty(doc, ESP_BT);
        ESP_BT.println();
    } 
    else 
    {
        ESP_BT.println("PIN INCORRECTO");
        authenticated = false; 
    }
    // Limpiamos buffer por si acaso
    while(ESP_BT.available()) ESP_BT.read();
  }
}