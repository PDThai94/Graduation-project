#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

//Config

//Relay config
#define RELAY_IN1 27
#define RELAY_IN2 26

//Led pin config
#define LED_RED 17
#define LED_GREEN 5

//BME280 sensor config
Adafruit_BME280 bme; 
unsigned long delayTime;

//Flame sensor config
#define IR_DO_PIN 0

//MQ7 sensor config
#define MQ7_AO_PIN 16
#define MQ7_DO_PIN 4


void setup() 
{
  Serial.begin(9600);

//Relay setup
pinMode(RELAY_IN1, OUTPUT);
digitalWrite(RELAY_IN1, LOW);
pinMode(RELAY_IN2, OUTPUT);
digitalWrite(RELAY_IN2, LOW);

//-------------------------------------------------------------------------
//  Setup Led
  pinMode(LED_RED, OUTPUT);
  digitalWrite(LED_RED, LOW);
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, LOW);
  
//------------------------------------------------------------------------- 
//  Setup flame sensor
  pinMode(IR_DO_PIN, INPUT);

//-------------------------------------------------------------------------
//  Setup MQ7 sensor
  // set the ADC attenuation to 11 dB (up to ~3.3V input)
  analogSetAttenuation(ADC_11db);
  pinMode(MQ7_DO_PIN, INPUT);
  Serial.println("Warming up the MQ2 sensor");
  
//-------------------------------------------------------------------------
//  Setup bme_280
  bool status;
  status = bme.begin(0x76);  
  if (!status) 
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  
//  Finish setup
//------------------------------------------------------------------------- 
  Serial.println("-- Default Test --");
  delayTime = 1000;

  Serial.println();
}


void loop() 
{ 
  BME_280();
  IR_sensor();
  MQ7_sensor();
  delay(delayTime);
}

void BME_280() 
{
//  Read temperature
  float Temperature = bme.readTemperature();
  Serial.print("Temperature = ");
  Serial.print(Temperature);
  Serial.println(" *C");
  
//  Read humidity
  float Humidity = bme.readHumidity();
  Serial.print("Humidity = ");
  Serial.print(Humidity);
  Serial.println(" %");
  Serial.println();
  if (Temperature > 32)
  {
    Serial.println(" High temperature => The fire warning");
    digitalWrite(RELAY_IN2, HIGH);    
  } 
  else
  {
    Serial.println("Stable temperature");
    digitalWrite(RELAY_IN2, LOW);    
  }
}

void IR_sensor()
{
   int flame_state = digitalRead(IR_DO_PIN);
    if (flame_state == HIGH)
    {
      Serial.println("No flame dected => The fire is NOT detected");
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_GREEN, HIGH);
      digitalWrite(RELAY_IN1, LOW);    
    }
    else
    {
      Serial.println("Flame dected => The fire is detected");
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_RED, HIGH);
      digitalWrite(RELAY_IN1, HIGH);
    }
      
}

void MQ7_sensor()
{
  int gasValue = analogRead(MQ7_AO_PIN);
  int gasState = digitalRead(MQ7_DO_PIN);
//  Analog read value 
  Serial.print("MQ7 sensor AO value: ");
  Serial.println(gasValue);
//  Digital read value
  if (gasState == HIGH)
    Serial.println("The gas is NOT present");
  else
    Serial.println("The gas is present");
}
