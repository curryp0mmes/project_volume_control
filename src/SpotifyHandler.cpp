#include "SpotifyHandler.h"

bool SpotifyHandler::getUserCode(String serverCode)
{
    https.begin("https://accounts.spotify.com/api/token");
    String auth = "Basic " + base64::encode(String(SPOTIFY_APP_ID) + ":" + String(SPOTIFY_APP_SECRET));
    https.addHeader("Authorization", auth);
    https.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String requestBody = "grant_type=authorization_code&code=" + serverCode + "&redirect_uri=" + String(REDIRECT_URI);
    // Send the POST request to the Spotify API
    int httpResponseCode = https.POST(requestBody);
    // Check if the request was successful
    if (httpResponseCode == HTTP_CODE_OK)
    {
        String response = https.getString();
        JsonDocument doc;
        deserializeJson(doc, response);
        accessToken = String((const char *)doc["access_token"]);
        refreshToken = String((const char *)doc["refresh_token"]);
        tokenExpireTime = doc["expires_in"];
        tokenStartTime = millis();
        accessTokenSet = true;
        logger->consolePrintLn(accessToken);
        logger->consolePrintLn(refreshToken);
    }
    else
    {
        logger->consolePrintLn(https.getString());
    }
    // Disconnect from the Spotify API
    https.end();
    return accessTokenSet;
}

bool SpotifyHandler::refreshAuth()
{
    https.begin("https://accounts.spotify.com/api/token");
    String auth = "Basic " + base64::encode(String(SPOTIFY_APP_ID) + ":" + String(SPOTIFY_APP_SECRET));
    https.addHeader("Authorization", auth);
    https.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String requestBody = "grant_type=refresh_token&refresh_token=" + String(refreshToken);
    // Send the POST request to the Spotify API
    int httpResponseCode = https.POST(requestBody);
    accessTokenSet = false;
    // Check if the request was successful
    if (httpResponseCode == HTTP_CODE_OK)
    {
        String response = https.getString();
        JsonDocument doc;
        deserializeJson(doc, response);
        accessToken = String((const char *)doc["access_token"]);
        // refreshToken = doc["refresh_token"];
        tokenExpireTime = doc["expires_in"];
        tokenStartTime = millis();
        accessTokenSet = true;
        logger->consolePrintLn(accessToken);
        logger->consolePrintLn(refreshToken);
    }
    else
    {
        logger->consolePrintLn(https.getString());
    }
    // Disconnect from the Spotify API
    https.end();
    return accessTokenSet;
}

int SpotifyHandler::getVolume() {
    String url = "https://api.spotify.com/v1/me/player/currently-playing";
    https.useHTTP10(true);
    https.begin(url);
    String auth = "Bearer " + String(accessToken);
    https.addHeader("Authorization", auth);
    int httpResponseCode = https.GET();
    bool success = false;
    String songId = "";
    bool refresh = false;

    // Check if the request was successful
    if (httpResponseCode == HTTP_CODE_OK)
    {
        JsonObject response;
        deserializeJson(response, https.getString());

        return response["device"]["volume_percent"];
    }
    return -1;
}

bool SpotifyHandler::getTrackInfo()
{
    String url = "https://api.spotify.com/v1/me/player/currently-playing";
    https.useHTTP10(true);
    https.begin(url);
    String auth = "Bearer " + String(accessToken);
    https.addHeader("Authorization", auth);
    int httpResponseCode = https.GET();
    bool success = false;
    String songId = "";
    bool refresh = false;

    // Check if the request was successful
    if (httpResponseCode == HTTP_CODE_OK)
    {
        //
        


        String currentSongProgress = getValue(https, "progress_ms");
        currentSongPositionMs = currentSongProgress.toFloat();
        String imageLink = "";
        while (imageLink.indexOf("image") == -1)
        {
            String height = getValue(https, "height");
            // logger->consolePrintLn(height);
            if (height.toInt() > 300)
            {
                imageLink = "";
                continue;
            }
            imageLink = getValue(https, "url");

            // logger->consolePrintLn(imageLink);
        }
        // logger->consolePrintLn(imageLink);

        String albumName = getValue(https, "name");
        String artistName = getValue(https, "name");
        String songDuration = getValue(https, "duration_ms");
        currentSong.durationMs = songDuration.toInt();
        String songName = getValue(https, "name");
        songId = getValue(https, "uri");
        String isPlay = getValue(https, "is_playing");
        isPlaying = isPlay == "true";
        logger->consolePrintLn(isPlay);
        // logger->consolePrintLn(songId);
        songId = songId.substring(15, songId.length() - 1);
        // logger->consolePrintLn(songId);
        https.end();
        // listSPIFFS();
        if (songId != currentSong.Id)
        {

            if (SPIFFS.exists("/albumArt.jpg") == true)
            {
                SPIFFS.remove("/albumArt.jpg");
            }
            // logger->consolePrintLn("trying to get album art");
            bool loaded_ok = getFile(imageLink.substring(1, imageLink.length() - 1).c_str(), "/albumArt.jpg"); // Note name preceded with "/"
            logger->consolePrintLn("Image load was: ");
            logger->consolePrintLn(String(loaded_ok));
            refresh = true;
        }
        currentSong.album = albumName.substring(1, albumName.length() - 1);
        currentSong.artist = artistName.substring(1, artistName.length() - 1);
        currentSong.song = songName.substring(1, songName.length() - 1);
        currentSong.Id = songId;
        currentSong.isLiked = findLikedStatus(songId);
        success = true;
    }
    else
    {
        logger->consolePrint("Error getting track info: ");
        logger->consolePrintLn(String(httpResponseCode));
        // String response = https.getString();
        // logger->consolePrintLn(response);
        https.end();
    }

    // Disconnect from the Spotify API
    if (success)
    {
        lastSongPositionMs = currentSongPositionMs;
    }
    return success;
}

bool SpotifyHandler::findLikedStatus(String songId)
{
    String url = "https://api.spotify.com/v1/me/tracks/contains?ids=" + songId;
    https.begin(url);
    String auth = "Bearer " + String(accessToken);
    https.addHeader("Authorization", auth);
    https.addHeader("Content-Type", "application/json");
    int httpResponseCode = https.GET();
    bool success = false;
    // Check if the request was successful
    if (httpResponseCode == 200)
    {
        String response = https.getString();
        https.end();
        return (response == "[ true ]");
    }
    else
    {
        logger->consolePrint("Error toggling liked songs: ");
        logger->consolePrintLn(String(httpResponseCode));
        String response = https.getString();
        logger->consolePrintLn(response);
        https.end();
    }

    // Disconnect from the Spotify API

    return success;
}

bool SpotifyHandler::togglePlay()
{
    String url = "https://api.spotify.com/v1/me/player/" + String(isPlaying ? "pause" : "play");
    isPlaying = !isPlaying;
    https.begin(url);
    String auth = "Bearer " + String(accessToken);
    https.addHeader("Authorization", auth);
    https.addHeader("Content-length", "0");
    int httpResponseCode = https.PUT("");
    bool success = false;
    // Check if the request was successful
    if (httpResponseCode == 204)
    {
        // String response = https.getString();
        logger->consolePrintLn((isPlaying ? "Playing" : "Pausing"));
        success = true;
    }
    else
    {
        logger->consolePrint("Error pausing or playing: ");
        logger->consolePrintLn(String(httpResponseCode));
        String response = https.getString();
        logger->consolePrintLn(response);
    }

    // Disconnect from the Spotify API
    https.end();
    getTrackInfo();
    return success;
}

bool SpotifyHandler::adjustVolume(int vol)
{
    if(!accessTokenSet) return false;

    String url = "https://api.spotify.com/v1/me/player/volume?volume_percent=" + String(vol);
    https.begin(url);
    String auth = "Bearer " + String(accessToken);
    https.addHeader("Authorization", auth);
    https.addHeader("Content-length", "0");
    int httpResponseCode = https.PUT("");
    bool success = false;
    // Check if the request was successful
    if (httpResponseCode == 204)
    {
        // String response = https.getString();
        currVol = vol;
        success = true;
    }
    else if (httpResponseCode == 403)
    {
        currVol = vol;
        success = false;
        logger->consolePrint("Error setting volume: ");
        logger->consolePrintLn(String(httpResponseCode));
        String response = https.getString();
        logger->consolePrintLn(response);
    }
    else
    {
        logger->consolePrint("Error setting volume: ");
        logger->consolePrintLn(String(httpResponseCode));
        String response = https.getString();
        logger->consolePrintLn(response); 
        logger->consolePrintLn(url);
    }

    // Disconnect from the Spotify API
    https.end();
    return success;
}

bool SpotifyHandler::skipForward()
{
    String url = "https://api.spotify.com/v1/me/player/next";
    https.begin(url);
    String auth = "Bearer " + String(accessToken);
    https.addHeader("Authorization", auth);
    https.addHeader("Content-length", "0");
    int httpResponseCode = https.POST("");
    bool success = false;
    // Check if the request was successful
    if (httpResponseCode == 204)
    {
        // String response = https.getString();
        logger->consolePrintLn("skipping forward");
        success = true;
    }
    else
    {
        logger->consolePrint("Error skipping forward: ");
        logger->consolePrintLn(String(httpResponseCode));
        String response = https.getString();
        logger->consolePrintLn(response);
    }

    // Disconnect from the Spotify API
    https.end();
    getTrackInfo();
    return success;
}
bool SpotifyHandler::skipBack()
{
    String url = "https://api.spotify.com/v1/me/player/previous";
    https.begin(url);
    String auth = "Bearer " + String(accessToken);
    https.addHeader("Authorization", auth);
    https.addHeader("Content-length", "0");
    int httpResponseCode = https.POST("");
    bool success = false;
    // Check if the request was successful
    if (httpResponseCode == 204)
    {
        // String response = https.getString();
        logger->consolePrintLn("skipping backward");
        success = true;
    }
    else
    {
        logger->consolePrint("Error skipping backward: ");
        logger->consolePrintLn(String(httpResponseCode));
        String response = https.getString();
        logger->consolePrintLn(response);
    }

    // Disconnect from the Spotify API
    https.end();
    getTrackInfo();
    return success;
}

bool SpotifyHandler::getFile(String url, String filename) {

  // If it exists then no need to fetch it
  if (SPIFFS.exists(filename) == true) {
    logger->consolePrintLn("Found " + filename);
    return 0;
  }

  logger->consolePrintLn("Downloading "  + filename + " from " + url);

  // Check WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {

    logger->consolePrint("[HTTP] begin...\n");

    HTTPClient http;
    http.begin(url);

    logger->consolePrint("[HTTP] GET...\n");
    // Start connection and send HTTP header
    int httpCode = http.GET();
    if (httpCode > 0) {
      fs::File f = SPIFFS.open(filename, "w+");
      if (!f) {
        logger->consolePrintLn("file open failed");
        return 0;
      }
      // HTTP header has been send and Server response header has been handled
      logger->consolePrintLn("[HTTP] GET... code: " + String(httpCode, DEC));

      // File found at server
      if (httpCode == HTTP_CODE_OK) {

        // Get length of document (is -1 when Server sends no Content-Length header)
        int total = http.getSize();
        int len = total;

        // Create buffer for read
        uint8_t buff[128] = { 0 };

        // Get tcp stream
        WiFiClient * stream = http.getStreamPtr();

        // Read all data from server
        while (http.connected() && (len > 0 || len == -1)) {
          // Get available data size
          size_t size = stream->available();

          if (size) {
            // Read up to 128 bytes
            int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

            // Write it to file
            f.write(buff, c);

            // Calculate remaining bytes
            if (len > 0) {
              len -= c;
            }
          }
          yield();
        }
        logger->consolePrintLn("");
        logger->consolePrintLn("[HTTP] connection closed or file end.");
      }
      f.close();
    }
    else {
      logger->consolePrintLn("[HTTP] GET... failed, error: " + String(http.errorToString(httpCode).c_str()));
    }
    http.end();
  }
  return 1; // File was fetched from web
}

String SpotifyHandler::getValue(HTTPClient &http, String key) {
  bool found = false, look = false, seek = true;
  int ind = 0;
  String ret_str = "";

  int len = http.getSize();
  char char_buff[1];
  WiFiClient * stream = http.getStreamPtr();
  while (http.connected() && (len > 0 || len == -1)) {
    size_t size = stream->available();
    // logger->consolePrint("Size: ");
    // logger->consolePrintLn(size);
    if (size) {
      int c = stream->readBytes(char_buff, ((size > sizeof(char_buff)) ? sizeof(char_buff) : size));
      if (found) {
        if (seek && char_buff[0] != ':') {
          continue;
        } else if(char_buff[0] != '\n'){
            if(seek && char_buff[0] == ':'){
                seek = false;
                int c = stream->readBytes(char_buff, 1);
            }else{
                ret_str += char_buff[0];
            }
        }else{
            break;
        }
          
        // logger->consolePrint("get: ");
        // logger->consolePrintLn(get);
      }
      else if ((!look) && (char_buff[0] == key[0])) {
        look = true;
        ind = 1;
      } else if (look && (char_buff[0] == key[ind])) {
        ind ++;
        if (ind == key.length()) found = true;
      } else if (look && (char_buff[0] != key[ind])) {
        ind = 0;
        look = false;
      }
    }
  }
//   logger->consolePrintLn(*(ret_str.end()));
//   logger->consolePrintLn(*(ret_str.end()-1));
//   logger->consolePrintLn(*(ret_str.end()-2));
  if(*(ret_str.end()-1) == ','){
    ret_str = ret_str.substring(0,ret_str.length()-1);
  }
  return ret_str;
}
