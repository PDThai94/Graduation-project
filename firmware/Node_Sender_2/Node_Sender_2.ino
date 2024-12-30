#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

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

// Cấu trúc dữ liệu gửi qua ESP-NOW
typedef struct {
  uint8_t node_id;         // ID của Node (1 hoặc 2)
  float temperature;       // Nhiệt độ
  float humidity;          // Độ ẩm
  bool flame_detected;     // Trạng thái phát hiện lửa
  bool co_detected;        // Trạng thái phát hiện khí CO
} SensorData;

SensorData data;

// Địa chỉ MAC của Gateway
uint8_t gatewayMAC[] = {0xE8, 0x6B, 0xEA, 0xC9, 0xB7, 0x34};

// Đặt ID cho node
#define NODE_ID 2  // Thay thành 2 cho Node 2

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

  // Kết nối BME280
  if (!bme.begin(BME280_ADDRESS)) {
    Serial.println("Không tìm thấy cảm biến BME280!");
    while (1);
  }

  // Cấu hình ESP-NOW
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // // Tắt chế độ tiết kiệm năng lượng
  if (esp_wifi_set_ps(WIFI_PS_NONE) == ESP_OK) {
    Serial.println("Power Save Mode: NONE (Max Performance)");
  } else {
    Serial.println("Failed to disable Power Save mode");
  }

  // Đặt công suất phát tối đa
  if (esp_wifi_set_max_tx_power(78) == ESP_OK) {
    Serial.println("TX Power: Maximum (19.5 dBm)");
  } else {
    Serial.println("Failed to set TX Power");
  }

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }else {
    Serial.println("ESP-NOW Init Success");
  }


  esp_now_peer_info_t peerInfo = {} ;
  memcpy(peerInfo.peer_addr, gatewayMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // Gán ID cho node
  data.node_id = NODE_ID;
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

  // Gửi dữ liệu qua ESP-NOW
  esp_now_send(gatewayMAC, (uint8_t *)&data, sizeof(data));

  // Chờ 2 giây trước khi lặp lại
  delay(2000);
}
