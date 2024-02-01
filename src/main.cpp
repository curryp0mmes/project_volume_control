#include <Arduino.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <WebServer.h>

#include "index.h"
#include "SecretCredentials.h"
#include "Slider.h"
#include "SpotifyHandler.h"

// ESP32C3 super mini setup
// #define TFT_MISO 5
// #define TFT_MOSI 8
// #define TFT_SCLK 9
// #define TFT_CS    -1 // Not connected
// #define TFT_DC    2
// #define TFT_RST   4
// #define TFT_BL   10
// #define TFT_BACKLIGHT_ON HIGH  // Level to turn ON back-light (HIGH or LOW)

#define POTIPOWER GPIO_NUM_21
#define POTIVAL GPIO_NUM_1
#define MOTOR1 GPIO_NUM_5
#define MOTOR2 GPIO_NUM_3

unsigned long timePiezo;
bool piezoToggle = false;
int maxVal = 0;
int lastSliderVal = 0;
int potiStorage = 1000;

TFT_eSPI tft = TFT_eSPI();
Slider slider = Slider(MOTOR1, MOTOR2, POTIVAL, POTIPOWER);
WebServer server(80);
SpotifyHandler spotifyHandler(tft);

//Web server callbacks
void handleRoot() {
    Serial.println("handling root");
    char page[500];
    sprintf(page,mainPage,SPOTIFY_APP_ID,REDIRECT_URI);
    server.send(200, "text/html", String(page)+"\r\n"); //Send web page
}

void handleCallbackPage() {
    if(!spotifyHandler.accessTokenSet){
        if (server.arg("code") == ""){     //Parameter not found
            char page[500];
            sprintf(page,errorPage,SPOTIFY_APP_ID,REDIRECT_URI);
            server.send(200, "text/html", String(page)); //Send web page
        }else{     //Parameter found
            if(spotifyHandler.getUserCode(server.arg("code"))){
                server.send(200,"text/html","Spotify setup complete Auth refresh in :"+String(spotifyHandler.tokenExpireTime));
            }else{
                char page[500];
                sprintf(page,errorPage,SPOTIFY_APP_ID,REDIRECT_URI);
                server.send(200, "text/html", String(page)); //Send web page
            }
        }
    }else{
        server.send(200,"text/html","Spotify setup complete");
    }
}

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap)
{
    // Stop further decoding as image is running off bottom of screen
    if (y >= tft.height())
        return 0;

    // This function will clip the image block rendering automatically at the TFT boundaries
    tft.pushImage(x, y, w, h, bitmap);

    // Return 1 to decode next block
    return 1;
}

void printWifiStatus()
{
    // print the SSID of the network you're attached to:
    tft.print("SSID: ");
    tft.println(WiFi.SSID());

    // print your board's IP address:
    IPAddress ip = WiFi.localIP();
    tft.print("IP Address: ");
    tft.println(ip);

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    tft.print("signal strength (RSSI):");
    tft.print(rssi);
    tft.println(" dBm");
}

void connectToWifi()
{
    tft.print("Attempting to connect to SSID: ");
    tft.println(WIFI_SSID);

    WiFi.useStaticBuffers(true);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    WiFi.setTxPower(WIFI_POWER_8_5dBm);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        tft.print(".");
    }

    tft.println("");
    tft.println("Connected to WiFi");

    printWifiStatus();
}

void buttonPress()
{
    piezoToggle = !piezoToggle;
    tft.fillCircle(120, 180, 60, piezoToggle ? TFT_RED : TFT_GREEN);
    slider.vibrate();
    int val = slider.getVal();
    delay(1000);
    slider.gotoPos(potiStorage);
    potiStorage = val;

    slider.gotoPos(map(spotifyHandler.getVolume(), 0, 100, 0, 4096));
}

void setup(void)
{

    pinMode(TFT_BL, OUTPUT);
    Serial.begin(9600);

    tft.begin();            // initialize a ST7789 chip
    tft.setSwapBytes(true); // swap the byte order for pushImage() - corrects endianness
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE);

    if (!SPIFFS.begin(true))
    {
        tft.println("SPIFFS initialisation failed!");
        while (1)
            yield(); // Stay here twiddling thumbs waiting
    }

    TJpgDec.setJpgScale(4);

    // The byte order can be swapped (set true for TFT_eSPI)
    TJpgDec.setSwapBytes(true);

    // The decoder must be given the exact name of the rendering function above
    TJpgDec.setCallback(tft_output);

    connectToWifi();

    server.on("/", handleRoot);      //Which routine to handle at root location
    server.on("/callback", handleCallbackPage);      //Which routine to handle at root location
    server.begin();                  //Start server


    tft.fillRect(0, 0, 240, 30, TFT_WHITE);

    timePiezo = millis();
}

void loop()
{
    if(tft.getCursorY() > 300) { 
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0, 10);
    }

    if(!spotifyHandler.accessTokenSet) server.handleClient();

    int piezoVal = analogRead(GPIO_NUM_0);

    unsigned long time = millis();
    if (piezoVal > 1000 && (time - timePiezo) > 100)
    {
        timePiezo = time;
        buttonPress();
    }

    if (time % 50 == 0)
    {
        int potiVal = slider.getVal();
        if (potiVal < lastSliderVal - 20 || potiVal > lastSliderVal + 20)
        {
            lastSliderVal = potiVal;
            float cVal = map(potiVal, 0, 4096, 0, 100) / 100.f;

            tft.fillRoundRect(map(potiVal, 0, 4096, 2, 238), 2, 238, 26, 4, TFT_BLACK);
            tft.fillRoundRect(2, 2, map(potiVal, 0, 4096, 2, 238), 26, 4, cVal > 0.5f ? TFT_GREEN : TFT_RED);

            spotifyHandler.adjustVolume(map(potiStorage,0,4096,0,100));
        }
    }
    delay(1);
}
