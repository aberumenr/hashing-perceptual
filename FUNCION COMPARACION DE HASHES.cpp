#include <iostream>
// Para el hash
#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath> 

// Para el display de la imagen
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp> 

using namespace cv;
using namespace std;

// Funcion para generar un hash 
unsigned long long improvedImageHash(const string& imagePath) {
    // Cargar la imagen usando opencv
    Mat image = imread(imagePath, IMREAD_COLOR);

    // Verificar si la imagen funciona 
    if (image.empty()) {
        cout << "Error: No se pudo cargar la imagen." << endl;
        return 0;
    }

    unsigned long long hash = 0;  // Usamos un hash de 64 bits para mayor capacidad
    const unsigned long long prime = 31; // Constante prima para mejorar la dispersion

    // Recorrer cada pixel y realizar una operación mas compleja sobre los valores RGB
    for (int row = 0; row < image.rows; ++row) {
        for (int col = 0; col < image.cols; ++col) {
            // Obtener el valor del pixel (BGR en OpenCV)
            Vec3b pixel = image.at<Vec3b>(row, col);

            // Realizar operaciones sobre los canales B, G, R para mezclar los valores
            unsigned long long pixelValue = (pixel[0] * prime) ^ (pixel[1] << 8) ^ (pixel[2] << 16);

            // Sumar el valor del pixel al hash usando operaciones de desplazamiento y XOR
            hash = (hash * prime) + (pixelValue ^ (pixelValue >> 3) ^ (pixelValue << 7));

            // Operacion adicional para mejorar la dispersión
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

int main() {
    string imagePath1 = "C:\\Users\\prisc\\Downloads\\Snoopy in the Fall Wallpaper 1366x768.png";
    string imagePath2 = "C:\\Users\\prisc\\Downloads\\snoopy.png";

    // Calcular los hashes de las imágenes
    unsigned long long imageHash1 = improvedImageHash(imagePath1);
    unsigned long long imageHash2 = improvedImageHash(imagePath2);

    if (imageHash1 != 0 && imageHash2 != 0) {
        cout << "Hash de la primera imagen: " << hex << imageHash1 << endl;
        cout << "Hash de la segunda imagen: " << hex << imageHash2 << endl;

        // Calcular la distancia de Hamming entre los dos hashes
        int distancia = hammingDistance(imageHash1, imageHash2);
        cout << "Distancia de Hamming entre las imágenes: " << distancia << endl;

        if (distancia == 0) {
            cout << "Las imagenes son idénticas." << endl;
        }
        else {
            cout << "Las imagenes son diferentes." << endl;
        }
    }

    return 0;
}