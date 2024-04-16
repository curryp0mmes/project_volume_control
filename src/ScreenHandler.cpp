#include "ScreenHandler.h"

void ScreenHandler::setup(Logger* loggerRef, SpotifyHandler* spotifyRef, Slider* sliderRef) {
    logger = loggerRef;
    spotify = spotifyRef;
    slider = sliderRef;

    pinMode(TFT_BL, OUTPUT);
    Serial.begin(9600);

    tft.begin();            // initialize a ST7789 chip
    tft.setSwapBytes(true); // swap the byte order for pushImage() - corrects endianness
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE);

    tft.setCursor(0,10);
    tft.println("Hello World!");
}

void ScreenHandler::drawScreen() {
    switch (currentPage) {
        default:
            currentPage = 0;
        case 0:    
            drawConsolePage();
            break;
        case 1:
            drawSliderPage();
            break;
        case 2:
            drawSpotifyPage();
            break;
    }
}

void ScreenHandler::drawConsolePage() {
    if(logger->update || needsRedraw) {
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(200,0);
        tft.print(String(currentPage) + "/3");
        tft.setCursor(0,0);
        tft.setTextSize(2);
        tft.println("Console");
        tft.setTextSize(1);

        for(int i = 0; i < LOG_LENGTH; i++) {
            tft.println(logger->consoleLog[i]);
        }
    }

    needsRedraw = false;
    logger->update = false;
}

void ScreenHandler::drawSliderPage() {
    if(needsRedraw) {
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(200,0);
        tft.print(String(currentPage) + "/3");
        tft.setCursor(0,0);
        tft.setTextSize(2);
        tft.println("Slider");
        tft.setTextSize(1);
        needsRedraw = false;
    }
    tft.fillRoundRect(map(slider->getVal(), 0, 4096, 2, 238), 102, 238, 26, 4, TFT_BLACK);
    tft.fillRoundRect(2, 102, map(slider->getVal(), 0, 4096, 2, 238), 26, 4, slider->getVal() > 2048 ? TFT_GREEN : TFT_RED);
}

void ScreenHandler::nextPage() {
    currentPage++;
    needsRedraw = true;
}

bool ScreenHandler::drawSpotifyPage(bool fullRefresh, bool likeRefresh)
{
    if(needsRedraw) {
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(200,0);
        tft.print(String(currentPage) + "/3");

        //draw the spotify logo
        tft.fillSmoothCircle(12,12,10,TFT_DARKGREEN,TFT_BLACK);
        tft.drawSmoothArc(12,25,18,16,160,200,TFT_BLACK,TFT_DARKGREEN, true);
        tft.drawSmoothArc(12,29,18,16,160,200,TFT_BLACK,TFT_DARKGREEN, true);
        tft.drawSmoothArc(12,33,18,17,160,200,TFT_BLACK,TFT_DARKGREEN, true);

        needsRedraw = false;
    }

    if (!spotify->accessTokenSet)
    {
        tft.setCursor(10,100);
        tft.print("Please Log in at http://" + WiFi.localIP().toString());
        return true;
    }
    

    int rectWidth = 120;
    int rectHeight = 10;
    if (fullRefresh)
    {
        if (SPIFFS.exists("/albumArt.jpg"))
        {
            TJpgDec.setSwapBytes(true);
            TJpgDec.setJpgScale(4);
            TJpgDec.drawFsJpg(26, 5, "/albumArt.jpg");
        }
        else
        {
            TJpgDec.setSwapBytes(false);
            TJpgDec.setJpgScale(1);
            TJpgDec.drawFsJpg(0, 0, "/Angry.jpg");
        }
        tft.setTextDatum(MC_DATUM);
        tft.setTextWrap(true);
        tft.setCursor(0, 85);
        tft.print(spotify->currentSong.artist);
        // tft.drawString(currentSong.artist, tft.width() / 2, 10);
        tft.setCursor(0, 110);

        tft.print(spotify->currentSong.song);
        // tft.print(currentSong.song);
        // tft.drawString(currentSong.song, tft.width() / 2, 115);
        // tft.drawString(currentSong.song, tft.width() / 2, 125);

        tft.drawRoundRect(
            tft.width() / 2 - rectWidth / 2,
            140,
            rectWidth,
            rectHeight,
            4,
            TFT_DARKGREEN);
    }
    if (fullRefresh || likeRefresh)
    {
        if (spotify->currentSong.isLiked)
        {
            TJpgDec.setJpgScale(1);
            TJpgDec.drawFsJpg(128 - 20, 0, "/heart.jpg");
            //    tft.fillCircle(128-10,10,10,TFT_GREEN);
        }
        else
        {
            tft.fillRect(128 - 21, 0, 21, 21, TFT_BLACK);
        }
    }
    if (spotify->lastSongPositionMs > spotify->currentSongPositionMs)
    {
        tft.fillSmoothRoundRect(
            tft.width() / 2 - rectWidth / 2 + 2,
            140 + 2,
            rectWidth - 4,
            rectHeight - 4,
            10,
            TFT_BLACK);
        spotify->lastSongPositionMs = spotify->currentSongPositionMs;
    }
    tft.fillSmoothRoundRect(
        tft.width() / 2 - rectWidth / 2 + 2,
        140 + 2,
        rectWidth * (spotify->currentSongPositionMs / spotify->currentSong.durationMs) - 4,
        rectHeight - 4,
        10,
        TFT_GREEN);
    // tft.println(currentSongPositionMs);
    // tft.println(currentSong.durationMs);
    // tft.println(currentSongPositionMs/currentSong.durationMs);
    return true;
}