#ifndef INTERFACE_H
#define INTERFACE_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <SFML/Graphics.hpp>
#include <string>

sf::RenderWindow* createTab();
void runInterface(const std::string& imagePath1, const std::string& imagePath2);
sf::Texture loadImage(const cv::Mat& image);

#endif // INTERFACE_H
