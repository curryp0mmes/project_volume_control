#pragma once

#include <HTTPClient.h>
#include <TJpg_Decoder.h>
#include <ArduinoJson.h>
#include <base64.h>
#include <WiFi.h>

#include "SecretCredentials.h"
#include "Logger.h"

class SpotifyHandler
{
public:
    SpotifyHandler(Logger* logger) : logger(logger)
    {
    }
    bool getUserCode(String serverCode);
    bool refreshAuth();
    bool getTrackInfo();
    int getVolume();
    bool findLikedStatus(String songId);
    bool togglePlay();
    bool adjustVolume(int vol);
    bool skipForward();
    bool skipBack();
    struct httpResponse
    {
        int responseCode;
        String responseMessage;
    };

    struct songDetails
    {
        int durationMs;
        String album;
        String artist;
        String song;
        String Id;
        bool isLiked;
    };

    bool accessTokenSet = false;
    long tokenStartTime;
    int tokenExpireTime;
    struct songDetails currentSong;
    float currentSongPositionMs;
    float lastSongPositionMs;
    int currVol;

private:
    Logger* logger;
    HTTPClient https;
    bool isPlaying = false;
    String accessToken;
    String refreshToken;
    bool getFile(String url, String filename);
    String getValue(HTTPClient &http, String key);
};

