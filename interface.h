#ifndef INTERFACE_H
#define INTERFACE_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <utility>

struct ImageEntry {
    std::string path;
    unsigned long long hash;
};

extern std::vector<ImageEntry> imageDatabase;

//las fucniones del hashing y comparacion de hashes
unsigned long long improvedImageHash(const std::string& imagePath);
int hammingDistance(unsigned long long hash1, unsigned long long hash2);

sf::RenderWindow* createTab();
void runInterface(const std::string& imagePath1, const std::string& imagePath2);
sf::Texture loadImage(const cv::Mat& image);

//botones
std::pair<sf::RectangleShape, sf::Text> agregarBotonRect(
    const sf::Vector2f& position,
    const sf::Vector2f& size,
    const std::string& label,
    sf::Font& font
);
std::pair<sf::CircleShape, sf::Text> agregarBotonCirc(
    const sf::Vector2f& position,
    float radius,
    const std::string& label,
    sf::Font& font
);

#endif // INTERFACE_H
