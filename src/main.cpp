#include <SFML/Graphics.hpp>
#include "emu/chip8.hpp"

int main(int argc, char** argv)
{
    chip8 cpu;
    sf::RenderWindow window(sf::VideoMode(800, 600), "Skylark");
    sf::Texture texture;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear({0,0,0});
        window.display();
    }

    return 0;
}