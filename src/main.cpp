#include <cstring>
#include <SFML/Graphics.hpp>
#include "emu/chip8.hpp"
#include "emu/apu.hpp"

sf::Keyboard::Key keymap[16]
{
    sf::Keyboard::Key::X,
    sf::Keyboard::Key::A,
    sf::Keyboard::Key::S,
    sf::Keyboard::Key::D,
    sf::Keyboard::Key::Q,
    sf::Keyboard::Key::W,
    sf::Keyboard::Key::E,
    sf::Keyboard::Key::Num1,
    sf::Keyboard::Key::Num2,
    sf::Keyboard::Key::Num3,
    sf::Keyboard::Key::Num4,
    sf::Keyboard::Key::R,
    sf::Keyboard::Key::F,
    sf::Keyboard::Key::V,
    sf::Keyboard::Key::C,
    sf::Keyboard::Key::Z
};

chip8* cpu;
apu* audio;
u8 pixels[HEIGHT][WIDTH][4] {0u};

void reset(char* fname)
{
    delete cpu;
    delete audio;

    cpu = new chip8;
    cpu->init();
    cpu->load(fname);

    audio = new apu;
    audio->init();

    std::memset(pixels,0u,HEIGHT*WIDTH*4);
}

int main(int argc, char** argv)
{
    if(argc == 1) return 1;

    cpu = new chip8;
    cpu->init();
    cpu->load(argv[1]);

    audio = new apu;
    audio->init();

    sf::RenderWindow window(sf::VideoMode(WIDTH*10, HEIGHT*10, 8), "Skylark");

    sf::Clock clock;

    sf::Texture texture;
    texture.create(WIDTH, HEIGHT);
    texture.update((u8*)&pixels);

    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setScale(10,10);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if(event.type == sf::Event::KeyPressed)
            {
                for(u32 i = 0; i < 16; i++)
                {
                    if(keymap[i] == event.key.code)
                        cpu->pad[i] = 1u;
                }
                if(event.key.code == sf::Keyboard::Key::F1)
                    reset(argv[1]);
            }
            else if(event.type == sf::Event::KeyReleased)
            {
                for(u32 i = 0; i < 16; i++)
                {
                    if(keymap[i] == event.key.code)
                        cpu->pad[i] = 0u;
                }
            }
        }
        
        if(clock.getElapsedTime().asSeconds() >= 1/IPS)
        {
            cpu->nextop();

            if(cpu->dt > 0u) cpu->dt--;
            if(cpu->st > 0u) 
            {
                cpu->st--;
                audio->play();
            } else audio->stop();

            if(cpu->drawflag)
            {
                window.clear({0,0,0});
                for(int i = 0; i < HEIGHT; i++)
                {
                    for(int j = 0; j < WIDTH; j++)
                    {
                        if(cpu->vram[i][j] != 0)
                        {
                            pixels[i][j][0] = 0xff;
                            pixels[i][j][1] = 0xff;
                            pixels[i][j][2] = 0xff;
                            pixels[i][j][3] = 0xff;
                        }
                        else
                        {
                            pixels[i][j][0] = 0x00;
                            pixels[i][j][1] = 0x00;
                            pixels[i][j][2] = 0x00;
                            pixels[i][j][3] = 0xff;
                        }
                    }
                }
                texture.update((u8*)&pixels);
                window.draw(sprite);
                window.display();
            }
        }
        clock.restart();
    }

    return 0;
}