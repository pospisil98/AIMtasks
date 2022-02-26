#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <execution>
#include <algorithm>

#include "Utils.h"

class Image {
public:
	int width;
	int height;
	int components;

    const int quality = 90;

	std::string path;

	std::vector<float> data;
    std::vector<int> histogram;
    std::vector<float> CDF;

    enum MonadicOperationType {
        NEGATIVE,
        THRESHOLD,
        BRIGHTNESS,
        CONTRAST,
        GAMMA_CORRECTION,
        QUANTIZATION,
        HISTOGRAM_EQUALIZATION
    };

    Image(std::string path);

    bool load(std::string path);

    void save(std::string prefix = "");

    void doOperation(MonadicOperationType operation, float value = 0.0f);


private:
    void RGBToLuminanceImage(unsigned char* image, int nu, int nv);
    
    void computeHistogram();
    
    void computeCDF();

    void negative();

    void threshold(float value);

    void brightness(float value);

    void contrast(float value);

    void gammaCorrection(float value);

    void quantization(int value);

    void histogramEqualization();
};

