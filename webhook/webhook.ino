#include <WiFi.h>
#include <HTTPClient.h>

// WiFi config
const char* ssid = "marker";
const char* password = "mark777za";

// HC-SR04 Pins
#define TRIG_PIN 23
#define ECHO_PIN 22

// LED Pin
#define LED_PIN 2

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  // เชื่อมต่อ WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
}

void loop() {
  // วัดระยะ
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  float distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // เงื่อนไขเปิด/ปิด LED และแสดงสถานะ
  bool ledOn = (distance > 0 && distance < 20);
  if (ledOn) {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED ON");
  } else {
    digitalWrite(LED_PIN, LOW);
    Serial.println("LED OFF");
  }

  // ส่งข้อมูลไป Webhook
  if (duration > 0 && WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // สร้าง URL พร้อมพารามิเตอร์ distance และ led_status
    String url = "https://webhook.site/237ec732-802d-466c-ba68-ec185abbcc87";
    url += "?distance=" + String(distance, 2);
    url += "&led_status=" + String(ledOn ? "ON" : "OFF");

    http.begin(url);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      Serial.print("✅ Sent to webhook. Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("❌ Error sending request: ");
      Serial.println(http.errorToString(httpResponseCode));
    }

    http.end();
  }

  delay(1000); // ส่งทุก 1 วินาที
}
