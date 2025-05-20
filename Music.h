#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <SDL_mixer.h>
#include <string>

class AudioManager {
private:
    Mix_Music* backgroundMusic;
    int volume;
    bool musicOn;

public:
    AudioManager();
    ~AudioManager();

    bool loadMusic(const std::string& filePath);
    void playMusic();
    void pauseMusic();
    void resumeMusic();
    void toggleMusic();
    void setVolume(int vol);
    void increaseVolume();
    void decreaseVolume();


    bool isMusicOn() const { return musicOn; }
    int getVolume() const { return volume; }
};

#endif
