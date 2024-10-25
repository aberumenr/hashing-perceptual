#include "interface.h"

sf::RenderWindow* createTab() {
    return new sf::RenderWindow(sf::VideoMode(800, 600), "ventana principal", sf::Style::Close | sf::Style::Resize);
}

sf::Texture loadImage(const cv::Mat& image) {
    sf::Texture texture;

    if (image.empty()) {
        std::cerr << "Error: Imagen en formato incorrecto." << std::endl;
        return texture;
    }

    //se convierte la imagen
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

    //formato para sfml
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

    //se genera la imagen sfml con los datos de los pixeles
    sf::Image img;
    img.create(imageRGB.cols, imageRGB.rows, pixels.data());

    if (!texture.loadFromImage(img)) {
        std::cerr << "Error: Imagen en formato incorrecto." << std::endl;
    }

    return texture;
}

void runInterface(const std::string& imagePath1, const std::string& imagePath2) {
    sf::RenderWindow* window = createTab();
    
    //se cargan las imagenes con cv y luego se pasan a sfml
    cv::Mat image1 = cv::imread(imagePath1);
    sf::Texture texture1 = loadImage(image1);
    sf::Sprite sprite1(texture1);

    cv::Mat image2 = cv::imread(imagePath2);
    sf::Texture texture2 = loadImage(image2);
    sf::Sprite sprite2(texture2);

    //escala y posicion de las fotos a comparar
    auto resizePics = [&](sf::Vector2u windowSize) {
        //dimesniones originales
        float originalWidth1 = texture1.getSize().x;
        float originalHeight1 = texture1.getSize().y;
        float originalWidth2 = texture2.getSize().x;
        float originalHeight2 = texture2.getSize().y;

        //a la mitad d ela pantalla cad auna
        float maxWidth = static_cast<float>(windowSize.x) / 2;
        float maxHeight = static_cast<float>(windowSize.y);

        float scaleFactor1 = std::min(maxWidth / originalWidth1, maxHeight / originalHeight1);
        float scaleFactor2 = std::min(maxWidth / originalWidth2, maxHeight / originalHeight2);

        sprite1.setScale(scaleFactor1, scaleFactor1);
        sprite2.setScale(scaleFactor2, scaleFactor2);

        sprite1.setPosition(
            (maxWidth - sprite1.getGlobalBounds().width) / 2,
            (maxHeight - sprite1.getGlobalBounds().height) / 2
        );

        sprite2.setPosition(
            maxWidth + (maxWidth - sprite2.getGlobalBounds().width) / 2,
            (maxHeight - sprite2.getGlobalBounds().height) / 2
        );
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
        }

        window->clear(sf::Color::Black);

        //se dibujan las fotitos
        if (sprite1.getTexture()) {
            window->draw(sprite1);
        }
        if (sprite2.getTexture()) {
            window->draw(sprite2);
        }

        window->display();
    }

    delete window; 
}
