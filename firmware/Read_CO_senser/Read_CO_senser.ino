/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-gas-sensor
 */

#define AO_PIN 4  // ESP32's pin GPIO4 connected to AO pin of the MQ2 sensor

void setup() {
  // initialize serial communication
  Serial.begin(9600);

  // set the ADC attenuation to 11 dB (up to ~3.3V input)
  analogSetAttenuation(ADC_11db);
  Serial.println("Warming up the MQ7 sensor");
  delay(20000);  // wait for the MQ7 to warm up
}

void loop() {
  int gasValue = analogRead(AO_PIN);

  Serial.print("MQ7 sensor AO value: ");
  Serial.println(gasValue);
}
