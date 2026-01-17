#include <WiFi.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WebServer.h>
#include "DHT.h"

/* ========= PIN ========= */
#define SDA_PIN 21
#define SCL_PIN 22
#define DHT_PIN 27
#define DHT_TYPE DHT22   

/* ========= LCD ========= */
LiquidCrystal_I2C lcd(0x27, 20, 4);

/* ========= DHT ========= */
DHT dht(DHT_PIN, DHT_TYPE);

/* ========= WIFI ========= */
const char* ssid = "Excitel_HARSHIT_2.4G";
const char* password = "dinesh480tak";

/* ========= WEB ========= */
WebServer server(80);

/* ========= VAR ========= */
bool otaEnabled = false;
unsigned long lastRead = 0;
float temperature = 0.0;
float humidity = 0.0;

/* ========= WEB PAGE ========= */
void handleRoot() {
  String html = "<h2>ESP32 PRACTICAL STATUS</h2>";
  html += "<p><b>IP:</b> " + WiFi.localIP().toString() + "</p>";
  html += "<p><b>WiFi:</b> ";
  html += (WiFi.status() == WL_CONNECTED) ? "Connected</p>" : "Disconnected</p>";
  html += "<p><b>OTA:</b> ";
  html += otaEnabled ? "ON</p>" : "OFF</p>";
  html += "<p><b>Practical:</b> DHT22</p>";
  html += "<p><b>Temperature:</b> " + String(temperature, 1) + " °C</p>";
  html += "<p><b>Humidity:</b> " + String(humidity, 1) + " %</p>";

  server.send(200, "text/html", html);
}

/* ================= SETUP ================= */
void setup() {
  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();

  dht.begin();   // ✅ DHT22 init

  /* ---- WIFI ---- */
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(ssid, password);

  lcd.setCursor(0,0);
  lcd.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
  }

  /* ---- OTA ---- */
  ArduinoOTA.setHostname("ESP32-DHT22");
  ArduinoOTA.begin();
  otaEnabled = true;

  /* ---- WEB ---- */
  server.on("/", handleRoot);
  server.begin();
}

/* ================= LOOP ================= */
void loop() {
  ArduinoOTA.handle();
  server.handleClient();

  readDHT();
  updateLCD();
}

/* ================= DHT READ ================= */
void readDHT() {
  if (millis() - lastRead < 2500) return;  // ✅ DHT22 needs slower read
  lastRead = millis();

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (!isnan(h) && !isnan(t)) {
    humidity = h;
    temperature = t;
  }
}

/* ================= LCD UPDATE ================= */
void updateLCD() {
  static unsigned long lastLCD = 0;
  if (millis() - lastLCD < 1000) return;
  lastLCD = millis();

  lcd.clear();

  // Row 1
  lcd.setCursor(0,0);
  lcd.print("IP:");
  lcd.print(WiFi.localIP());

  // Row 2
  lcd.setCursor(0,1);
  lcd.print("OTA:");
  lcd.print(otaEnabled ? "ON " : "OFF");
  lcd.print(" WiFi:");
  lcd.print(WiFi.status() == WL_CONNECTED ? "ON" : "OFF");

  // Row 3
  lcd.setCursor(0,2);
  lcd.print("Practical:DHT22");

  // Row 4
  lcd.setCursor(0,3);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print("C H:");
  lcd.print(humidity, 1);
  lcd.print("%");
}


