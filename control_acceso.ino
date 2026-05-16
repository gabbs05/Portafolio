#include <SPI.h>
#include <MFRC522.h>

// --- Definición de Pines  ---
#define SS_PIN    5
#define RST_PIN   21
#define MOTOR_OPEN    16 // Apertura de portón
#define MOTOR_CLOSE   17 // Cierre de portón
#define ECHO    34
#define TRIG    4
#define MOTOR    26  // (Sistema Activo/Ultrasónico)
#define LED_STANDBY 27  // LED Rojo (Sistema Bloqueado)
#define BUZZER    15

// --- Variables Globales ---
MFRC522 mfrc522(SS_PIN, RST_PIN); 
bool sistemaActivo = false; // false = Bloqueado, true = controlado por Ultrasónico
String claveMaestra = "bajenlascuotas"; // La clave que buscamos en la tarjeta

// Clave de autenticación MIFARE (Por defecto FFFFFFFFFFFF)
MFRC522::MIFARE_Key key;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Configuración de pines
  pinMode(ECHO, INPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(MOTOR_OPEN, OUTPUT);
  pinMode(MOTOR_CLOSE, OUTPUT);
  pinMode(MOTOR, OUTPUT);
  pinMode(LED_STANDBY, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  
  // Aseguramos motores apagados al inicio
  digitalWrite(MOTOR_OPEN, LOW);
  digitalWrite(MOTOR_CLOSE, LOW);
  digitalWrite(MOTOR, LOW);

  // Inicialización SPI y RC522 
  SPI.begin(18, 19, 23, SS_PIN);
  mfrc522.PCD_Init();
  
  // Preparamos la key FFFFFFFFFFFF una sola vez
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF; 
  }

  Serial.println(F("--- Sistema de Control de Acceso Iniciado ---"));
  Serial.println(F("Acerque tarjeta maestra para activar/desactivar el sistema."));
}

void loop() {
  // 1. Verificar si hay tarjeta presente para cambiar el estado del sistema
  checkRFID();

  // 2. Si el sistema está ACTIVO, operar con el sensor ultrasónico
  if (sistemaActivo) {
    long distancia = leerUltrasonico();
    
    // Si detecta objeto a menos de 20cm (ajustable)
    if (distancia > 0 && distancia < 20) {
       Serial.println("Objeto detectado! Abriendo portón...");
       accionPorton();
    }
  }
  
  // Pequeña pausa para estabilidad
  delay(100); 
}

// --- Función para leer y validar la tarjeta RFID ---
void checkRFID() {
  // Miramos si hay tarjeta nueva Y si podemos leerla 
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  byte block = 1; 
  byte buffer[18];
  byte size = sizeof(buffer);
  MFRC522::StatusCode status;

  // Autenticar 
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Error Auth: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return; // Salir si falla auth
  }

  // Leer Bloque
  status = mfrc522.MIFARE_Read(block, buffer, &size);
  if (status == MFRC522::STATUS_OK) {
    // Convertir buffer a String para comparar
    String datosLeidos = "";
    for (byte i = 0; i < 16; i++) {
      if(buffer[i] != 0) { // Ignorar espacios vacíos o nulos
         datosLeidos += (char)buffer[i];
      }
    }
    datosLeidos.trim(); // Eliminar espacios en blanco extra

    Serial.print("Datos en tarjeta: ");
    Serial.println(datosLeidos);

    // COMPARACIÓN DE CLAVE
    if (datosLeidos.startsWith(claveMaestra)) {
      sistemaActivo = !sistemaActivo; // Invertir estado (Toggle)
      
      if(sistemaActivo) {
        Serial.println(F(">>> SISTEMA ACTIVADO (Modo Ultrasónico) <<<"));
        digitalWrite(MOTOR, HIGH);
        digitalWrite(LED_STANDBY, LOW);
        sonarBuzzer(1, 500);
        accionPorton();
        // Feedback visual o sonoro opcional aquí
      } else {
        Serial.println(F(">>> SISTEMA DESACTIVADO (Standby) <<<"));
        digitalWrite(MOTOR, LOW);
        digitalWrite(LED_STANDBY, HIGH);
        sonarBuzzer(2, 200);
      }
      delay(1000); // Evitar lecturas múltiples por rebote
    } else {
      Serial.println(F("Acceso Denegado: Clave incorrecta."));
      sonarBuzzer(3, 100);
    }
  } else {
    Serial.print(F("Error Leer: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }

  // Finalizar comunicación con tarjeta
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

// --- Función para leer el HC-SR04 ---
long leerUltrasonico() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  
  long duration = pulseIn(ECHO, HIGH, 30000); // Timeout de 30ms para no bloquear
  
  // Fórmula: Distancia = (Tiempo * VelocidadSonido) / 2
  // Velocidad del sonido aprox 0.034 cm/us
  long distance = duration * 0.034 / 2;
  
  return distance;
}

// --- Secuencia de movimiento del motor ---
// --- Secuencia de movimiento del motor con SEGURIDAD ---
void accionPorton() {
  Serial.println("Abriendo...");
  digitalWrite(MOTOR_OPEN, HIGH);
  digitalWrite(MOTOR_CLOSE, LOW);
  delay(2000); // Tiempo abriendo

  // FRENAR
  digitalWrite(MOTOR_OPEN, LOW);
  Serial.println("Abierto. Esperando despeje...");

  // --- LÓGICA DE SEGURIDAD ---
  // Mantenemos el portón abierto SIEMPRE que:
  // 1. No hayan pasado 4 segundos DESDE que se detectó el último obstáculo.
  // 2. O si hay un obstáculo presente ahora mismo.
  
  unsigned long tiempoInicio = millis();
  
  while (millis() - tiempoInicio < 4000) {
    long distancia = leerUltrasonico();
    
    // Si hay un objeto cerca (menos de 20cm), REINICIAMOS el contador
    if (distancia > 0 && distancia < 20) {
      Serial.println("¡Obstáculo! Reiniciando tiempo de espera...");
      tiempoInicio = millis(); // El tiempo vuelve a empezar de 0
    }
    
    delay(200); // Pequeña pausa para no saturar el sensor
  }
  // ---------------------------
  
  Serial.println("Zona despejada. Cerrando...");
  digitalWrite(MOTOR_CLOSE, HIGH);
  digitalWrite(MOTOR_OPEN, LOW);
  delay(2000); // Tiempo cerrando
  
  digitalWrite(MOTOR_CLOSE, LOW);
  Serial.println("Portón Cerrado.");
}

void sonarBuzzer(int veces, int duracion) {
  for (int i = 0; i < veces; i++) {
    digitalWrite(BUZZER, HIGH);
    delay(duracion);
    digitalWrite(BUZZER, LOW);
    // Si hay más de un pitido, necesitamos una pausa entre ellos
    if (veces > 1) {
      delay(100); // Pausa de silencio entre beeps
    }
  }
}