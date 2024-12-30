#include <Arduino.h>
#include <Firebase_ESP_Client.h>
#include <esp_now.h>
#include <WiFi.h>


// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Nguyen Tien Hoa"
#define WIFI_PASSWORD "Nguyentienhoa"

// Insert Firebase project API Key
#define API_KEY "AIzaSyAoCKv3BaqZz_mNPUJEgpAzcGT6HbL_r08"

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "loc1852002@gmail.com"
#define USER_PASSWORD "Martini1905"

// Insert RTDB URLefine the RTDB URL
#define DATABASE_URL "https://fire-warning-ca3ca-default-rtdb.asia-southeast1.firebasedatabase.app/"

// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

String uid;


String databasePath;
String tempPath;
String humPath;
String flamePath;
String COPath;

// Cấu trúc dữ liệu nhận từ Node Sensor
typedef struct {
  uint8_t node_id;         // ID của Node
  float temperature;       // Nhiệt độ
  float humidity;          // Độ ẩm
  bool flame_detected;     // Trạng thái phát hiện lửa
  bool co_detected;        // Trạng thái phát hiện khí CO
} SensorData;

SensorData data;

// Timer variables (send new readings every three minutes)
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 10000;


// Initialize WiFi
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

void onDataRecv(const esp_now_recv_info_t *recvInfo, const uint8_t *incomingData, int len) {
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

// Write float values to the database
void sendFloat(String path, float value){
  if (Firebase.RTDB.setFloat(&fbdo, path.c_str(), value)){
    Serial.print("Writing value: ");
    Serial.print (value);
    Serial.print(" on the following path: ");
    Serial.println(path);
    Serial.println("PASSED");
    Serial.print("PATH: ");Serial.println(fbdo.dataPath());
    Serial.print("TYPE: ");Serial.println(fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.print("REASON: ");Serial.println (fbdo.errorReason());
  }
}

void setup(){
  Serial.begin(115200);
  // Initialize BME280 sensor
  initWiFi();
  WiFi.mode(WIFI_STA);

    // Initialize ESP-NOW
  if (esp_now_init() == ESP_OK) 
  {
    Serial.println("ESP-NOW initialized");
  } 
  else 
  {
    Serial.println("ESP-NOW initialization failed");
    return;
  }
  esp_now_register_recv_cb(esp_now_recv_cb_t(onDataRecv));

  // Assign the api key (required)
  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

  // Update database path
  databasePath = "/UsersData/";
  databasePath += uid;

  // Update database path for sensor readings
  tempPath = databasePath;
  tempPath += "/temperature"; // --> UsersData/<user_uid>/temperature
  humPath = databasePath;
  humPath += "/humidity"; // --> UsersData/<user_uid>/humidity
  flamePath = databasePath;
  flamePath += "/pressure"; // --> UsersData/<user_uid>/pressure
  COPath = databasePath;
  COPath += "/CO"; // --> UsersData/<user_uid>/CO
}

void loop(){
  // Send new readings to database
  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

    // Send readings to database:
    sendFloat(tempPath, data.temperature);
    sendFloat(humPath, data.humidity);
    sendFloat(flamePath, data.flame_detected);
    sendFloat(COPath, data.co_detected);
  }
}