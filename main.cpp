#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <list>
#include <cmath>
#include <SFML/Graphics.hpp>

// Para el display de la imagen
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include "interface.h"

using namespace cv;
using namespace std;

const int TABLE_SIZE = 10007;  // num primo grande para la tabla hash


// Funcion para generar un hash
unsigned long long improvedImageHash(const string& imagePath) {
    // Cargar la imagen usando opencv
    Mat image = imread(imagePath, IMREAD_COLOR);

    // Verificar si la imagen se carg�
    if (image.empty()) {
        cout << "Error: No se pudo cargar la imagen." << endl;
        return 0;
    }

    unsigned long long hash = 0;  // Usamos un hash de 64 bits para mayor capacidad
    const unsigned long long prime = 31; // Constante prima para mejorar la dispersi�n

    // Recorrer cada p�xel y realizar una operaci�n sobre los valores RGB
    for (int row = 0; row < image.rows; ++row) {
        for (int col = 0; col < image.cols; ++col) {
            // Obtener el valor del p�xel (BGR en OpenCV)
            Vec3b pixel = image.at<Vec3b>(row, col);

            // Realizar operaciones sobre los canales B, G, R para mezclar los valores
            unsigned long long pixelValue = (pixel[0] * prime) ^ (pixel[1] << 8) ^ (pixel[2] << 16);

            // Sumar el valor del p�xel al hash usando operaciones de desplazamiento y XOR
            hash = (hash * prime) + (pixelValue ^ (pixelValue >> 3) ^ (pixelValue << 7));

            // Operaci�n adicional para mejorar la dispersi�n
            hash = hash ^ (hash >> 13);
        }
    }

    return hash;
}

int hammingDistance(unsigned long long hash1, unsigned long long hash2) {
    unsigned long long res = hash1 ^ hash2;
    int distancia = 0;

    // Contar los bits diferentes
    while (res != 0) {
        distancia += res & 1; // Incrementar si el bit menos significativo es 1
        res >>= 1; // Desplazar a la derecha
    }

    return distancia;
}


// se crea la tabla hash para prevenir las colisiones de hashing por chaining
class HashTable {
    vector<list<unsigned long long>> table;

public:
    HashTable(int size) : table(size) {}

    // Insertar un hash en la tabla
    void insert(unsigned long long hashValue) {
        int index = hashValue % TABLE_SIZE;
        table[index].push_back(hashValue);  // A�adir a la lista en esa posici�n
    }

    // Buscar un hash en la tabla
    bool search(unsigned long long hashValue) {
        int index = hashValue % TABLE_SIZE;
        for (auto& h : table[index]) {
            if (h == hashValue) {
                return true;  // Hash encontrado
            }
        }
        return false;  // No encontrado
    }
};

int main() {
    string imagePath1 = "C:\\Users\\alexa\\OneDrive\\Pictures\\Roblox\\RobloxScreenShot20231129_222148820.png";
    string imagePath2 = "C:\\Users\\alexa\\OneDrive\\Pictures\\Roblox\\RobloxScreenShot20231129_222148820.png";

    //calcular de manera individual los hashes de las imagenes
    unsigned long long imageHash1 = improvedImageHash(imagePath1);
    unsigned long long imageHash2 = improvedImageHash(imagePath2);

    if (imageHash1 != 0 && imageHash2 != 0) {
        cout << "Hash de la primera imagen: " << hex << imageHash1 << endl;
        cout << "Hash de la segunda imagen: " << hex << imageHash2 << endl;

        //se calcula la distancia de hamming entre los hashes de las dos imagenes
        int distancia = hammingDistance(imageHash1, imageHash2);
        cout << "Distancia de Hamming entre las im�genes: " << distancia << endl;

        if (distancia == 0) {
            cout << "Las imagenes son id�nticas." << endl;
        }
        else {
            cout << "Las imagenes son diferentes." << endl;
        }

        // Crear la tabla hash
        HashTable hashTable(TABLE_SIZE);

        // Insertar el hash de la primera imagen en la tabla
        if (!hashTable.search(imageHash1)) {
            hashTable.insert(imageHash1);
            cout << "Hash de la primera imagen insertado en la tabla." << endl;
        }
        else {
            cout << "Colisi�n detectada: el hash de la primera imagen ya existe en la tabla." << endl;
        }
    }

    runInterface(imagePath1, imagePath2);

    return 0;
}