#pragma once
#include <SFML/Audio.hpp>
#include "util/types.hpp"
#include "util/constants.hpp"

class apu
{
private:
    s16 samples[SAMPLE_RATE] {0};
    sf::SoundBuffer buffer;
    sf::Sound sound;
    bool playing = false;

public:
    void init();
    void play();
    void stop();
};