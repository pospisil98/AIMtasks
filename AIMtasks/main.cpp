#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <vector>
#include <sstream> 
#include <Windows.h>

#include "Image.hpp"


/// <summary>
/// Prints instructions for tool usage.
/// </summary>
void printHelp() {
    std::cout << "First task for AIM course" << std::endl << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << std::endl;
    std::cout << "Operations:" << std::endl;
    std::cout << "[n] - NEGATIVE" << std::endl;
    std::cout << "[t value] - THRESHOLD (value should be in <0, 1) range)" << std::endl;
    std::cout << "[b value] - BRIGHTNESS (value should be in <-1, 1> range)" << std::endl;
    std::cout << "[c value] - CONSTRAST (value should be in <0, inf) range)" << std::endl;
    std::cout << "[g value] - GAMMA CORRECTION (value should be in <0, inf) range)" << std::endl;
    std::cout << "[k value] - QUANTIZATION (value should be in <0, inf) range and integer)" << std::endl;
    std::cout << "[h] - HISTOGRAM EQUALIZATION" << std::endl;
}

/// <summary>
/// Parses inputline string into operation and its input value.
/// </summary>
/// <param name="inLine">String to be parsed</param>
/// <param name="operation">Reference where to save characted present in input string</param>
/// <param name="value">Reference where to save value present in input string</param>
void parseInputLine(std::string inLine, char &operation, std::string &value) {
    std::stringstream ss(inLine);

    std::string s;
    std::vector<std::string> parts;
    while (std::getline(ss, s, ' ')) {
        parts.push_back(s);
    }
    
    operation = parts[0][0];

    if (parts.size() > 1) {
        value = parts[1];
    }
}


int main()
{
    //std::string inputPath = "in.jpg";
    std::string inputPath = "in_eq.jpg";

    std::string inLine;
    std::string value;
    char operation;

    Image image(inputPath);

    printHelp();
    do {    
        std::cout << std::endl << "Your input: ";
        std::getline(std::cin, inLine);

        parseInputLine(inLine, operation, value);
        
        float fValue = 0.0f;
        if (value != "") {
            fValue = std::stof(value);
        }

        switch (operation)
        {
        case 'n':   
            std::cout << "Doing NEGATIVE operation and saving" << std::endl;
            image.doOperation(Image::MonadicOperationType::NEGATIVE);
            break;
        case 't':
            if (0.0f >= fValue || 1.0f <= fValue) {
                std::cout << "Please provide correct input values." << std::endl;
                break;
            }

            std::cout << "Doing THRESHOLD (" << value << ") operation and saving" << std::endl;
            image.doOperation(Image::MonadicOperationType::THRESHOLD, fValue);
            break;
        case 'b':
            if (-1.0f >= fValue || 1.0f <= fValue) {
                std::cout << "Please provide correct input values." << std::endl;
                break;
            }

            std::cout << "Doing BRIGHTNESS (" << value << ") operation and saving" << std::endl;
            image.doOperation(Image::MonadicOperationType::BRIGHTNESS, fValue);
            break;
        case 'c':
            if (0.0f >= fValue) {
                std::cout << "Please provide correct input values." << std::endl;
                break;
            }

            image.doOperation(Image::MonadicOperationType::CONTRAST, fValue);
            break;
        case 'g':
            if (0.0f >= fValue) {
                std::cout << "Please provide correct input values." << std::endl;
                break;
            }

            std::cout << "Doing GAMMA CORECTION (" << value << ") operation and saving" << std::endl;
            image.doOperation(Image::MonadicOperationType::GAMMA_CORRECTION, fValue);
            break;
        case 'k':
            if (0.0f >= fValue) {
                std::cout << "Please provide correct input values." << std::endl;
                break;
            }

            std::cout << "Doing QUANTIZATION (" << value << ") operation and saving" << std::endl;
            image.doOperation(Image::MonadicOperationType::QUANTIZATION, fValue);
            break;
        case 'h':
            std::cout << "Doing HISTOGRAM EQUALIZATION operation and saving" << std::endl;
            image.doOperation(Image::MonadicOperationType::HISTOGRAM_EQUALIZATION);
            break;
        case 'q':
            std::cout << "Quitting app" << std::endl;
            break;
        default:
            std::cout << "Unknown input, please try again" << std::endl;
            break;
        }
    } while (operation != 'q');
}
