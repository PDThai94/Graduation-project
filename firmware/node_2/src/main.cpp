#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <esp_now.h>
#include <WiFi.h>

// Cấu hình Node
#define NODE_ID 2  // ID của Node

// Cấu hình chân cảm biến và ngoại vi
#define RELAY_IN1 27
#define RELAY_IN2 26
#define LED_RED 17
#define LED_GREEN 5
#define IR_DO_PIN 0
#define MQ7_AO_PIN 16
#define MQ7_DO_PIN 4


// Địa chỉ I2C của BME280
#define BME280_ADDRESS 0x76
Adafruit_BME280 bme;

// Cấu hình dữ liệu cảm biến
typedef struct {
  uint8_t node_id;
  float temperature;
  float humidity;
  bool flame_detected;
  bool co_detected;
} SensorData;

SensorData data;

// Cấu hình địa chỉ MAC của Gateway
uint8_t gatewayMAC[] = {0xE8, 0x6B, 0xEA, 0xC9, 0xB7, 0x34};  // Thay bằng MAC của Gateway
uint8_t receivedNodeID = 0;

void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&receivedNodeID, incomingData, sizeof(receivedNodeID));
  Serial.printf("Received request for Node ID: %d\n", receivedNodeID);
}

void setup() {
  Serial.begin(115200);

  // Cấu hình chân
  pinMode(IR_DO_PIN, INPUT);
  pinMode(MQ7_DO_PIN, INPUT);

  // Khởi tạo cảm biến BME280
  if (!bme.begin(BME280_ADDRESS)) {
    Serial.println("BME280 not found!");
    while (1);
  }

  // Khởi tạo ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  // Đăng ký callback
  esp_now_register_recv_cb(onDataRecv);

  // Thêm Gateway làm peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, gatewayMAC, 6);
  peerInfo.channel = 0;  // Sử dụng kênh mặc định
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add Gateway");
    return;
  }

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

  Serial.println("Node Initialized.");
    // Gán ID cho node
  data.node_id = NODE_ID;
}

void loop() {
  if (receivedNodeID == NODE_ID) {
    // Đọc dữ liệu cảm biến
    data.node_id = NODE_ID;
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

    // Gửi dữ liệu về Gateway
    esp_now_send(gatewayMAC, (uint8_t *)&data, sizeof(data));
    Serial.println("Data sent to Gateway.");
    receivedNodeID = 0;  // Reset trạng thái nhận yêu cầu
  }
  delay(100);  // Chờ yêu cầu tiếp theo
}
