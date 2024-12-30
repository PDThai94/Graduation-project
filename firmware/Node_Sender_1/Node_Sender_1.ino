#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

// Cấu hình Node
#define NODE_ID 1  // Thay đổi ID cho từng Node: 1, 2, ...
#define BME280_ADDRESS 0x76

// Cấu hình chân
#define RELAY_IN1 27
#define RELAY_IN2 26
#define LED_RED 17
#define LED_GREEN 5
#define IR_DO_PIN 0
#define MQ7_AO_PIN 16
#define MQ7_DO_PIN 4

uint8_t receivedNodeID = 0;

// Khai báo cảm biến và cấu trúc dữ liệu
Adafruit_BME280 bme;
typedef struct {
  uint8_t node_id;
  float temperature;
  float humidity;
  bool flame_detected;
  bool co_detected;
} SensorData;

SensorData data;
uint8_t gatewayMAC[] = {0xE8, 0x6B, 0xEA, 0xC9, 0xB7, 0x34};  // Địa chỉ Gateway

void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&receivedNodeID, incomingData, sizeof(receivedNodeID));
  Serial.print("Received Node ID: ");
  Serial.println(receivedNodeID);
}

void setup() {
  Serial.begin(115200);
  // Cấu hình ngoại vi
  pinMode(RELAY_IN1, OUTPUT);
  pinMode(RELAY_IN2, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(IR_DO_PIN, INPUT);
  pinMode(MQ7_DO_PIN, INPUT);

  // Mặc định trạng thái ban đầu
  digitalWrite(RELAY_IN1, HIGH);
  digitalWrite(RELAY_IN2, HIGH);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, HIGH);  // Đèn xanh bật khi an toàn

  if (!bme.begin(BME280_ADDRESS)) {
    Serial.println("BME280 not found!");
    while (1);
  }

  // Khởi tạo ESP-NOW
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Tắt chế độ tiết kiệm năng lượng và đặt công suất phát tối đa
  if (esp_wifi_set_ps(WIFI_PS_NONE) == ESP_OK) {
    Serial.println("Power Save Mode: NONE (Max Performance)");
  } else {
    Serial.println("Failed to disable Power Save mode");
  }

  if (esp_wifi_set_max_tx_power(78) == ESP_OK) {
    Serial.println("TX Power: Maximum (19.5 dBm)");
  } else {
    Serial.println("Failed to set TX Power");
  }

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }
  esp_now_register_recv_cb(esp_now_recv_cb_t(onDataRecv));  // Đăng ký callback nhận dữ liệu

  Serial.println("Node Initialized. Waiting for Gateway requests...");
}


void loop() {
  // Đọc dữ liệu cảm biến
  data.temperature = bme.readTemperature();
  data.humidity = bme.readHumidity();
  data.flame_detected = digitalRead(IR_DO_PIN) == LOW;
  data.co_detected = digitalRead(MQ7_DO_PIN) == LOW;

  // Điều khiển ngoại vi dựa trên dữ liệu cảm biến
  if (data.flame_detected && data.co_detected) {
    // Kích hoạt đèn đỏ và relay
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(RELAY_IN1, LOW);
    digitalWrite(RELAY_IN2, LOW);
  } else {
    // Trạng thái an toàn
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(RELAY_IN1, HIGH);
    digitalWrite(RELAY_IN2, HIGH);
  }

  if (receivedNodeID == NODE_ID) {
    Serial.print("Request from Gateway. Sending data... ");
    // Gửi dữ liệu về Gateway
    esp_now_send(gatewayMAC, (uint8_t *)&data, sizeof(data));
    Serial.println("Data sent.");
    receivedNodeID = 0;
  } else {
    Serial.println("Node ID does not match. Waiting for request...");
  }

  delay(100);  // Chờ Gateway gửi yêu cầu
}
