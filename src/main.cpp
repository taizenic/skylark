#include <SFML/Graphics.hpp>
#include "emu/chip8.hpp"

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

int main(int argc, char** argv)
{
    if(argc == 1) return 1;

    chip8* cpu = new chip8;
    cpu->init();
    cpu->load(argv[1]);

    sf::RenderWindow window(sf::VideoMode(WIDTH*10, HEIGHT*10, 8), "Skylark");
    sf::Clock clock;
    //window.setSize(sf::Vector2u(WIDTH*10, HEIGHT*10));

    while (window.isOpen())
    {
        if(clock.getElapsedTime().asSeconds() >= 1/IPS)
        {
            window.clear();

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

            cpu->nextop();

            sf::RectangleShape rect(sf::Vector2f(10,10));
            for(int i = 0; i < HEIGHT; i++)
            {
                for(int j = 0; j < WIDTH; j++)
                {
                    if(cpu->vram[i][j] != 0)
                    {
                        rect.setPosition((float)j*10, (float)i*10);
                        window.draw(rect);
                    }
                }
            }
            window.display();
        }
        clock.restart();
    }

    return 0;
}