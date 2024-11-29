#include "interface.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <fstream>
#include <algorithm> 

// Para el display de la imagen
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>



using namespace cv;
using namespace std;

namespace fs = std::filesystem;

const int TABLE_SIZE = 10007;  // num primo grande para la tabla hash

// funcion para generar un hash
unsigned long long improvedImageHash(const string& imagePath) {
    // cargar la imagen usando opencv
    Mat image = imread(imagePath, IMREAD_COLOR);

    // verificar si la imagen se cargo
    if (image.empty()) {
        cout << "Error: No se pudo cargar la imagen." << endl;
        return 0;
    }

    unsigned long long hash = 0;  // usamos un hash de 64 bits para mayor capacidad
    const unsigned long long prime = 31; // constante prima para mejorar la dispersion

    // recorrer cada pixel y realizar una operacion sobre los valores RGB
    for (int row = 0; row < image.rows; ++row) {
        for (int col = 0; col < image.cols; ++col) {
            // obtener el valor del pixel (BGR en OpenCV)
            Vec3b pixel = image.at<Vec3b>(row, col);

            // realizar operaciones sobre los canales B, G, R para mezclar los valores
            unsigned long long pixelValue = (pixel[0] * prime) ^ (pixel[1] << 8) ^ (pixel[2] << 16);

            // sumar el valor del pixel al hash usando operaciones de desplazamiento y XOR
            hash = (hash * prime) + (pixelValue ^ (pixelValue >> 3) ^ (pixelValue << 7));

            // operacion adicional para mejorar la dispersion
            hash = hash ^ (hash >> 13);
        }
    }

    return hash;
}

int hammingDistance(unsigned long long hash1, unsigned long long hash2) {
    unsigned long long res = hash1 ^ hash2;
    int distancia = 0;

    // contar los bits diferentes
    while (res != 0) {
        distancia += res & 1; // incrementar si el bit menos significativo es 1
        res >>= 1; // desplazar a la derecha
    }

    return distancia;
}


// iniciar la base de datos 
vector<ImageEntry> DatabaseImage() {
    vector<ImageEntry> database;
    for (int i = 1; i < 16; ++i) { // cambia el tamaño de la base aqui
        string path = "C:\\Users\\alexa\\source\\repos\\opencv\\opencv\\Imagenes\\IMG" + to_string(i) + ".jpg";
        unsigned long long hash = improvedImageHash(path); // crea el hash de las imagenes de la base de datos
        if (hash != 0) {
            database.push_back({ path, hash });
        }
    }
    return database;
}

std::vector<ImageEntry> imageDatabase;

int main() {
    // vector para la base de datos 
    imageDatabase = DatabaseImage();

    // hash de imagen cargada por el usuario
    string userImagePath = "C:\\Users\\alexa\\OneDrive\\Pictures\\Roblox\\RobloxScreenShot20231218_204242323.png";
    unsigned long long userImageHash = improvedImageHash(userImagePath);

    if (userImageHash != 0) {
        cout << "Hash de la imagen cargada: " << hex << userImageHash << endl;

        // vector para almacenar imagenes similares y sus distancias
        vector<pair<int, string>> similarImages;

        for (const auto& entry : imageDatabase) {
            int distancia = hammingDistance(userImageHash, entry.hash);
            if (distancia < 20) {  // rango de similitud
                similarImages.push_back(make_pair(distancia, entry.path));
            }
        }

        // ordenar imagenes similares por distancia de Hamming
        sort(similarImages.begin(), similarImages.end());

        // variable para contar la cant de imagenes similares max 3
        int numSimilarImages = min(3, static_cast<int>(similarImages.size()));

        if (numSimilarImages > 0) {
            cout << "Se encontraron " << numSimilarImages << " imágenes similares:" << endl;
            for (int i = 0; i < numSimilarImages; ++i) {
                cout << "Imagen similar: " << similarImages[i].second
                    << " Distancia de Hamming: " << similarImages[i].first << endl;
            }
        }
        else {
            cout << "No se encontraron imágenes similares en la base de datos." << endl;
        }
    }
    else {
        cout << "No se pudo calcular el hash de la imagen cargada." << endl;
    }

    runInterface(userImagePath, userImagePath);
    return 0;
}



