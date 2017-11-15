// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#include <DHT.h>
#include <Wire.h>
#include <Arduino.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0,SCL,SDA);

#define DHTPIN 0

#define DHTTYPE DHT22 // DHT 22 (AM2302), AM2321

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE, 11);

struct DHT22_Reading {
  float humidity;
  float temp_c;
  float temp_f;

  float hindex_c;
  float hindex_f;

  unsigned long readmillis;
};

bool read_dht22(struct DHT22_Reading *r) {
  r->humidity = dht.readHumidity();
  r->temp_c   = dht.readTemperature(false);
  r->temp_f   = dht.readTemperature(true);

  if (isnan(r->humidity) || isnan(r->temp_c) || isnan(r->temp_f)) {
    return false;
  }

  r->hindex_c = dht.computeHeatIndex(r->temp_c, r->humidity, false);
  r->hindex_f = dht.computeHeatIndex(r->temp_f, r->humidity, true);

  r->readmillis = millis();

  return true;
}

void display_reading(struct DHT22_Reading *r) {
  char humidity[32] = "Humidty  ";  // for converting float to string
  char temp_f[32]   = "Temp F   ";
  char hif[32]      = "Heat Idx ";
  char sampletime[16];

  
  dtostrf(r->humidity, 2, 2, &humidity[strlen(humidity)]);
  dtostrf(r->temp_f,2,2,&temp_f[strlen(temp_f)]);
  dtostrf(r->hindex_f,2,2,&hif[strlen(hif)]);
  
  sprintf(sampletime,"[%lu]",r->readmillis);
  Serial.println(humidity);
  Serial.println(temp_f);
  Serial.println(hif);
  Serial.println(sampletime);

  u8g2.setFont(u8g2_font_8x13B_mf);
  u8g2.clearBuffer();

  u8g2.drawStr(0,12,humidity);
  u8g2.drawStr(0,28,temp_f);
  u8g2.drawStr(0,40,hif);
  u8g2.drawStr(0,63,sampletime);
  
  u8g2.sendBuffer();
  
}

void setup() {
  Serial.begin(115200);
  Serial.println("\nDHT22 Test!");
  dht.begin();

  u8g2.begin();  
}

void loop() {

  struct DHT22_Reading reading;

  // Wait a few seconds between measurements.
  delay(2100);

  int res = read_dht22(&reading);
  if(!res) {   // Didn't get a reading    
    Serial.println("Failed to read from DHT sensor!");
    return; 
  }
 
  display_reading(&reading);

}
