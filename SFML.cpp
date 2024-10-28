#include "interface.h"

sf::RenderWindow* createTab() {
    return new sf::RenderWindow(sf::VideoMode(800, 600), "Ventana principal", sf::Style::Close | sf::Style::Resize);
}

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
    button.setFillColor(sf::Color(106, 142, 250));

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
    button.setFillColor(sf::Color(106, 142, 250));  //color del boton

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


void runInterface(const std::string& imagePath1, const std::string& imagePath2) {
    sf::RenderWindow* window = createTab();

    //font para los botones
    sf::Font font;
    if (!font.loadFromFile("C:\\Users\\prisc\\Downloads\\nexa\\Nexa-Heavy.ttf")) { //aqui ponen el path del archivo ttf
        std::cerr << "Error cargando fuente" << std::endl;
    }

    //creacion de botones
    auto subirImagenButton = agregarBotonRect({ 300, 350 }, { 200, 50 }, "Subir Imagen", font);
    auto buscarButton = agregarBotonRect({ 300, 420 }, { 200, 50 }, "Buscar", font);
    auto guardadosButton = agregarBotonRect({ 300, 490 }, { 200, 50 }, "Guardados", font);
    auto ayudaButton = agregarBotonCirc({ 725, 30 }, { 20 }, "?", font);

    cv::Mat image1 = cv::imread(imagePath1);
    sf::Texture texture1 = loadImage(image1);
    sf::Sprite sprite1(texture1);
    cv::Mat image2 = cv::imread(imagePath2);
    sf::Texture texture2 = loadImage(image2);
    sf::Sprite sprite2(texture2);

    sf::Text mensajeAyuda;
    mensajeAyuda.setFont(font);
    //este es el mensaje de ayuda que podran leer los usuarios
    mensajeAyuda.setString("El sistema te permite buscar imágenes mediante comparación\n"
        "con las imágenes del programa o puedes cargar las tuyas.\n"
        "Usa los botones para realizar acciones como Subir Imagen,\n"
        "Buscar o acceder a las imágenes Guardadas.");
    mensajeAyuda.setCharacterSize(14);
    mensajeAyuda.setFillColor(sf::Color::Black);
    mensajeAyuda.setPosition(0, 25); // ajusta la posición del mensaje

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
        sf::Event event;
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window->close();
            }

            if (event.type == sf::Event::Resized) {
                resizePics(window->getSize());
            }
            //se inicia la funcion para que se detecte que los botones funcionan
            //el mensaje de cout se va a quitar por ahorita es para comprobar que si funciona el clic
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    // busca las coordenadas del mouse en la ventana
                    sf::Vector2f mousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));

                    if (subirImagenButton.first.getGlobalBounds().contains(mousePos)) {
                        std::cout << "Clic a boton Subir Imagen." << std::endl;
                        // aqui se le agrega una ventana para subir la imagen que quiere el usuario
                    }
                    
                    else if (buscarButton.first.getGlobalBounds().contains(mousePos)) {
                        std::cout << "Clic a boton Buscar." << std::endl;
                        // aqui se le agrega una ventana para buscar la imagen que quiere el usuario
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

        //se dibujan las imagenes
        if (sprite1.getTexture()) {
            window->draw(sprite1);
        }
        if (sprite2.getTexture()) {
            window->draw(sprite2);
        }

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