#include "Music.h"

AudioManager::AudioManager() : backgroundMusic(nullptr), volume(MIX_MAX_VOLUME), musicOn(true) {
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
}

AudioManager::~AudioManager() {
    if (backgroundMusic) {
        Mix_FreeMusic(backgroundMusic);
    }
    Mix_CloseAudio();
}

bool AudioManager::loadMusic(const std::string& filePath) {
    backgroundMusic = Mix_LoadMUS(filePath.c_str());
    if (!backgroundMusic) return false;
    Mix_VolumeMusic(volume);
    return true;
}

void AudioManager::playMusic() {
    if (backgroundMusic && musicOn) {
        Mix_PlayMusic(backgroundMusic, -1);
    }
}

void AudioManager::pauseMusic() {
    Mix_PauseMusic();
    musicOn = false;
}

void AudioManager::resumeMusic() {
    Mix_ResumeMusic();
    musicOn = true;
}

void AudioManager::toggleMusic() {
    if (musicOn) pauseMusic();
    else resumeMusic();
}

void AudioManager::setVolume(int vol) {
    volume = vol < 0 ? 0 : (vol > MIX_MAX_VOLUME ? MIX_MAX_VOLUME : vol);
    Mix_VolumeMusic(volume);
}

void AudioManager::increaseVolume() {
    setVolume(volume + 16);
}

void AudioManager::decreaseVolume() {
    setVolume(volume - 16);
}

