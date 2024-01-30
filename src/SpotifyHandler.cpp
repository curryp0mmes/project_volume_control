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
        tft.println(accessToken);
        tft.println(refreshToken);
    }
    else
    {
        tft.println(https.getString());
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
        tft.println(accessToken);
        tft.println(refreshToken);
    }
    else
    {
        tft.println(https.getString());
    }
    // Disconnect from the Spotify API
    https.end();
    return accessTokenSet;
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
            // tft.println(height);
            if (height.toInt() > 300)
            {
                imageLink = "";
                continue;
            }
            imageLink = getValue(https, "url");

            // tft.println(imageLink);
        }
        // tft.println(imageLink);

        String albumName = getValue(https, "name");
        String artistName = getValue(https, "name");
        String songDuration = getValue(https, "duration_ms");
        currentSong.durationMs = songDuration.toInt();
        String songName = getValue(https, "name");
        songId = getValue(https, "uri");
        String isPlay = getValue(https, "is_playing");
        isPlaying = isPlay == "true";
        tft.println(isPlay);
        // tft.println(songId);
        songId = songId.substring(15, songId.length() - 1);
        // tft.println(songId);
        https.end();
        // listSPIFFS();
        if (songId != currentSong.Id)
        {

            if (SPIFFS.exists("/albumArt.jpg") == true)
            {
                SPIFFS.remove("/albumArt.jpg");
            }
            // tft.println("trying to get album art");
            bool loaded_ok = getFile(imageLink.substring(1, imageLink.length() - 1).c_str(), "/albumArt.jpg"); // Note name preceded with "/"
            tft.println("Image load was: ");
            tft.println(loaded_ok);
            refresh = true;
            tft.fillScreen(TFT_BLACK);
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
        tft.print("Error getting track info: ");
        tft.println(httpResponseCode);
        // String response = https.getString();
        // tft.println(response);
        https.end();
    }

    // Disconnect from the Spotify API
    if (success)
    {
        drawScreen(refresh);
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
        tft.print("Error toggling liked songs: ");
        tft.println(httpResponseCode);
        String response = https.getString();
        tft.println(response);
        https.end();
    }

    // Disconnect from the Spotify API

    return success;
}
bool SpotifyHandler::drawScreen(bool fullRefresh, bool likeRefresh)
{
    int rectWidth = 120;
    int rectHeight = 10;
    if (fullRefresh)
    {
        if (SPIFFS.exists("/albumArt.jpg") == true)
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
        tft.print(currentSong.artist);
        // tft.drawString(currentSong.artist, tft.width() / 2, 10);
        tft.setCursor(0, 110);

        tft.print(currentSong.song);
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
        if (currentSong.isLiked)
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
    if (lastSongPositionMs > currentSongPositionMs)
    {
        tft.fillSmoothRoundRect(
            tft.width() / 2 - rectWidth / 2 + 2,
            140 + 2,
            rectWidth - 4,
            rectHeight - 4,
            10,
            TFT_BLACK);
        lastSongPositionMs = currentSongPositionMs;
    }
    tft.fillSmoothRoundRect(
        tft.width() / 2 - rectWidth / 2 + 2,
        140 + 2,
        rectWidth * (currentSongPositionMs / currentSong.durationMs) - 4,
        rectHeight - 4,
        10,
        TFT_GREEN);
    // tft.println(currentSongPositionMs);
    // tft.println(currentSong.durationMs);
    // tft.println(currentSongPositionMs/currentSong.durationMs);
    return true;
}

bool SpotifyHandler::togglePlay()
{
    String url = "https://api.spotify.com/v1/me/player/" + String(isPlaying ? "pause" : "play");
    isPlaying = !isPlaying;
    https.begin(url);
    String auth = "Bearer " + String(accessToken);
    https.addHeader("Authorization", auth);
    int httpResponseCode = https.PUT("");
    bool success = false;
    // Check if the request was successful
    if (httpResponseCode == 204)
    {
        // String response = https.getString();
        tft.println((isPlaying ? "Playing" : "Pausing"));
        success = true;
    }
    else
    {
        tft.print("Error pausing or playing: ");
        tft.println(httpResponseCode);
        String response = https.getString();
        tft.println(response);
    }

    // Disconnect from the Spotify API
    https.end();
    getTrackInfo();
    return success;
}

bool SpotifyHandler::adjustVolume(int vol)
{
    String url = "https://api.spotify.com/v1/me/player/volume?volume_percent=" + String(vol);
    https.begin(url);
    String auth = "Bearer " + String(accessToken);
    https.addHeader("Authorization", auth);
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
        tft.print("Error setting volume: ");
        tft.println(httpResponseCode);
        String response = https.getString();
        tft.println(response);
    }
    else
    {
        tft.print("Error setting volume: ");
        tft.println(httpResponseCode);
        String response = https.getString();
        tft.println(response);
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
    int httpResponseCode = https.POST("");
    bool success = false;
    // Check if the request was successful
    if (httpResponseCode == 204)
    {
        // String response = https.getString();
        tft.println("skipping forward");
        success = true;
    }
    else
    {
        tft.print("Error skipping forward: ");
        tft.println(httpResponseCode);
        String response = https.getString();
        tft.println(response);
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
    int httpResponseCode = https.POST("");
    bool success = false;
    // Check if the request was successful
    if (httpResponseCode == 204)
    {
        // String response = https.getString();
        tft.println("skipping backward");
        success = true;
    }
    else
    {
        tft.print("Error skipping backward: ");
        tft.println(httpResponseCode);
        String response = https.getString();
        tft.println(response);
    }

    // Disconnect from the Spotify API
    https.end();
    getTrackInfo();
    return success;
}

bool SpotifyHandler::getFile(String url, String filename) {

  // If it exists then no need to fetch it
  if (SPIFFS.exists(filename) == true) {
    tft.println("Found " + filename);
    return 0;
  }

  tft.println("Downloading "  + filename + " from " + url);

  // Check WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {

    tft.print("[HTTP] begin...\n");

    HTTPClient http;
    http.begin(url);

    tft.print("[HTTP] GET...\n");
    // Start connection and send HTTP header
    int httpCode = http.GET();
    if (httpCode > 0) {
      fs::File f = SPIFFS.open(filename, "w+");
      if (!f) {
        tft.println("file open failed");
        return 0;
      }
      // HTTP header has been send and Server response header has been handled
      tft.printf("[HTTP] GET... code: %d\n", httpCode);

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
        tft.println();
        tft.print("[HTTP] connection closed or file end.\n");
      }
      f.close();
    }
    else {
      tft.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
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
    // tft.print("Size: ");
    // tft.println(size);
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
          
        // tft.print("get: ");
        // tft.println(get);
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
//   tft.println(*(ret_str.end()));
//   tft.println(*(ret_str.end()-1));
//   tft.println(*(ret_str.end()-2));
  if(*(ret_str.end()-1) == ','){
    ret_str = ret_str.substring(0,ret_str.length()-1);
  }
  return ret_str;
}
