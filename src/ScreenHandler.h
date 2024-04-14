#pragma once

#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include "SpotifyHandler.h"
#include "Slider.h"
#include "Logger.h"



class ScreenHandler
{
public:
    void setup(Logger* loggerRef, SpotifyHandler* spotifyRef, Slider* sliderRef);
    void nextPage();
    void prevPage();
    void drawScreen();
    void drawConsolePage();
    void drawSliderPage();
    bool drawSpotifyPage(bool fullRefresh = false, bool likeRefresh = false);

    TFT_eSPI* getTFT() {
        return &tft;
    }
private:
    TFT_eSPI tft = TFT_eSPI();
    bool needsRedraw = false;
    int currentPage = 0;

    Logger* logger;
    SpotifyHandler* spotify;
    Slider* slider;
};
