#include <TFT_eSPI.h>
#include <WiFi.h>

// ESP32C3 super mini setup
//#define TFT_MISO 5
//#define TFT_MOSI 8
//#define TFT_SCLK 9
//#define TFT_CS    -1 // Not connected
//#define TFT_DC    2
//#define TFT_RST   4  // Connect reset to ensure display initialises
//#define TFT_BL   10            // LED back-light control pi
//#define TFT_BACKLIGHT_ON HIGH  // Level to turn ON back-light (HIGH or LOW)



TFT_eSPI tft = TFT_eSPI();


void toggleBacklight() {
  digitalWrite(TFT_BL, !digitalRead(TFT_BL));
}

void setup(void) {
  pinMode(TFT_BL, OUTPUT);
  Serial.begin(9600);
  Serial.println("ST7789 TFT WiFi Scan Test");

  tft.begin();                                 // initialize a ST7789 chip
  tft.setSwapBytes(true);                      // swap the byte order for pushImage() - corrects endianness
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);

  toggleBacklight();  // Schakel backlight in
}


void displayWiFiScanResults() {
  int n = WiFi.scanNetworks();

  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  
  if (n == 0) {
    tft.setCursor(10, 10);
    tft.println("No networks found");
  } else {
    tft.setCursor(10, 10);
    tft.print(n);
    tft.println(" networks found");

    for (int i = 0; i < n; ++i) {
      tft.print(i + 1);
      tft.print(": ");
      tft.print(WiFi.SSID(i));
      tft.print(" (");
      tft.print(WiFi.RSSI(i));
      tft.print(")");
      tft.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
    }
  }
  
}

void loop() {
//  delay(1000);
//  toggleBacklight();  // Schakel backlight in
//  delay(1000);
//  toggleBacklight();  // Schakel backlight uit

  displayWiFiScanResults();  // Toon WiFi-scanresultaten op het LCD-scherm

  // Wacht een tijdje voordat je opnieuw scant
  delay(5000);
}
