#include "SpotifyHandler.h"

SpotifyHandler::SpotifyHandler() {

}

//https://gitlab.com/makeitforless/spotify_controller/-/blob/main/spotify_buddy.ino?ref_type=heads

//TODO
/*
bool SpotifyHandler::getSongPlaying(TFT_eSPI *tft) {
    String url = "https://api.spotify.com/v1/me/player/currently-playing";
    http.useHTTP10(true);
    http.begin(*client,url);
    String auth = "Bearer " + String(accessToken);
    http.addHeader("Authorization",auth);
    int httpResponseCode = http.GET();
    bool success = false;
    String songId = "";
    bool refresh = false;
    // Check if the request was successful
    if (httpResponseCode == 200) {
                    // 

        String currentSongProgress = getValue(http,"progress_ms");
        currentSongPositionMs = currentSongProgress.toFloat();
        String imageLink = "";
        while(imageLink.indexOf("image") == -1){
            String height = getValue(http,"height");
            // Serial.println(height);
            if(height.toInt() > 300){
                imageLink = "";
                continue;
            }
            imageLink = getValue(http, "url");
            
            // Serial.println(imageLink);
        }
        // Serial.println(imageLink);
        
        
        String albumName = getValue(http,"name");
        String artistName = getValue(http,"name");
        String songDuration = getValue(http,"duration_ms");
        currentSong.durationMs = songDuration.toInt();
        String songName = getValue(http,"name");
        songId = getValue(http,"uri");
        String isPlay = getValue(http, "is_playing");
        isPlaying = isPlay == "true";
        Serial.println(isPlay);
        // Serial.println(songId);
        songId = songId.substring(15,songId.length()-1);
        // Serial.println(songId);
        http.end();
        // listSPIFFS();
        if (songId != currentSong.Id){
            
            if(SPIFFS.exists("/albumArt.jpg") == true) {
                SPIFFS.remove("/albumArt.jpg");
            }
            // Serial.println("trying to get album art");
            bool loaded_ok = getFile(imageLink.substring(1,imageLink.length()-1).c_str(), "/albumArt.jpg"); // Note name preceded with "/"
            Serial.println("Image load was: ");
            Serial.println(loaded_ok);
            refresh = true;
            tft->fillScreen(TFT_BLACK);
        }
        currentSong.album = albumName.substring(1,albumName.length()-1);
        currentSong.artist = artistName.substring(1,artistName.length()-1);
        currentSong.song = songName.substring(1,songName.length()-1);
        currentSong.Id = songId;
        currentSong.isLiked = findLikedStatus(songId);
        success = true;
    } else {
        Serial.print("Error getting track info: ");
        Serial.println(httpResponseCode);
        // String response = https.getString();
        // Serial.println(response);
        http.end();
    }
    
    
    // Disconnect from the Spotify API
    if(success){
        drawScreen(refresh);
        lastSongPositionMs = currentSongPositionMs;
    }
    return success;
}
*/