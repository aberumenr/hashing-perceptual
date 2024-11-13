#ifndef INTERFACE_H
#define INTERFACE_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <filesystem>
#include <fstream>


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
