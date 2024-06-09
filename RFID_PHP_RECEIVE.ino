#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <MFRC522.h>

// Define the RFID and LCD pins
#define SS_PIN D4  // SDA / SS
#define RST_PIN D3 // RST
MFRC522 mfrc522(SS_PIN, RST_PIN);

// WiFi credentials
const char* ssid = "Infinix ZERO 30 5G";
const char* password = "Hydrargyrum";

// The URL of your PHP script
const char* serverName = "http://192.168.160.132/NodeMCU-and-RFID-RC522-IoT-Projects/fetch_data.php?id=";

// LCD setup
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  lcd.init();
  lcd.backlight();

  WiFi.begin(ssid, password);

  Serial.println("Connecting to WiFi");
  lcd.setCursor(0, 0);
  lcd.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      String uidStr = "";
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        uidStr += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
        uidStr += String(mfrc522.uid.uidByte[i], HEX);
      }
      uidStr.toUpperCase();

      String url = serverName + uidStr;
      fetchAndDisplayData(url);

      mfrc522.PICC_HaltA();
    }
  }
  delay(1000);
}

void fetchAndDisplayData(String url) {
  HTTPClient http;
  WiFiClient client;
  http.begin(client, url);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println("Received payload:");
    Serial.println(payload);

    // Parse JSON
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);

    const char* message = doc["message"];
    const char* id = doc["id"];
    const char* name = doc["name"];
    const char* gender = doc["gender"];
    const char* email = doc["email"];
    const char* mobile = doc["mobile"];

    // Display data on LCD
    lcd.clear();
    if (message) {
      lcd.setCursor(0, 0);
      lcd.print("Message:");
      lcd.setCursor(0, 1);
      lcd.print(message);
    } else {
      lcd.setCursor(0, 0);
      lcd.print("ID: ");
      lcd.print(id);
      lcd.setCursor(0, 1);
      lcd.print("Name: ");
      lcd.print(name);
      lcd.setCursor(0, 2);
      lcd.print("Gender: ");
      lcd.print(gender);
      lcd.setCursor(0, 3);
      lcd.print("Email: ");
      lcd.print(email);
    }

    // Print data to serial
    Serial.print("ID: ");
    Serial.println(id);
    Serial.print("Name: ");
    Serial.println(name);
    Serial.print("Gender: ");
    Serial.println(gender);
    Serial.print("Email: ");
    Serial.println(email);
    Serial.print("Mobile: ");
    Serial.println(mobile);

  } else {
    Serial.println("Error on HTTP request");
  }

  http.end();
}
