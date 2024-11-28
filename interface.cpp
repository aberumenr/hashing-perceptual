#include "interface.h"
#include "tinyfiledialogs.h"
#include <windows.h>
#include <shellapi.h>
#include <commdlg.h>

namespace fs = std::filesystem;

//funcion para seleccionar imagen
std::wstring stringToWString(const std::string& str) {
    return std::wstring(str.begin(), str.end());
}

std::string openFolder(const std::string& initialFolder) {
    wchar_t filePath[MAX_PATH] = L""; 

    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL; 
    ofn.lpstrFilter = L"Image Files\0*.png;*.jpg;*.jpeg;*.bmp\0"; 
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrInitialDir = stringToWString(initialFolder).c_str(); 
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    if (GetOpenFileName(&ofn)) {
        return std::string(filePath, filePath + wcslen(filePath)); 
    }
    return ""; 
}


//para folder Mis Imagenes
bool saveImageToFolder(const cv::Mat& image, const std::string& fileName) {
    std::string folderPath = "Mis Imagenes";

    //si no se ha creado el folder de imagenes, se crea
    if (!fs::exists(folderPath)) {
        fs::create_directory(folderPath);
    }

    std::string filePath = folderPath + "/" + fileName;
    return cv::imwrite(filePath, image);
}

//para folder Guardados
bool likedImagesFolder(const cv::Mat& image, const std::string& fileName) {
    std::string folderPath = "Mis Guardados";

    //si no se ha creado el folder de imagenes, se crea
    if (!fs::exists(folderPath)) {
        fs::create_directory(folderPath);
    }

    std::string filePath = folderPath + "/" + fileName;
    return cv::imwrite(filePath, image);
}

//para abrir el folder de Guardadso
void openSavedImagesFolder() {
    std::string folderPath = "C:\\Users\\alexa\\source\\repos\\opencv\\opencv\\Imagenes\\Mis Guardados";
    ShellExecuteA(nullptr, "open", folderPath.c_str(), nullptr, nullptr, SW_SHOW);
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
            pixels[index] = color[0];      
            pixels[index + 1] = color[1];  
            pixels[index + 2] = color[2];  
            pixels[index + 3] = 255;       
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

                    //BOTON SUBIR
                    if (subirImagenButton.first.getGlobalBounds().contains(mousePos)) {
                        // aqui se le agrega una ventana para subir la imagen que quiere el usuario
                        sf::RenderWindow* subirImagen = createTab();
                        DragAcceptFiles(subirImagen->getSystemHandle(), TRUE);


                        auto uploadButton = agregarBotonRect({ 861.7, 490.6 }, { 226.8, 46.5 }, " ", font);
                        auto dragWindow = agregarBotonRect({ 743.9, 176.6 }, { 462.3, 201.8 }, " ", font);

                        //imagen de fondo 
                        sf::Texture subirTexture;
                        if (!subirTexture.loadFromFile("C:\\Users\\alexa\\OneDrive\\Documents\\hashing perceptual de imagenes\\subir.png")) { //aqui va el path a la imagen
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

                        //para ver si arrastra el mouse
                        bool isDragging = false;
                        std::string droppedFilePath;

                        while (subirImagen->isOpen()) {
                            //ver los mensajes y actividad direcrtamente de Windows
                            MSG msg;
                            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                                //std::cout << "Processing Windows message: " << msg.message << std::endl;
                                if (msg.message == WM_DROPFILES) {
                                    std::cout << "WM_DROPFILES detected" << std::endl;
                                    HDROP hDrop = (HDROP)msg.wParam;
                                    char filePath[MAX_PATH];

                                    if (DragQueryFileA(hDrop, 0, filePath, MAX_PATH)) {
                                        std::cout << "Archivo arrastrado: " << filePath << std::endl;
                                        cv::Mat uploadedImage = cv::imread(filePath);

                                        if (!uploadedImage.empty()) {
                                            std::cout << "Imagen cargada exitosamente." << std::endl;
                                            std::filesystem::path pathObj(filePath);
                                            std::string fileName = pathObj.filename().string();

                                            if (saveImageToFolder(uploadedImage, fileName)) {
                                                std::cout << "Imagen guardada exitosamente en la carpeta." << std::endl;
                                                imageUploaded = true;
                                            }
                                            else {
                                                std::cerr << "Error al guardar la imagen." << std::endl;
                                            }
                                        }
                                        else {
                                            std::cerr << "Error al cargar la imagen arrastrada." << std::endl;
                                        }
                                    }

                                    DragFinish(hDrop);
                                }

                                TranslateMessage(&msg);
                                DispatchMessage(&msg);
                            }

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

                                    //si esta en el area de arrastrar
                                    if (dragWindow.first.getGlobalBounds().contains(mouseClickPos)) {
                                        isDragging = true;
                                    }
                                }

                            }

                            subirImagen->clear(sf::Color::White);
                            subirImagen->draw(subirSprite);
                            subirImagen->draw(uploadButton.first);
                            subirImagen->draw(dragWindow.first);

                            if (imageUploaded) {
                                subirImagen->draw(successMessage);
                            }

                            subirImagen->display();
                        }

                    }

                    //BOTON BUSCAR
                    else if (buscarButton.first.getGlobalBounds().contains(mousePos)) {
                        std::cout << "Clic a boton Buscar." << std::endl;
                        // aqui se le agrega una ventana para buscar la imagen que quiere el usuario
                        sf::RenderWindow* buscarImagen = createTab();

                        //imagen de fondo 
                        sf::Texture buscarTexture;
                        if (!buscarTexture.loadFromFile("C:\\Users\\alexa\\OneDrive\\Documents\\hashing perceptual de imagenes\\buscarImagen.png")) { //aqui va el path a la imagen
                            std::cerr << "Error: No se pudo cargar la imagen de fondo." << std::endl;
                        }
                        sf::Sprite buscarSprite(buscarTexture);

                        auto abrirMisImagenesButton = agregarBotonRect({ 793.1, 409.5 }, { 266.6, 75 }, " ", font); 

                        //file path a mis imagenes
                        std::string folderPath = "C:\\Users\\alexa\\source\\repos\\opencv\\opencv\\Mis Imagenes";

                        //se guarda la que elige el usuario
                        std::string selectedImagePath;

                        while (buscarImagen->isOpen()) {
                            sf::Event buscarAbierto;
                            while (buscarImagen->pollEvent(buscarAbierto)) {
                                if (buscarAbierto.type == sf::Event::Closed) {
                                    buscarImagen->close();
                                }
                                if (buscarAbierto.type == sf::Event::MouseButtonPressed &&
                                    buscarAbierto.mouseButton.button == sf::Mouse::Left) {
                                    sf::Vector2f mouseClickPos(buscarAbierto.mouseButton.x, buscarAbierto.mouseButton.y);

                                    if (abrirMisImagenesButton.first.getGlobalBounds().contains(mouseClickPos)) {
                                        std::cout << "Le diste click al boton de Mis Imagenes" << std::endl;
                                        
                                        selectedImagePath = openFolder(folderPath);
                                        if (!selectedImagePath.empty()) {
                                            std::cout << "Archivo seleccionado: " << selectedImagePath << std::endl;
                                            //calcular hash de la imagen q eligio el usuario
                                            unsigned long long selectedImageHash = improvedImageHash(selectedImagePath);

                                            if (selectedImageHash != 0) {
                                                std::cout << "Hash de la imagen seleccionada: " << std::hex << selectedImageHash << std::endl;

                                                //se compara con la base de datos
                                                std::vector<std::pair<int, std::string>> similarImages;
                                                for (const auto& entry : imageDatabase) {
                                                    int distancia = hammingDistance(selectedImageHash, entry.hash);

                                                    if (distancia < 30) { //que tan similares
                                                        similarImages.emplace_back(distancia, entry.path);
                                                    }
                                                }

                                                std::sort(similarImages.begin(), similarImages.end());
                                                int numSimilarImages = std::min(3, static_cast<int>(similarImages.size()));

                                                if (numSimilarImages > 0) {
                                                    std::cout << "Se encontraron " << numSimilarImages << " imágenes similares:" << std::endl;
                                                    for (int i = 0; i < numSimilarImages; ++i) {
                                                        std::cout << "Imagen similar: " << similarImages[i].second
                                                                  << " Distancia de Hamming: " << similarImages[i].first << std::endl;
                                                    }

                                                    //se imprimen de acuerdo a cuantas se encontraron

                                                    //UN RESULTADO
                                                    if (numSimilarImages == 1) {
                                                        std::cout << "Imagen similar: " << similarImages[0].second
                                                            << " Distancia de Hamming: " << similarImages[0].first << std::endl;

                                                        // aqui se le agrega una ventana ver resultados 
                                                        sf::RenderWindow* UnResultado = createTab();

                                                        //imagen de fondo 
                                                        sf::Texture UnResultadoTexture;
                                                        if (!UnResultadoTexture.loadFromFile("C:\\Users\\alexa\\OneDrive\\Documents\\hashing perceptual de imagenes\\1resultado.png")) { //aqui va el path a la imagen
                                                            std::cerr << "Error: No se pudo cargar la imagen de fondo." << std::endl;
                                                        }
                                                        sf::Sprite UnResultadoSprite(UnResultadoTexture);

                                                        //cargar imagen
                                                        cv::Mat image = cv::imread(similarImages[0].second);
                                                        sf::Texture foto1textura;
                                                        if (!image.empty()) {
                                                            foto1textura = loadImage(image);
                                                        } else {
                                                            std::cerr << "Error al cargar la imagen: " << similarImages[0].second << std::endl;
                                                        }
                                                        sf::Sprite foto1sprite(foto1textura);
                                                        //para que quepa en el cubito
                                                        float desiredWidth = 375; //ancho
                                                        float desiredHeight = 375; //alto

                                                        //dimensiones acrtuales
                                                        sf::Vector2u textureSize = foto1textura.getSize();
                                                        float currentWidth = static_cast<float>(textureSize.x);
                                                        float currentHeight = static_cast<float>(textureSize.y);

                                                        //escalar
                                                        float scaleX = desiredWidth / currentWidth;
                                                        float scaleY = desiredHeight / currentHeight;
                                                        foto1sprite.setScale(scaleX, scaleY);
                                                        foto1sprite.setPosition(495.6, 246.5);

                                                        //boton guardar imagen
                                                        auto saveButton = agregarBotonRect({ 627.9, 638.3 }, { 110.2, 26.7 }, "", font);

                                                        while (UnResultado->isOpen()) {
                                                            sf::Event UnResultadoAbierto;
                                                            while (UnResultado->pollEvent(UnResultadoAbierto)) {
                                                                if (UnResultadoAbierto.type == sf::Event::Closed) {
                                                                    UnResultado->close();
                                                                }
                                                                if (UnResultadoAbierto.type == sf::Event::MouseButtonPressed &&
                                                                    UnResultadoAbierto.mouseButton.button == sf::Mouse::Left) {
                                                                    sf::Vector2f mouseClickPos(UnResultadoAbierto.mouseButton.x, UnResultadoAbierto.mouseButton.y);

                                                                    if (saveButton.first.getGlobalBounds().contains(mouseClickPos)) {
                                                                        //aqui deberia ir lo que se necesita para guardar el path de la imagen
                                                                        std::cout << "Ruta de la imagen guardada: " << similarImages[0].second << std::endl;

                                                                        cv::Mat image = cv::imread(similarImages[0].second);

                                                                        //nombre del archivo desde el filea pth
                                                                        std::filesystem::path sourcePath(similarImages[0].second);
                                                                        std::string fileName = sourcePath.filename().string();

                                                                        if (likedImagesFolder(image, fileName)) {
                                                                            std::cout << "Imagen guardada en la carpeta 'Mis Guardados' con el nombre: " << fileName << std::endl;
                                                                        }
                                                                        else {
                                                                            std::cerr << "Error al guardar la imagen: " << fileName << std::endl;
                                                                        }
                                                                    }

                                                                }
                                                            }
                                                            //se dibuja todo
                                                            UnResultado->clear(sf::Color::White);          
                                                            UnResultado->draw(UnResultadoSprite);  

                                                            if (!image.empty()) {
                                                                UnResultado->draw(foto1sprite);             
                                                            }

                                                            UnResultado->draw(saveButton.first);             
                                                            UnResultado->draw(saveButton.second);    

                                                            UnResultado->display();
                                                        }
                                                        delete UnResultado; //se libera la memorai
                                                    }

                                                    //DOS RESULTADOS 
                                                    else if (numSimilarImages == 2) {
                                                        //ventana
                                                        sf::RenderWindow* DosResultados = createTab();

                                                        //fondo
                                                        sf::Texture DosResultadosTexture;
                                                        if (!DosResultadosTexture.loadFromFile("C:\\Users\\alexa\\OneDrive\\Documents\\hashing perceptual de imagenes\\2resultados.png")) { //aqui va el path a la imagen
                                                            std::cerr << "Error: No se pudo cargar la imagen de fondo." << std::endl;
                                                        }
                                                        sf::Sprite DosResultadosSprite(DosResultadosTexture);

                                                        //sprites de las fotos
                                                        std::vector<sf::Sprite> imageSprites;
                                                        std::vector<std::unique_ptr<sf::Texture>> imageTextures;

                                                        for (int i = 0; i < 2; ++i) {
                                                            //cargar imagenes
                                                            cv::Mat image = cv::imread(similarImages[i].second);
                                                            if (!image.empty()) {
                                                                auto texture = std::make_unique<sf::Texture>(loadImage(image));
                                                                if (texture->getSize().x == 0 || texture->getSize().y == 0) {
                                                                    std::cerr << "Error al convertir la imagen a textura: " << similarImages[i].second << std::endl;
                                                                    continue;
                                                                }

                                                                //escalarlos
                                                                sf::Sprite sprite(*texture);
                                                                float desiredWidth = 375; //ancho
                                                                float desiredHeight = 375; //alto
                                                                sf::Vector2u textureSize = texture->getSize();
                                                                float currentWidth = static_cast<float>(textureSize.x);
                                                                float currentHeight = static_cast<float>(textureSize.y);
                                                                float scaleX = desiredWidth / currentWidth;
                                                                float scaleY = desiredHeight / currentHeight;
                                                                sprite.setScale(scaleX, scaleY);

                                                                //posiciones
                                                                if (i == 0) {
                                                                    sprite.setPosition(254.9, 290.8); //primera foto
                                                                }
                                                                else {
                                                                    sprite.setPosition(736.3, 290.8); //segunda
                                                                }

                                                                imageSprites.push_back(sprite);
                                                                imageTextures.push_back(std::move(texture));
                                                            }
                                                            else {
                                                                std::cerr << "Error al cargar la imagen: " << similarImages[i].second << std::endl;
                                                            }
                                                        }

                                                        //botones
                                                        std::vector<std::pair<sf::RectangleShape, sf::Text>> saveButtons;
                                                        for (int i = 0; i < 2; ++i) {
                                                            //´para la primer foto
                                                            if (i == 0) {
                                                                saveButtons.push_back(agregarBotonRect({ 387.2, 682.6 }, { 110.2, 26.7 }, " ", font));
                                                            }
                                                            else {
                                                                saveButtons.push_back(agregarBotonRect({ 868.6, 682.6 }, { 110.2, 26.7 }, " ", font));
                                                            }
                                                        }

                                                        while (DosResultados->isOpen()) {
                                                            sf::Event DosResultadosAbierto;
                                                            while (DosResultados->pollEvent(DosResultadosAbierto)) {
                                                                if (DosResultadosAbierto.type == sf::Event::Closed) {
                                                                    DosResultados->close();
                                                                }
                                                                if (DosResultadosAbierto.type == sf::Event::MouseButtonPressed &&
                                                                    DosResultadosAbierto.mouseButton.button == sf::Mouse::Left) {
                                                                    sf::Vector2f mouseClickPos(DosResultadosAbierto.mouseButton.x, DosResultadosAbierto.mouseButton.y);

                                                                    for (int i = 0; i < 2; ++i) {
                                                                        if (saveButtons[i].first.getGlobalBounds().contains(mouseClickPos)) {
                                                                            //aqui va q se guardan
                                                                            std::cout << "Ruta de la imagen guardada: " << similarImages[i].second << std::endl;

                                                                            cv::Mat image = cv::imread(similarImages[i].second);

                                                                            //nombre del archivo desde el filea pth
                                                                            std::filesystem::path sourcePath(similarImages[i].second);
                                                                            std::string fileName = sourcePath.filename().string();

                                                                            if (likedImagesFolder(image, fileName)) {
                                                                                std::cout << "Imagen guardada en la carpeta 'Mis Guardados' con el nombre: " << fileName << std::endl;
                                                                            }
                                                                            else {
                                                                                std::cerr << "Error al guardar la imagen: " << fileName << std::endl;
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                            }

                                                            //dibujar todo
                                                            DosResultados->clear(sf::Color::White);           
                                                            DosResultados->draw(DosResultadosSprite); 

                                                            //dibujar gotos
                                                            for (auto& sprite : imageSprites) {
                                                                DosResultados->draw(sprite);                
                                                            }

                                                            for (auto& button : saveButtons) {
                                                                DosResultados->draw(button.first);          
                                                                DosResultados->draw(button.second);        
                                                            }

                                                            DosResultados->display();                    
                                                        }
                                                        delete DosResultados; 
                                                    }

                                                    //TRES RESULTADOS
                                                    else { //para 3 o mas
                                                        sf::RenderWindow* TresResultados = createTab();

                                                        sf::Texture TresResultadosTexture;
                                                        if (!TresResultadosTexture.loadFromFile("C:\\Users\\alexa\\OneDrive\\Documents\\hashing perceptual de imagenes\\3resultados.png")) { // Fondo para 3 resultados
                                                            std::cerr << "Error: No se pudo cargar la imagen de fondo." << std::endl;
                                                        }
                                                        sf::Sprite TresResultadosSprite(TresResultadosTexture);

                                                        std::vector<sf::Sprite> imageSprites;
                                                        std::vector<std::unique_ptr<sf::Texture>> imageTextures;

                                                        for (int i = 0; i < 3; ++i) { // solo mostramos las 3 mas parecidas
                                                            cv::Mat image = cv::imread(similarImages[i].second);
                                                            if (!image.empty()) {
                                                                auto texture = std::make_unique<sf::Texture>(loadImage(image));
                                                                if (texture->getSize().x == 0 || texture->getSize().y == 0) {
                                                                    std::cerr << "Error al convertir la imagen a textura: " << similarImages[i].second << std::endl;
                                                                    continue;
                                                                }

                                                                sf::Sprite sprite(*texture);
                                                                float desiredWidth = 375;
                                                                float desiredHeight = 375; 
                                                                sf::Vector2u textureSize = texture->getSize();
                                                                float currentWidth = static_cast<float>(textureSize.x);
                                                                float currentHeight = static_cast<float>(textureSize.y);
                                                                float scaleX = desiredWidth / currentWidth;
                                                                float scaleY = desiredHeight / currentHeight;
                                                                sprite.setScale(scaleX, scaleY);

                                                                if (i == 0) {
                                                                    sprite.setPosition(76.9, 249.1); //primera foto
                                                                }
                                                                else if (i == 1) {
                                                                    sprite.setPosition(495.5, 249.1); //segunda
                                                                }
                                                                else {
                                                                    sprite.setPosition(914.3, 249.1); //tercera
                                                                }

                                                                imageSprites.push_back(sprite);
                                                                imageTextures.push_back(std::move(texture));
                                                            }
                                                            else {
                                                                std::cerr << "Error al cargar la imagen: " << similarImages[i].second << std::endl;
                                                            }
                                                        }

                                                        //botones
                                                        std::vector<std::pair<sf::RectangleShape, sf::Text>> saveButtons;
                                                        for (int i = 0; i < 3; ++i) {
                                                            if (i == 0) {
                                                                saveButtons.push_back(agregarBotonRect({ 209.2, 650 }, { 110.2, 26.7 }, " ", font));
                                                            }
                                                            else if (i == 1) {
                                                                saveButtons.push_back(agregarBotonRect({ 627.8, 650 }, { 110.2, 26.7 }, " ", font));
                                                            }
                                                            else {
                                                                saveButtons.push_back(agregarBotonRect({ 1046.6, 650 }, { 110.2, 26.7 }, " ", font));
                                                            }
                                                        }

                                                        while (TresResultados->isOpen()) {
                                                            sf::Event TresResultadosAbierto;
                                                            while (TresResultados->pollEvent(TresResultadosAbierto)) {
                                                                if (TresResultadosAbierto.type == sf::Event::Closed) {
                                                                    TresResultados->close();
                                                                }
                                                                if (TresResultadosAbierto.type == sf::Event::MouseButtonPressed &&
                                                                    TresResultadosAbierto.mouseButton.button == sf::Mouse::Left) {
                                                                    sf::Vector2f mouseClickPos(TresResultadosAbierto.mouseButton.x, TresResultadosAbierto.mouseButton.y);

                                                                    for (int i = 0; i < 3; ++i) {
                                                                        if (saveButtons[i].first.getGlobalBounds().contains(mouseClickPos)) {
                                                                            //guaradr
                                                                            std::cout << "Ruta de la imagen guardada: " << similarImages[i].second << std::endl;

                                                                            cv::Mat image = cv::imread(similarImages[i].second);

                                                                            //nombre del archivo desde el filea pth
                                                                            std::filesystem::path sourcePath(similarImages[i].second);
                                                                            std::string fileName = sourcePath.filename().string();

                                                                            if (likedImagesFolder(image, fileName)) {
                                                                                std::cout << "Imagen guardada en la carpeta 'Mis Guardados' con el nombre: " << fileName << std::endl;
                                                                            }
                                                                            else {
                                                                                std::cerr << "Error al guardar la imagen: " << fileName << std::endl;
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                            TresResultados->clear(sf::Color::White);           
                                                            TresResultados->draw(TresResultadosSprite);       

                                                            for (auto& sprite : imageSprites) {
                                                                TresResultados->draw(sprite);                
                                                            }

                                                            for (auto& button : saveButtons) {
                                                                TresResultados->draw(button.first);          
                                                                TresResultados->draw(button.second);        
                                                            }
                                                            TresResultados->display();               
                                                        }
                                                        delete TresResultados; 
                                                    }
                                                }
                                                else {
                                                    std::cout << "No se encontraron imágenes similares." << std::endl;
                                                    if (!buscarTexture.loadFromFile("C:\\Users\\alexa\\OneDrive\\Documents\\hashing perceptual de imagenes\\sinResultados.png")) { //aqui va el path a la imagen
                                                        std::cerr << "Error: No se pudo cargar la imagen de fondo." << std::endl;
                                                    }
                                                    buscarImagen->draw(buscarSprite);
                                                }
                                            }
                                            else {
                                                std::cerr << "Error al calcular el hash de la imagen seleccionada." << std::endl;
                                            }
                                        }
                                        else {
                                            std::cout << "No se selecciono ningun archivo." << std::endl;
                                        }

                                    }
                                    
                                }
                            }
                            //display
                            buscarImagen->clear(sf::Color::White);

                            //se dibuja el fondo
                            buscarImagen->draw(buscarSprite);

                            //boton
                            buscarImagen->draw(abrirMisImagenesButton.first);
                            buscarImagen->draw(abrirMisImagenesButton.second);

                            buscarImagen->display();
                        }
                    }

                    //BOTON GUARDADOS
                    else if (guardadosButton.first.getGlobalBounds().contains(mousePos)) {
                        std::cout << "Clic a boton Guardados." << std::endl;
                        
                        sf::RenderWindow* imagenesGuardadas = createTab();
                        
                        auto verImagenes = agregarBotonRect({ 206.4, 344.8 }, { 243.2, 78.5 }, " ", font);

                        //imagen de fondo 
                        sf::Texture guardadosTexture;
                        if (!guardadosTexture.loadFromFile("C:\\Users\\alexa\\OneDrive\\Documents\\hashing perceptual de imagenes\\guardados.png")) { //aqui va el path a la imagen
                            std::cerr << "Error: No se pudo cargar la imagen de fondo." << std::endl;
                        }
                        sf::Sprite guardadosSprite(guardadosTexture);

                        while (imagenesGuardadas->isOpen()) {
                            
                            sf::Event guardadosAbierto;

                            while (imagenesGuardadas->pollEvent(guardadosAbierto)) {
                                if (guardadosAbierto.type == sf::Event::Closed) {
                                    imagenesGuardadas->close();
                                }

                                if (guardadosAbierto.type == sf::Event::MouseButtonPressed &&
                                    guardadosAbierto.mouseButton.button == sf::Mouse::Left) {
                                    sf::Vector2f mouseClickPos(guardadosAbierto.mouseButton.x, guardadosAbierto.mouseButton.y);

                                    //boton de ver imagenes
                                    if (verImagenes.first.getGlobalBounds().contains(mouseClickPos)) {
                                        openSavedImagesFolder();
                                    }
                                }
                            }

                            imagenesGuardadas->clear(sf::Color::White);
                            imagenesGuardadas->draw(guardadosSprite);

                            imagenesGuardadas->draw(verImagenes.first);
                            imagenesGuardadas->draw(verImagenes.second);

                            imagenesGuardadas->display();
                        }

                    }

                    //BOTON AYUDA
                    else if (ayudaButton.first.getGlobalBounds().contains(mousePos)) {
                        std::cout << "Clic a boton Ayuda." << std::endl;
                        // aqui se le agrega una ventana para buscar la imagen que quiere el usuario
                        sf::RenderWindow* ayudaImagen = createTab();

                        //imagen de fondo 
                        sf::Texture ayudaTexture;
                        if (!ayudaTexture.loadFromFile("C:\\Users\\alexa\\OneDrive\\Documents\\hashing perceptual de imagenes\\boton ayuda.png")) { //aqui va el path a la imagen
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
