#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <SPI.h>
#include <MFRC522.h>

// --- Definición de pines (Basado en tu tabla) ---
#define SS_PIN    5    
#define RST_PIN   21   

MFRC522 mfrc522(SS_PIN, RST_PIN);

// --- Configuración de Red Wi-Fi ---
const char* ssid = "Fenix_Tenda";
const char* password = "Mazda3+*";

// --- Configuración del Servidor Remoto ---
String serverURL = "https://secure-pass-nfc.davidvillamizar.com/api/code?id=";

void setup() {
  Serial.begin(115200);
  while (!Serial); // Espera a que se inicie el Monitor Serial
  
  // Inicializa el bus SPI con los pines: SCK=18, MISO=19, MOSI=23, SS=5
  SPI.begin(18, 19, 23, SS_PIN);
  mfrc522.PCD_Init();
  
  Serial.println("-----------------------------------");
  Serial.println("Iniciando Sistema Secure Pass...");
  Serial.println("RFID RC522 inicializado.");

  // Conexión Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n¡Conectado a la red Wi-Fi exitosamente!");
  Serial.println("Esperando tarjeta...");
  Serial.println("-----------------------------------");
}

void loop() {
  // 1. Revisar si hay una nueva tarjeta presente
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return; // Si no hay tarjeta, vuelve a iniciar el loop
  }
  
  // 2. Preparar la lectura del Bloque 1
  byte block = 1; 
  byte buffer[18];  
  byte size = sizeof(buffer);
  
  // Clave por defecto de fábrica: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  
  // 3. Autenticar acceso al bloque
  MFRC522::StatusCode status;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  
  if (status != MFRC522::STATUS_OK) {
    Serial.print("❌ Error en autenticación: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    mfrc522.PICC_HaltA();
    return;
  }
  
  // 4. Leer los datos del bloque
  status = mfrc522.MIFARE_Read(block, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("❌ Error leyendo tarjeta: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
  } else {
    // 5. Procesar los datos leídos
    String claveLeida = "";
    for (byte i = 0; i < 16; i++) {
      // Evitamos concatenar caracteres nulos o basura de la memoria vacía
      if(buffer[i] != 0 && buffer[i] != 255) {
        claveLeida += (char)buffer[i];
      }
    }
    
    claveLeida.trim(); // Limpiamos espacios en blanco extra
    
    Serial.print("\n🔑 Clave extraída de la tarjeta: ");
    Serial.println(claveLeida);
    
    // 6. Enviar al servidor si la clave es válida
    if(claveLeida.length() > 0) {
      verificarAccesoRemoto(claveLeida);
    } else {
      Serial.println("⚠️ El bloque de la tarjeta está vacío.");
    }
  }
  
  // Finalizar comunicación con la tarjeta actual
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  
  // Pausa de 3 segundos para evitar lecturas múltiples accidentales
  delay(3000); 
}

// --- Función para la petición HTTPS ---
void verificarAccesoRemoto(String clave) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure *client = new WiFiClientSecure;
    
    // Omitimos la validación estricta del certificado SSL para agilizar pruebas
    client->setInsecure(); 
    
    HTTPClient http;
    String finalURL = serverURL + clave;
    
    Serial.print("🌐 Enviando datos a: ");
    Serial.println(finalURL);
    
    // Iniciar conexión HTTPS
    if (http.begin(*client, finalURL)) {
      int httpResponseCode = http.GET(); 
      
      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.print("✅ Código HTTP: ");
        Serial.println(httpResponseCode);
        Serial.print("📥 Respuesta del servidor: ");
        Serial.println(response);
      } else {
        Serial.print("❌ Error en la petición HTTPS: ");
        Serial.println(http.errorToString(httpResponseCode).c_str());
      }
      http.end(); // Cerrar conexión
    } else {
      Serial.println("❌ Error: No se pudo establecer la conexión con el servidor.");
    }
    
    delete client; // Liberar memoria
  } else {
    Serial.println("⚠️ Error: El ESP32 perdió la conexión Wi-Fi.");
  }
}