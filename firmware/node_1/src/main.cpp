#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <esp_now.h>
#include <WiFi.h>

// Cấu hình Node
#define NODE_ID 1  // ID của Node
#define BME280_ADDRESS 0x76

// Cấu hình chân
#define IR_DO_PIN 0
#define MQ7_DO_PIN 4

Adafruit_BME280 bme;
typedef struct {
  uint8_t node_id;
  float temperature;
  float humidity;
  bool flame_detected;
  bool co_detected;
} SensorData;

SensorData data;
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

  Serial.println("Node Initialized.");
}

void loop() {
  if (receivedNodeID == NODE_ID) {
    // Đọc dữ liệu cảm biến
    data.node_id = NODE_ID;
    data.temperature = bme.readTemperature();
    data.humidity = bme.readHumidity();
    data.flame_detected = digitalRead(IR_DO_PIN) == LOW;
    data.co_detected = digitalRead(MQ7_DO_PIN) == LOW;

    // Gửi dữ liệu về Gateway
    esp_now_send(gatewayMAC, (uint8_t *)&data, sizeof(data));
    Serial.println("Data sent to Gateway.");
    receivedNodeID = 0;  // Reset trạng thái nhận yêu cầu
  }
  delay(100);  // Chờ yêu cầu tiếp theo
}
