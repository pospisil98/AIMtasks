#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <vector>
#include <sstream> 
#include <Windows.h>

#include "Image.h"


void printHelp() {
    std::cout << "First task for AIM course" << std::endl << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << std::endl;
    std::cout << "Operations:" << std::endl;
    std::cout << "[n] - NEGATIVE" << std::endl;
    std::cout << "[t value] - THRESHOLD" << std::endl;
    std::cout << "[b value] - BRIGHTNESS (value should be in <-1, 1> range)" << std::endl;
    std::cout << "[c value] - CONSTRAST (value should be in <0, inf) range)" << std::endl;
    std::cout << "[g value] - GAMMA CORRECTION (value should be in <0, inf) range)" << std::endl;
    std::cout << "[k value] - QUANTIZATIN (value should be in <0, inf) range and integer)" << std::endl;
}

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
    char operation;
    std::string value;

    Image image(inputPath);

    printHelp();

    do {    
        std::cout << std::endl << "Your input: ";
        std::getline(std::cin, inLine);

        parseInputLine(inLine, operation, value);

        switch (operation)
        {
        case 'n':   
            std::cout << "Doing NEGATIVE operation and saving" << std::endl;
            image.doOperation(Image::MonadicOperationType::NEGATIVE);
            break;
        case 't':
            std::cout << "Doing THRESHOLD (" << value << ") operation and saving" << std::endl;
            image.doOperation(Image::MonadicOperationType::THRESHOLD, std::stof(value));
            break;
        case 'b':
            std::cout << "Doing BRIGHTNESS (" << value << ") operation and saving" << std::endl;
            image.doOperation(Image::MonadicOperationType::BRIGHTNESS, std::stof(value));
            break;
        case 'c':
            image.doOperation(Image::MonadicOperationType::CONTRAST, std::stof(value));
            break;
        case 'g':
            std::cout << "Doing GAMMA CORECTION (" << value << ") operation and saving" << std::endl;
            image.doOperation(Image::MonadicOperationType::GAMMA_CORRECTION, std::stof(value));
            break;
        case 'k':
            std::cout << "Doing QUANTIZATION (" << value << ") operation and saving" << std::endl;
            image.doOperation(Image::MonadicOperationType::QUANTIZATION, std::stof(value));
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
