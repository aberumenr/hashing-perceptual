#include "interface.h"

sf::RenderWindow* createTab() {
    return new sf::RenderWindow(sf::VideoMode(800, 600), "ventana principal", sf::Style::Close | sf::Style::Resize);
}

void runInterface() {
    sf::RenderWindow* window = createTab();

    while (window->isOpen()) {
        sf::Event event;
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window->close();  //se cierra la ventana cuando el usuario la cierra de manera explicita
            }
        }

        window->clear(sf::Color::Black); 
        window->display(); 
    }

    //por lo del apuntador, para prevenir fugas de memoria
    delete window;
}
