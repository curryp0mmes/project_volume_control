#pragma once

#include "SecretCredentials.h"

class SpotifyHandler{
    public:
        SpotifyHandler();
        char* getSongPlaying();
    private:
        char auth[256];

};