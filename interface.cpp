#include "interface.h"
#include "tinyfiledialogs.h"

namespace fs = std::filesystem;

bool saveImageToFolder(const cv::Mat& image, const std::string& fileName) {
    std::string folderPath = "Mis Imagenes";

    //si no se ha creado el folder de imagenes, se crea
    if (!fs::exists(folderPath)) {
        fs::create_directory(folderPath);
    }

    std::string filePath = folderPath + "/" + fileName;
    return cv::imwrite(filePath, image);
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
    if (!menuTexture.loadFromFile("C:\\Users\\prisc\\OneDrive\\Documentos\\Visual Studio 2022\\main menu.png")) { //aqui va el path a la imagen
        std::cerr << "Error: No se pudo cargar la imagen de fondo." << std::endl;
    }
    sf::Sprite menuSprite(menuTexture);

    //font para los botones
    sf::Font font;
    if (!font.loadFromFile("C:\\Users\\prisc\\OneDrive\\Documentos\\Visual Studio 2022\\nexa\\Nexa-Heavy.ttf")) { //aqui ponen el path del archivo ttf
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

                        auto uploadButton = agregarBotonRect({ 861.7, 490.6 }, { 226.8, 46.5 }, " ", font);

                        //imagen de fondo 
                        sf::Texture subirTexture;
                        if (!subirTexture.loadFromFile("C:\\Users\\prisc\\OneDrive\\Documentos\\Visual Studio 2022\\subir.png")) { //aqui va el path a la imagen
                            std::cerr << "Error: No se pudo cargar la imagen de fondo." << std::endl;
                        }
                        sf::Sprite subirSprite(subirTexture);

                        //aqui se guardan la textura y sprite de la imagen subida
                        sf::Texture uploadedTexture;
                        sf::Sprite uploadedSprite;
                        bool imageUploaded = false;

                        //mensajito
                        sf::Text successMessage;
                        successMessage.setFont(font);
                        successMessage.setString("Tu imagen se subio exitosamente! \n Sube otra o regresa a buscar");
                        successMessage.setCharacterSize(20);
                        successMessage.setFillColor(sf::Color::Black);
                        successMessage.setPosition(278.3, 331.9);

                        while (subirImagen->isOpen()) {
                            sf::Event subirAbierto;

                            while (subirImagen->pollEvent(subirAbierto)) {
                                if (subirAbierto.type == sf::Event::Closed) {
                                    subirImagen->close();
                                }
                                if (subirAbierto.type == sf::Event::MouseButtonPressed &&
                                    subirAbierto.mouseButton.button == sf::Mouse::Left) {
                                    sf::Vector2f mouseClickPos(subirAbierto.mouseButton.x, subirAbierto.mouseButton.y);

                                    //boton de subir archivo
                                    if (uploadButton.first.getGlobalBounds().contains(mouseClickPos)) {
                                        const char* fileFilter[1] = { "*.png;*.jpg;*.jpeg;*.bmp" };
                                        const char* filePath = tinyfd_openFileDialog("Select Image File", "", 1, fileFilter, "Image Files", 0);

                                        if (filePath) {
                                            std::cout << "Archivo seleccionado: " << filePath << std::endl;
                                            cv::Mat uploadedImage = cv::imread(filePath);
                                            if (uploadedImage.empty()) {
                                                std::cerr << "Error al cargar la imagen seleccionada." << std::endl;
                                            }
                                            else {
                                                //se guarda el nombre original
                                                std::filesystem::path pathObj(filePath);
                                                std::string fileName = pathObj.filename().string();
                                                //se guarda la foto en folder
                                                if (saveImageToFolder(uploadedImage, fileName)) {
                                                    imageUploaded = true;
                                                }
                                                else {
                                                    std::cerr << "Error al guardar la imagen." << std::endl;
                                                }
                                            }
                                        }
                                        else {
                                            std::cout << "No se selecciono ningun archivo." << std::endl;
                                        }
                                    }
                                }
                            }
                            subirImagen->clear(sf::Color::White);
                            subirImagen->draw(subirSprite);
                            subirImagen->draw(uploadButton.first);

                            if (imageUploaded) {
                                subirImagen->draw(successMessage);
                            }

                            subirImagen->display();
                        }

                    }

                    else if (buscarButton.first.getGlobalBounds().contains(mousePos)) {
                        std::cout << "Clic a boton Buscar." << std::endl;
                        // aqui se le agrega una ventana para buscar la imagen que quiere el usuario
                        sf::RenderWindow* buscarImagen = createTab();

                        //imagen de fondo 
                        sf::Texture buscarTexture;
                        if (!buscarTexture.loadFromFile("C:\\Users\\prisc\\OneDrive\\Documentos\\Visual Studio 2022\\subir.png")) { //aqui va el path a la imagen
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
                        std::cout << "Clic a boton Ayuda." << std::endl;
                        // aqui se le agrega una ventana para buscar la imagen que quiere el usuario
                        sf::RenderWindow* ayudaImagen = createTab();

                        //imagen de fondo 
                        sf::Texture ayudaTexture;
                        if (!ayudaTexture.loadFromFile("C:\\Users\\prisc\\OneDrive\\Documentos\\Visual Studio 2022\\boton ayuda.png")) { //aqui va el path a la imagen
                            std::cerr << "Error: No se pudo cargar la imagen de fondo." << std::endl;
                        }
                        ayudaTexture.setSmooth(true);
                        sf::Sprite ayudaSprite(ayudaTexture);

                        auto resizeAyudaSprite = [&]() {
                        sf::Vector2u ventanaSize = ayudaImagen->getSize();
                        float scaleX = static_cast<float>(ventanaSize.x) / ayudaTexture.getSize().x;
                        float scaleY = static_cast<float>(ventanaSize.y) / ayudaTexture.getSize().y;
                        float scale = std::min(scaleX, scaleY);
                        ayudaSprite.setScale(scale, scale);

                        ayudaSprite.setPosition(
                            (ventanaSize.x - ayudaSprite.getGlobalBounds().width) / 2,
                            (ventanaSize.y - ayudaSprite.getGlobalBounds().height) / 2
                        );
                            };

                        resizeAyudaSprite();

                        while (ayudaImagen->isOpen()) {
                            sf::Event ayudaAbierto;
                            while (ayudaImagen->pollEvent(ayudaAbierto)) {
                                if (ayudaAbierto.type == sf::Event::Closed) {
                                    ayudaImagen->close();
                                }
                                if (ayudaAbierto.type == sf::Event::Closed)
                                {
                                    resizeAyudaSprite();
                                }
                            }
                            //display
                            ayudaImagen->clear(sf::Color::White);

                            //se dibuja el fondo
                            ayudaImagen->draw(ayudaSprite);

                            ayudaImagen->display();
                        }
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

        window->display();
    }

    delete window;
}
