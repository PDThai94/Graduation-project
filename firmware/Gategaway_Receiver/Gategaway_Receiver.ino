#include <esp_now.h>
#include <WiFi.h>

// Cấu trúc dữ liệu nhận từ Node Sensor
typedef struct {
  uint8_t node_id;         // ID của Node
  float temperature;       // Nhiệt độ
  float humidity;          // Độ ẩm
  bool flame_detected;     // Trạng thái phát hiện lửa
  bool co_detected;        // Trạng thái phát hiện khí CO
} SensorData;

SensorData data;

// Callback khi nhận dữ liệu qua ESP-NOW
void onDataRecv(const esp_now_recv_info *recvInfo, const uint8_t *incomingData, int len) {
  memcpy(&data, incomingData, sizeof(data));

  // Hiển thị dữ liệu trên Serial Monitor
  Serial.println("=============== Thong Tin Sensor ===============");
  Serial.print("Node ID: ");
  Serial.println(data.node_id);

  Serial.print("Nhiet do: ");
  Serial.print(data.temperature);
  Serial.println(" C");

  Serial.print("Do am: ");
  Serial.print(data.humidity);
  Serial.println(" %");

  Serial.print("Lua: ");
  Serial.println(data.flame_detected ? "Co Lua" : "Khong");

  Serial.print("Khi CO: ");
  Serial.println(data.co_detected ? "Nguy hiem" : "An toan");

  Serial.println("===============================================");
}

void setup() {
  Serial.begin(115200);

  // Cấu hình ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  esp_now_register_recv_cb(onDataRecv);
}

void loop() {
  // Gateway chỉ cần nhận và hiển thị dữ liệu, không cần xử lý gì trong loop
}
