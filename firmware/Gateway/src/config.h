#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

#define LG2 2
#define LY2 0
#define LR2 4

#define LG1 16
#define LY1 17 
#define LR1 5

#define BUZZ1 33
#define BUZZ2 32

// WiFi Credentials
#define WIFI_SSID "Thai04092002"
#define WIFI_PASSWORD "04092002"

// Firebase Configuration
#define API_KEY "AIzaSyARh2E7VKMCTOze2lledQDDdKx1H9sbK7g"
#define USER_EMAIL "Thai04092002@gmail.com"
#define USER_PASSWORD "phamduchuan"
#define DATABASE_URL "https://fire-warning-77a6d-default-rtdb.asia-southeast1.firebasedatabase.app/"

#define NODE_COUNT 2
uint8_t nodeMACs[NODE_COUNT][6] = {
  {0xe4, 0x65, 0xb8, 0x58, 0x2e, 0xd0},  // MAC Node 1 e4:65:b8:58:2e:d0
  {0xd4, 0x8a, 0xfc, 0x5f, 0x87, 0xc8}   // MAC Node 2 d4:8a:fc:5f:87:c8
};
uint8_t currentNode = 0;

#endif