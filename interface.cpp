#include "interface.h"

sf::Texture loadImage(const cv::Mat& image) {
    sf::Texture texture;

    if (image.empty()) {
        std::cerr << "Error: Imagen en formato incorrecto." << std::endl;
        return texture;
    }

    //se convierte img a sfml formato
    cv::Mat imageRGB;
    if (image.channels() == 3) {
        cv::cvtColor(image, imageRGB, cv::COLOR_BGR2RGB);
    }
    else if (image.channels() == 4) {
        cv::cvtColor(image, imageRGB, cv::COLOR_BGRA2RGBA);
    }
    else {
        std::cerr << "Error: Imagen en formato incorrecto." << image.channels() << std::endl;
        return texture;
    }

    std::vector<sf::Uint8> pixels(imageRGB.rows * imageRGB.cols * 4);

    for (int y = 0; y < imageRGB.rows; ++y) {
        for (int x = 0; x < imageRGB.cols; ++x) {
            cv::Vec3b color = imageRGB.at<cv::Vec3b>(y, x);
            int index = (y * imageRGB.cols + x) * 4;
            pixels[index] = color[0];      // Red
            pixels[index + 1] = color[1];  // Green
            pixels[index + 2] = color[2];  // Blue
            pixels[index + 3] = 255;       // Alpha
        }
    }

    sf::Image img;
    img.create(imageRGB.cols, imageRGB.rows, pixels.data());

    if (!texture.loadFromImage(img)) {
        std::cerr << "Error: Imagen en formato incorrecto." << std::endl;
    }

    return texture;
}

//boton rectangular
std::pair<sf::RectangleShape, sf::Text> agregarBotonRect(
    const sf::Vector2f& position,
    const sf::Vector2f& size,
    const std::string& label,
    sf::Font& font
) {
    sf::RectangleShape button(size);
    button.setPosition(position);
    button.setFillColor(sf::Color(106, 142, 250, 0));

    sf::Text buttonText;
    buttonText.setFont(font);
    buttonText.setString(label);
    buttonText.setCharacterSize(20);
    buttonText.setFillColor(sf::Color::White);
    buttonText.setPosition(
        position.x + (size.x - buttonText.getLocalBounds().width) / 2,
        position.y + (size.y - buttonText.getLocalBounds().height) / 3
    );

    return { button, buttonText };
}

//boton circular
std::pair<sf::CircleShape, sf::Text> agregarBotonCirc(
    const sf::Vector2f& position,
    float radius,
    const std::string& label,
    sf::Font& font
) {
    sf::CircleShape button(radius);
    button.setPosition(position);
    button.setFillColor(sf::Color(106, 142, 250, 0));  //color del boton

    sf::Text buttonText;
    buttonText.setFont(font);
    buttonText.setString(label);
    buttonText.setCharacterSize(20);
    buttonText.setFillColor(sf::Color::White);

    sf::FloatRect textBounds = buttonText.getLocalBounds();
    buttonText.setOrigin(textBounds.width / 1.7, textBounds.height / 1.2);
    buttonText.setPosition(
        position.x + radius,
        position.y + radius
    );

    return { button, buttonText };
}

sf::RenderWindow* createTab() {
    //tamaño fijo ventana
    return new sf::RenderWindow(sf::VideoMode(1366, 768), "Ventana principal", sf::Style::Titlebar | sf::Style::Close);
}

void runInterface(const std::string& imagePath1, const std::string& imagePath2) {
    sf::RenderWindow* window = createTab();
 
    //imagen de fondo 
    sf::Texture menuTexture;
    if (!menuTexture.loadFromFile("C:\\Users\\alexa\\OneDrive\\Documents\\hashing perceptual de imagenes\\main menu.png")) { //aqui va el path a la imagen
        std::cerr << "Error: No se pudo cargar la imagen de fondo." << std::endl;
    }
    sf::Sprite menuSprite(menuTexture);

    //font para los botones
    sf::Font font;
    if (!font.loadFromFile("C:\\Users\\alexa\\OneDrive\\Documents\\hashing perceptual de imagenes\\font\\Nexa-Heavy.ttf")) { //aqui ponen el path del archivo ttf
        std::cerr << "Error cargando fuente" << std::endl;
    }

    //creacion de botones
    auto subirImagenButton = agregarBotonRect({ 244.8, 293.9 }, { 270.8, 61.5 }, " ", font);
    auto buscarButton = agregarBotonRect({ 244.8, 365.8 }, { 270.8, 61.5 }, " ", font);
    auto guardadosButton = agregarBotonRect({ 244.8, 437.7 }, { 270.8, 61.5 }, " ", font);
    auto ayudaButton = agregarBotonCirc({ 1220, 90 }, { 35 }, " ", font);

    cv::Mat image1 = cv::imread(imagePath1);
    sf::Texture texture1 = loadImage(image1);
    sf::Sprite sprite1(texture1);
    cv::Mat image2 = cv::imread(imagePath2);
    sf::Texture texture2 = loadImage(image2);
    sf::Sprite sprite2(texture2);

    sf::Text mensajeAyuda;
    mensajeAyuda.setFont(font);
    //este es el mensaje de ayuda que podran leer los usuarios
    mensajeAyuda.setString("El sistema te permite buscar im�genes mediante comparaci�n\n"
        "con las im�genes del programa o puedes cargar las tuyas.\n"
        "Usa los botones para realizar acciones como Subir Imagen,\n"
        "Buscar o acceder a las im�genes Guardadas.");
    mensajeAyuda.setCharacterSize(14);
    mensajeAyuda.setFillColor(sf::Color::Black);
    mensajeAyuda.setPosition(0, 25); // ajusta la posici�n del mensaje

    //si es verdadero se mostrara el mensaje
    bool mostrarMensaje = false;

    auto resizePics = [&](sf::Vector2u windowSize) {
        float maxWidth = static_cast<float>(windowSize.x) / 2;
        float maxHeight = static_cast<float>(windowSize.y);

        float scaleFactor1 = std::min(maxWidth / texture1.getSize().x, maxHeight / texture1.getSize().y);
        float scaleFactor2 = std::min(maxWidth / texture2.getSize().x, maxHeight / texture2.getSize().y);

        sprite1.setScale(scaleFactor1, scaleFactor1);
        sprite2.setScale(scaleFactor2, scaleFactor2);

        sprite1.setPosition((maxWidth - sprite1.getGlobalBounds().width) / 2, (maxHeight - sprite1.getGlobalBounds().height) / 2);
        sprite2.setPosition(maxWidth + (maxWidth - sprite2.getGlobalBounds().width) / 2, (maxHeight - sprite2.getGlobalBounds().height) / 2);
        };

    resizePics(window->getSize());

    while (window->isOpen()) {
        sf::Event menuAbierto;
        while (window->pollEvent(menuAbierto)) {
            if (menuAbierto.type == sf::Event::Closed) {
                window->close();
            }

            if (menuAbierto.type == sf::Event::Resized) {
                sf::Vector2u newSize = window->getSize();
            }


            //se inicia la funcion para que se detecte que los botones funcionan
            //el mensaje de cout se va a quitar por ahorita es para comprobar que si funciona el clic
            if (menuAbierto.type == sf::Event::MouseButtonPressed) {
                if (menuAbierto.mouseButton.button == sf::Mouse::Left) {
                    // busca las coordenadas del mouse en la ventana
                    sf::Vector2f mousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));

                    if (subirImagenButton.first.getGlobalBounds().contains(mousePos)) {
                        // aqui se le agrega una ventana para subir la imagen que quiere el usuario
                        sf::RenderWindow* subirImagen = createTab();

                        //imagen de fondo 
                        sf::Texture subirTexture;
                        if (!subirTexture.loadFromFile("C:\\Users\\alexa\\OneDrive\\Documents\\hashing perceptual de imagenes\\subir.png")) { //aqui va el path a la imagen
                            std::cerr << "Error: No se pudo cargar la imagen de fondo." << std::endl;
                        }
                        sf::Sprite subirSprite(subirTexture);

                        while (subirImagen->isOpen()) {
                            sf::Event subirAbierto;
                            while (subirImagen->pollEvent(subirAbierto)) {
                                if (subirAbierto.type == sf::Event::Closed) {
                                    subirImagen->close();
                                }
                            }
                            //display
                            subirImagen->clear(sf::Color::White);

                            //se dibuja el fondo
                            subirImagen->draw(subirSprite);

                            subirImagen->display();
                        } 
                    }

                    else if (buscarButton.first.getGlobalBounds().contains(mousePos)) {
                        std::cout << "Clic a boton Buscar." << std::endl;
                        // aqui se le agrega una ventana para buscar la imagen que quiere el usuario
                        sf::RenderWindow* buscarImagen = createTab();

                        //imagen de fondo 
                        sf::Texture buscarTexture;
                        if (!buscarTexture.loadFromFile("C:\\Users\\alexa\\OneDrive\\Documents\\hashing perceptual de imagenes\\subir.png")) { //aqui va el path a la imagen
                            std::cerr << "Error: No se pudo cargar la imagen de fondo." << std::endl;
                        }
                        sf::Sprite buscarSprite(buscarTexture);

                        while (buscarImagen->isOpen()) {
                            sf::Event buscarAbierto;
                            while (buscarImagen->pollEvent(buscarAbierto)) {
                                if (buscarAbierto.type == sf::Event::Closed) {
                                    buscarImagen->close();
                                }
                            }
                            //display
                            buscarImagen->clear(sf::Color::White);

                            //se dibuja el fondo
                            buscarImagen->draw(buscarSprite);

                            buscarImagen->display();
                        }
                        /*
                        //se dibujan las imagenes
                        if (sprite1.getTexture()) {
                            window->draw(sprite1);
                        }
                        if (sprite2.getTexture()) {
                            window->draw(sprite2);
                        }*/
                    }

                    else if (guardadosButton.first.getGlobalBounds().contains(mousePos)) {
                        std::cout << "Clic a boton Guardados." << std::endl;
                        // aqui se le agrega una ventana donde se muestren las imagenes que ya ha guardado el usuario anteriormente
                    }

                    else if (ayudaButton.first.getGlobalBounds().contains(mousePos)) {
                        std::cout << "Clic a boton ayuda." << std::endl;
                        mostrarMensaje = !mostrarMensaje; // se hace la accion de mostrar/ocultar el mensaje de ayuda
                    }
                    else {
                        mostrarMensaje = false;
                    }
                }
            }
        }

        window->clear(sf::Color::White);

        //se dibuja el fondo
        window->draw(menuSprite);


        //se dibujan los botones
        window->draw(subirImagenButton.first); //la figura
        window->draw(subirImagenButton.second); //texto
        window->draw(buscarButton.first);
        window->draw(buscarButton.second);
        window->draw(guardadosButton.first);
        window->draw(guardadosButton.second);
        window->draw(ayudaButton.first);
        window->draw(ayudaButton.second);

        //se crea una ventana para poder mostrar el mensaje de ayuda para el usuario
        if (mostrarMensaje) {
            window->draw(mensajeAyuda);
        }

        window->display();
    }

    delete window;
}
