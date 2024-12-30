#include <esp_now.h>
#include <WiFi.h>


// Địa chỉ MAC của Node
uint8_t nodeMAC[] = {0xE4, 0x65, 0xB8, 0x58, 0x2E, 0xD0};  // Thay bằng MAC của Node: e4:65:b8:58:2e:d0

typedef struct {
  uint8_t node_id;
  float temperature;
  float humidity;
  bool flame_detected;
  bool co_detected;
} SensorData;

SensorData receivedData;

typedef struct {
  uint8_t check_node;
} Send_check;

Send_check sendData;

void onDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData));
  Serial.println("Data received from Node:");
  Serial.printf("Node ID: %d\n", receivedData.node_id);
  Serial.printf("Temperature: %.2f°C\n", receivedData.temperature);
  Serial.printf("Humidity: %.2f%%\n", receivedData.humidity);
  Serial.printf("Flame Detected: %s\n", receivedData.flame_detected ? "Yes" : "No");
  Serial.printf("CO Detected: %s\n", receivedData.co_detected ? "Yes" : "No");
}

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Request sent successfully" : "Failed to send request");
}

void setup() {
  Serial.begin(115200);

  // Khởi tạo ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  // Đăng ký callback
  esp_now_register_send_cb(onDataSent);
  esp_now_register_recv_cb(onDataRecv);

  // Thêm Node
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, nodeMAC, 6);
  peerInfo.channel = 0;  // Sử dụng kênh mặc định
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add Node");
    return;
  }

  Serial.println("Gateway Initialized.");
}

void loop() {
  uint8_t nodeID = 1;  // Yêu cầu Node với ID 1 gửi dữ liệu
  esp_err_t result = esp_now_send(nodeMAC, &nodeID, sizeof(nodeID));
  if (result != ESP_OK) {
    Serial.printf("Error sending request: %d\n", result);
  }
  delay(2000);  // Chờ 2 giây trước khi gửi yêu cầu tiếp theo
}
