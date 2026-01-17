#include <WiFi.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

/* ================= PINS ================= */
#define DHT_PIN 7          // ✅ Best GPIO for DHT22
#define DHT_TYPE DHT22

#define SDA_PIN 8
#define SCL_PIN 9

/* ================= OLED ================= */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/* ================= DHT ================= */
DHT dht(DHT_PIN, DHT_TYPE);

/* ================= WIFI ================= */
const char* ssid = "Excitel_HARSHIT_2.4G";
const char* password = "dinesh480tak";

/* ================= FLAGS ================= */
bool otaActive = false;
unsigned long lastRead = 0;

/* ================= SETUP ================= */
void setup() {
  Serial.begin(115200);

  /* ---------- I2C ---------- */
  Wire.begin(SDA_PIN, SCL_PIN);

  /* ---------- OLED ---------- */
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
    while (1);
  }
  oled.setRotation(2);
  oled.setTextColor(SSD1306_WHITE);

  showOLED("BOOTING", "");

  /* ---------- DHT ---------- */
  dht.begin();

  /* ---------- WIFI ---------- */
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(ssid, password);

  showOLED("CONNECTING", "WIFI");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
  }

  showOLED("WIFI", "CONNECTED");

  /* ---------- OTA ---------- */
  ArduinoOTA.setHostname("ESP32-S3-DHT22");
  ArduinoOTA.setTimeout(60000);

  ArduinoOTA.onStart([]() {
    otaActive = true;
    showOLED("OTA", "UPLOADING");
  });

  ArduinoOTA.onEnd([]() {
    otaActive = false;
    showOLED("UPDATE", "DONE");
  });

  ArduinoOTA.onError([](ota_error_t error) {
    otaActive = false;
    showOLED("OTA", "ERROR");
  });

  ArduinoOTA.begin();
}

/* ================= LOOP ================= */
void loop() {
  ArduinoOTA.handle();     // 🔥 NEVER REMOVE
  if (otaActive) return;   // 🔒 OTA LOCK

  if (millis() - lastRead >= 2000) {   // DHT22 needs ≥2s
    lastRead = millis();

    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature(); // Celsius

    displayDHT(temperature, humidity);
  }
}

/* ================= OLED DISPLAY ================= */
void displayDHT(float t, float h) {
  oled.clearDisplay();

  oled.setTextSize(1);
  oled.setCursor(18, 5);
  oled.print("DHT22 SENSOR");

  oled.setTextSize(2);
  oled.setCursor(10, 25);

  if (isnan(t) || isnan(h)) {
    oled.print("Sensor");
    oled.setCursor(10, 45);
    oled.print("Error");
  } else {
    oled.print("T:");
    oled.print(t, 1);
    oled.print(" C");

    oled.setCursor(10, 45);
    oled.print("H:");
    oled.print(h, 1);
    oled.print(" %");
  }

  oled.display();
}

/* ================= OLED HELPER ================= */
void showOLED(const char* l1, const char* l2) {
  oled.clearDisplay();
  oled.setTextSize(2);
  oled.setCursor(10, 20);
  oled.println(l1);
  oled.setCursor(10, 40);
  oled.println(l2);
  oled.display();
}


