#pragma once

#include "SecretCredentials.h"
#include <HTTPClient.h>
#include <TFT_eSPI.h>

class SpotifyHandler{
    public:
        SpotifyHandler();
        bool getSongPlaying(TFT_eSPI *tft);
    private:
        char auth[256];
        HTTPClient http;

};