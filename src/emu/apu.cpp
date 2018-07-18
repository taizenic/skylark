#include <cmath>
#include "emu/apu.hpp"

void apu::init()
{
    f64 x = 0;
    for(u32 i = 0; i < SAMPLE_RATE; i++)
    {
        samples[i] = 30000 * std::sin(x*6.28318);
        x += 440.0/SAMPLE_RATE;
    }
    buffer.loadFromSamples(samples, SAMPLE_RATE, 1, SAMPLE_RATE);

    sound.setBuffer(buffer);
    sound.setLoop(true);
}

void apu::play()
{
    if(playing) return;

    sound.play();
    playing = true;
}

void apu::stop()
{
    if(!playing) return;

    sound.stop();
    playing = false;
}