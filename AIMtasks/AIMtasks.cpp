#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <vector>
#include <execution>
#include <algorithm>
#include <sstream> 
#include <Windows.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "AIMtasks.h"


void printHelp() {
    std::cout << "First task for AIM course" << std::endl << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "[h] - HELP" << std::endl;
    std::cout << "[q] - QUIT" << std::endl;
    std::cout << "[f filename] - PATH TO INPUT" << std::endl;
    std::cout << "[o filename] - PATH TO OUTPUT" << std::endl;

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

void negative(std::vector<float>& image) {
    std::for_each(
        std::execution::par_unseq,
        image.begin(),
        image.end(),
        [] (auto&& item) {
            item = 1 - item;
        }
    );
}

void threshold(float value, std::vector<float>& image) {
    std::for_each(
        std::execution::par_unseq,
        image.begin(),
        image.end(),
        [value] (auto&& item) {
            if (item < value) {
                item = 0.0f;
            } else {
                item = 1.0f;
            }
        }
    );
}

void brightness(float value, std::vector<float>& image) {
    std::for_each(
        std::execution::par_unseq,
        image.begin(),
        image.end(),
        [value](auto&& item) {
            item = std::clamp(item + value, 0.0f, 1.0f);
        }
    );
}

void contrast(float value, std::vector<float>& image) {
    std::for_each(
        std::execution::par_unseq,
        image.begin(),
        image.end(),
        [value](auto&& item) {
            item = std::clamp(item * value, 0.0f, 1.0f);
        }
    );
}

void gammaCorrection(float value, std::vector<float>& image) {
    std::for_each(
        std::execution::par_unseq,
        image.begin(),
        image.end(),
        [value](auto&& item) {
            item = std::clamp(std::powf(item, value), 0.0f, 1.0f);
        }
    );
}

void quantization(float count, std::vector<float>& image) {
    std::for_each(
        std::execution::par_unseq,
        image.begin(),
        image.end(),
        [count](auto&& item) {
            item = std::clamp((std::floor(item * count) / count), 0.0f, 1.0f);
        }
    );
}

void computeHistogram(std::vector<int>& histogram, std::vector<float>& image) {
    histogram.clear();
    histogram.resize(256);

    for (float value : image) {
        int level = std::round(value * 255);

        histogram[level] += 1;
    }
}

void computeCDFfromHistogram(std::vector<float>& cdf, std::vector<int>& histogram, int pixelCount) {
    cdf.clear();
    cdf.resize(256);

    cdf[0] = histogram[0] / static_cast<float>(pixelCount);
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i - 1] + histogram[i] / static_cast<float>(pixelCount);
    }
}




void histogramEqualization(std::vector<float>& image) {
    std::vector<int> histogram;
    computeHistogram(histogram, image);

    std::vector<float> cdf;
    computeCDFfromHistogram(cdf, histogram, image.size());

    for (int i = 0; i < image.size(); i++) {
        float value = cdf[image[i] * 255];

        image[i] = std::clamp(value, 0.0f, 1.0f);
    }
}

inline float lumimance(const float r, const float g, const float b)
{
    return 0.2126f * r + 0.7152f * g + 0.0722f * b;
}

void RGBToLuminanceImage(unsigned char* image, int nu, int nv, std::vector<float>& output)
{
    for (int u = 0; u < nu; u++) {
        for (int v = 0; v < nv; v++) {
            int index = v * nu + u;

            float r = image[index * 3];
            float g = image[index * 3 + 1];
            float b = image[index * 3 + 2];

            float l = lumimance(r, g, b) / 256;

            output[index] = l;
        }
    }
}

void saveImage(std::vector<float>& data, std::string path, int x, int y) {
    std::vector<char> outputData(x * y * 3);

    for (int i = 0; i < x * y; i++)
    {
        outputData[i * 3 + 0] = data[i] * 255;
        outputData[i * 3 + 1] = data[i] * 255;
        outputData[i * 3 + 2] = data[i] * 255;
    }

    stbi_write_jpg(path.c_str(), x, y, 3, static_cast<void*>(outputData.data()), 90);
}

bool loadInputImage(std::vector<float>& data, std::string path, int& x, int& y, int& n) {
    int ok = stbi_info(path.c_str(), &x, &y, &n);

    if (ok == 1) {
        unsigned char* indata = stbi_load(path.c_str(), &x, &y, &n, 0);

        std::cout << "N:" << n << std::endl;
        
        data.clear();
        data.resize(x * y);

        RGBToLuminanceImage(indata, x, y, data);

        stbi_image_free(indata);
        return true;
    }

    return false;
}

int main()
{
    //std::string inputPath = "./in.jpg";
    std::string inputPath = "./in_eq.jpg";
    std::string outputPath = "./out.jpg";
    int x, y, n;
    std::vector<float> imageData;
    bool loadedImage = true;

    std::string inLine;
    char operation;
    std::string value;

    loadInputImage(imageData, inputPath, x, y, n);

    saveImage(imageData, outputPath, x, y);

    system("cls");
    printHelp();

    do {    
        std::cout << std::endl << "Your input: ";
        std::getline(std::cin, inLine);

        parseInputLine(inLine, operation, value);

        switch (operation)
        {
        case 'f':
            std::cout << "SET NEW INPUT TO: " << value << std::endl;

            if (loadInputImage(imageData, value, x, y, n)) {
                inputPath = value;
            } else {
                loadInputImage(imageData, inputPath, x, y, n);
                std::cout << "Problem with input." << std::endl;
            }

            break;
        case 'o':
            std::cout << "SET NEW OUTPUT TO: " << value << std::endl;
            outputPath = value;
            break;

        case 'n':   
            std::cout << "Doing NEGATIVE operation and saving to " << outputPath << std::endl;
            negative(imageData);
            saveImage(imageData, outputPath, x, y);
            break;
        case 't':
            std::cout << "Doing THRESHOLD (" << value << ") operation and saving to " << outputPath << std::endl;
            threshold(std::stof(value), imageData);
            saveImage(imageData, outputPath, x, y);
            break;
        case 'b':
            std::cout << "Doing BRIGHTNESS (" << value << ") operation and saving to " << outputPath << std::endl;
            brightness(std::stof(value), imageData);
            saveImage(imageData, outputPath, x, y);
            break;
        case 'c':
            std::cout << "Doing CONTRAST (" << value << ") operation and saving to " << outputPath << std::endl;
            contrast(std::stof(value), imageData);
            saveImage(imageData, outputPath, x, y);
            break;
        case 'g':
            std::cout << "Doing GAMMA CORECTION (" << value << ") operation and saving to " << outputPath << std::endl;
            gammaCorrection(std::stof(value), imageData);
            saveImage(imageData, outputPath, x, y);
            break;
        case 'k':
            std::cout << "Doing QUANTIZATION (" << value << ") operation and saving to " << outputPath << std::endl;
            quantization(std::stof(value), imageData);
            saveImage(imageData, outputPath, x, y);
            break;
        case 'h':
            std::cout << "Doing HISTOGRAM EQUALIZATION operation and saving to " << outputPath << std::endl;
            histogramEqualization(imageData);
            saveImage(imageData, outputPath, x, y);
            break;
        case 'q':
            std::cout << "QUIT" << std::endl;
            break;
        default:
            std::cout << "Unknown input, please try again" << std::endl;
            break;
        }
    } while (operation != 'q');
}
