#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Pin Definitions
const int LM35_PIN = A0;
const int LDR_PIN = A1;
const int DHT_PIN = 3;
const int WATER_LEVEL_PIN = A2; // Changed to analog input pin
const int RELAY_PIN = 5;
const int LED_PIN = 2;
const int BUZZER_PIN = 6; // New buzzer pin

#define DHT_TYPE DHT11
#define WATER_LEVEL_THRESHOLD 500  // Adjust this value as needed

// Sensor and Display Initialization
DHT dht(DHT_PIN, DHT_TYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // Set Pin Modes
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Initialize Display and Sensors
  lcd.init();
  lcd.backlight();
  dht.begin();
  Serial.begin(9600);

  // Startup Message
  lcd.setCursor(0, 0);
  lcd.print("System Initializing");
  delay(2000);
  lcd.clear();
}

void loop() {
  float temperature = readLM35Temperature();
  float humidity = readHumidity();
  int ldrValue = readLDRValue();
  int waterLevel = analogRead(WATER_LEVEL_PIN);

  displayTemperatureHumidity(temperature, humidity);
  controlLightBasedOnLDR(ldrValue);
  controlRelayBasedOnWaterLevel(waterLevel);
  controlBuzzerBasedOnWaterLevel(waterLevel);

  logToSerial(ldrValue, waterLevel);

  delay(1000);
}

float readLM35Temperature() {
  int total = 0;
  for (int i = 0; i < 10; i++) {
    total += analogRead(LM35_PIN);
    delay(10);
  }
  float average = total / 10.0;
  float voltage = average * (5.0 / 1023.0);
  return voltage * 100.0; // LM35 conversion
}

float readHumidity() {
  float h = dht.readHumidity();
  return isnan(h) ? 0 : h;
}

int readLDRValue() {
  int total = 0;
  for (int i = 0; i < 10; i++) {
    total += analogRead(LDR_PIN);
    delay(5);
  }
  return total / 10;
}

void controlLightBasedOnLDR(int ldrValue) {
  digitalWrite(LED_PIN, ldrValue > 150 ? LOW : HIGH);
}

void controlRelayBasedOnWaterLevel(int waterLevel) {
  digitalWrite(RELAY_PIN, waterLevel > WATER_LEVEL_THRESHOLD ? HIGH : LOW);
}

void controlBuzzerBasedOnWaterLevel(int waterLevel) {
  digitalWrite(BUZZER_PIN, waterLevel > WATER_LEVEL_THRESHOLD ? HIGH : LOW);
}

void displayTemperatureHumidity(float temp, float humidity) {
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temp, 1);
  lcd.print((char)223);
  lcd.print("C   ");

  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity, 0);
  lcd.print("%   ");
}

void logToSerial(int ldrValue, int waterLevel) {
  Serial.print("LDR: ");
  Serial.print(ldrValue);
  Serial.print(" | Water Level: ");
  Serial.print(waterLevel);
  Serial.print(" | Status: ");
  Serial.println(waterLevel > WATER_LEVEL_THRESHOLD ? "HIGH" : "LOW");
}
