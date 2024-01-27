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

unsigned long timePiezo;
bool piezoToggle = false;
int maxVal = 0;

TFT_eSPI tft = TFT_eSPI();

void displayWiFiScanResults() {
  int n = WiFi.scanNetworks();

  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  
  if (n == 0) {
    tft.setCursor(10, 10);
    tft.println("No networks found");
  } else {
    tft.setCursor(10, 10);
    tft.print(n);
    tft.println(" networks found");

    for (int i = 0; i < n && i < 5; ++i) {
      tft.print(i + 1);
      tft.print(": ");
      tft.print(WiFi.SSID(i));
      tft.print(" (");
      tft.setTextColor(TFT_RED);
      tft.print(WiFi.RSSI(i));
      tft.setTextColor(TFT_WHITE);
      tft.print(")");
      tft.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
    }
  }
  
}

void setup(void) {
  timePiezo = millis();
  pinMode(TFT_BL, OUTPUT);
  Serial.begin(9600);
  Serial.println("ST7789 TFT WiFi Scan Test");

  tft.begin();                                 // initialize a ST7789 chip
  tft.setSwapBytes(true);                      // swap the byte order for pushImage() - corrects endianness
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  displayWiFiScanResults(); 
}




void loop() {

  int val = analogRead(GPIO_NUM_0);

  unsigned long time = millis();
  if(val > 1000 && (time - timePiezo) > 100) {
    timePiezo = time;
    piezoToggle = !piezoToggle;
    tft.fillCircle(120, 180, 60, piezoToggle ? TFT_RED : TFT_GREEN);
  }

  
   
  

  delay(1);
/*
  tft.setCursor(10, 10);

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  if(WiFi.isConnected()) {
    tft.println("Connected");
    tft.fillRect(0,150,240,240, TFT_GREEN);
  }
  else {
    tft.println("Disconnected");
    tft.fillRect(0,150,240,240, TFT_RED);
  }
  int status = WiFi.status();
  if(status == 0) {
    WiFi.begin(WiFi.SSID(0));
  }
  tft.println(status);
  tft.setTextColor(TFT_GREEN);
  tft.println(WiFi.localIP());
  tft.println(WiFi.localIPv6());
  delay(2000);
  */
}
