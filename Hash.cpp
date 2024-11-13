#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <list>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <fstream>

// Para el display de la imagen
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include "interface.h"

using namespace cv;
using namespace std;

namespace fs = std::filesystem;

const int TABLE_SIZE = 10007;  // num primo grande para la tabla hash


struct ImageEntry {
    string path;
    unsigned long long hash;
};


// Funcion para generar un hash
unsigned long long improvedImageHash(const string& imagePath) {
    // Cargar la imagen usando opencv
    Mat image = imread(imagePath, IMREAD_COLOR);

    // Verificar si la imagen se cargó
    if (image.empty()) {
        cout << "Error: No se pudo cargar la imagen." << endl;
        return 0;
    }

    unsigned long long hash = 0;  // Usamos un hash de 64 bits para mayor capacidad
    const unsigned long long prime = 31; // Constante prima para mejorar la dispersión

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

// se crea la tabla hash para prevenir las colisiones de hashing por chaining
class HashTable {
    vector<list<unsigned long long>> table;

public:
    HashTable(int size) : table(size) {}

    // insertar un hash en la tabla
    void insert(unsigned long long hashValue) {
        int index = hashValue % TABLE_SIZE;
        table[index].push_back(hashValue);  // añadir a la lista en esa posicion
    }

    // buscar un hash en la tabla
    bool search(unsigned long long hashValue) {
        int index = hashValue % TABLE_SIZE;
        for (auto& h : table[index]) {
            if (h == hashValue) {
                return true;  // hash encontrado
            }
        }
        return false;  // no encontrado
    }
};

// iniciar la base de datos 
vector<ImageEntry> DatabaseImage() {
    vector<ImageEntry> database;
    for (int i = 1; i <= 13; ++i) { // aqui se cambia el tamaño de la base, esta en 13 pq solo descargue 13 imagenes
        string path = "C:\\Users\\mendi\\Documents\\PROYECTO ANALISIS DE ALGORITMOS\\Proyecto Analisis\\Images\\IMG" + to_string(i) + ".jpg";
        unsigned long long hash = improvedImageHash(path); //crea el hash de las imagenes de la base de datos
        if (hash != 0) {
            database.push_back({ path, hash });
        }
    }
    return database;
}

int main() {
    // vector para la base de datos 
    vector<ImageEntry> imageDatabase = DatabaseImage();

    // crear la tabla hash y agregar hashes de las imagenes de la base de datos
    HashTable hashTable(TABLE_SIZE);
    for (const auto& entry : imageDatabase) {
        hashTable.insert(entry.hash);
    }

    // hash de imagen cargada por el usuario
    string userImagePath = "C:\\Users\\mendi\\Documents\\PROYECTO ANALISIS DE ALGORITMOS\\Proyecto Analisis\\Images\\IMG12.jpg";
    unsigned long long userImageHash = improvedImageHash(userImagePath);

    if (userImageHash != 0) {
        cout << "Hash de la imagen cargada: " << hex << userImageHash << endl;

        // revisar si son similares
        bool foundSimilar = false;
        for (const auto& entry : imageDatabase) {
            int distancia = hammingDistance(userImageHash, entry.hash);
            if (distancia < 10) {  // rango de similitud
                cout << "Imagen similar encontrada: " << entry.path /*Aqui va la funcion que imprime la o las imagenes similares en vez del path*/ << " (Distancia de Hamming: " << distancia << ")" << endl;
                foundSimilar = true;
            }
        }

        if (!foundSimilar) 
        {
            cout << "No se encontraron imágenes similares en la base de datos." << endl;
        }
    }
    else 
    {
        cout << "No se pudo calcular el hash de la imagen cargada." << endl;
    }

    runInterface(userImagePath, userImagePath); 
    return 0;
}