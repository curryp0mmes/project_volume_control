#include <Arduino.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <WebServer.h>

#include "index.h"
#include "SecretCredentials.h"
#include "Slider.h"
#include "SpotifyHandler.h"
#include "ScreenHandler.h"

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
unsigned long time2Hz;
unsigned long time10Hz;
unsigned long time100Hz;
bool piezoToggle = false;
int maxVal = 0;
int lastSliderVal = 0;
int potiStorage = 1000;

Logger logger = Logger();
ScreenHandler screen = ScreenHandler();
Slider slider = Slider(MOTOR1, MOTOR2, POTIVAL, POTIPOWER);
WebServer server(80);
SpotifyHandler spotify(&logger);

//Web server callbacks
void handleRoot() {
    logger.consolePrintLn("handling root");
    char page[500];
    sprintf(page,mainPage,SPOTIFY_APP_ID,REDIRECT_URI);
    server.send(200, "text/html", String(page)+"\r\n"); //Send web page
}

void handleCallbackPage() {
    if(!spotify.accessTokenSet){
        if (server.arg("code") == ""){     //Parameter not found
            char page[500];
            sprintf(page,errorPage,SPOTIFY_APP_ID,REDIRECT_URI);
            server.send(200, "text/html", String(page)); //Send web page
        }else{     //Parameter found
            if(spotify.getUserCode(server.arg("code"))){
                server.send(200,"text/html","Spotify setup complete Auth refresh in :"+String(spotify.tokenExpireTime));
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



void printWifiStatus()
{
    // print the SSID of the network you're attached to:
    logger.consolePrint("SSID: ");
    logger.consolePrintLn(WiFi.SSID());

    // print your board's IP address:
    IPAddress ip = WiFi.localIP();
    logger.consolePrint("IP Address: ");
    logger.consolePrintLn(String(ip));

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    logger.consolePrint("signal strength (RSSI):");
    logger.consolePrint(String(rssi));
    logger.consolePrintLn(" dBm");
}

void connectToWifi()
{
    logger.consolePrint("Attempting to connect to SSID: ");
    logger.consolePrintLn(WIFI_SSID);
    screen.drawScreen();

    WiFi.useStaticBuffers(true);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    WiFi.setTxPower(WIFI_POWER_8_5dBm);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        logger.consolePrint(".");
    }

    logger.consolePrintLn("");
    logger.consolePrintLn("Connected to WiFi");

    printWifiStatus();
}

void buttonPress()
{
    screen.nextPage();

    piezoToggle = !piezoToggle;
    //tft.fillCircle(120, 180, 60, piezoToggle ? TFT_RED : TFT_GREEN);
    //slider.vibrate();
    int val = slider.getVal();

    //slider.gotoPos(potiStorage);
    slider.gotoPos(1500);
    potiStorage = val;

    //slider.gotoPos(map(spotify.getVolume(), 0, 100, 0, 4096));
}

bool drawImage(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap)
{
    // Stop further decoding as image is running off bottom of screen
    if (y >= screen.getTFT()->height())
        return 0;

    // This function will clip the image block rendering automatically at the TFT boundaries
    screen.getTFT()->pushImage(x, y, w, h, bitmap);

    // Return 1 to decode next block
    return 1;
}

void setup(void)
{
    screen.setup(&logger, &spotify, &slider);
    TJpgDec.setJpgScale(4);

    // The byte order can be swapped (set true for TFT_eSPI)
    TJpgDec.setSwapBytes(true);

    // The decoder must be given the exact name of the rendering function above
    TJpgDec.setCallback(drawImage);
    

    if (!SPIFFS.begin(true))
    {
        logger.consolePrintLn("SPIFFS initialisation failed!");
        screen.drawScreen();
        while (1)
            yield(); // Stay here twiddling thumbs waiting
    }

    int* bounds = slider.calibrate();
    logger.consolePrintLn("LO: " + String(bounds[0]) + " HI: " + String(bounds[1]));
    delete(bounds);

    connectToWifi();

    server.on("/", handleRoot);      //Which routine to handle at root location
    server.on("/callback", handleCallbackPage);      //Which routine to handle at root location
    server.begin();                  //Start server


    //tft.fillRect(0, 0, 240, 30, TFT_WHITE);

    timePiezo = millis();
    time2Hz = millis();
    time10Hz = millis();
    time100Hz = millis();
}

void loop()
{
    unsigned long time = millis();
    if((time - time2Hz) > (1000 / 2)) {
        time2Hz = time;

        /// 2 times per second

        int potiVal = slider.getVal();
        logger.consolePrintLn(String(potiVal));
    }

    if((time - time10Hz) > (1000 / 10)) {
        time10Hz = time;

        /// 10 times per second
        screen.drawScreen();
        if(!spotify.accessTokenSet) server.handleClient();
    }

    if((time - time100Hz) > (1000 / 100)) {
        time100Hz = time;

        /// (up to) 100 times per second

        /// Button press
        int piezoVal = analogRead(GPIO_NUM_0);
        if (piezoVal > 1000 && (time - timePiezo) > 2000)
        {
            timePiezo = time;
            buttonPress();
        }

        slider.update();
    }

}
