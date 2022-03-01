#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <execution>
#include <algorithm>

#include "Utils.hpp"

/// <summary>
/// Class representing image for purposes of AIM class. 
/// 
/// So far only grayscale images, it will be rewritten in future.
/// </summary>
class Image {
public:
    /// <summary> Width of image </summary>
	int width;
    /// <summary> Height of image </summary>
	int height;
    /// <summary> Number of components in each pixel </summary>
	int components;

    /// <summary> Quality of saved jpegs </summary>
    const int quality = 90;

    /// <summary>
    /// Enum representing possible monadic operations from Task I.
    /// </summary>
    enum class MonadicOperationType {
        NEGATIVE,
        THRESHOLD,
        BRIGHTNESS,
        CONTRAST,
        GAMMA_CORRECTION,
        QUANTIZATION,
        HISTOGRAM_EQUALIZATION
    };

    /// <summary>
    /// Construct image from given path.
    /// </summary>
    /// <param name="path">Path to file with image to be loaded.</param>
    Image(std::string path);

    /// <summary>
    /// Loads image from file given by path.
    /// </summary>
    /// <param name="path">Path to image file.</param>
    /// <returns>True on success.</returns>
    bool load(std::string path);


    /// <summary>
    /// Saves image data to file with same name as input with possibility of using a prefix.
    /// </summary>
    /// <param name="prefix">String to prepend before an output filename.</param>
    void save(std::string prefix = "");

    /// <summary>
    /// Performs given operation on image with specified value when needed.
    /// </summary>
    /// <param name="operation">Type of operation to do.</param>
    /// <param name="value">Input value of operation</param>
    void doOperation(MonadicOperationType operation, float value = 0.0f);


private:
    /// <summary> </summary>
    std::string path;

    /// <summary> Image data representing each pixel as float <0,1> in grayscale </summary>
    std::vector<float> data;
    /// <summary> Histogram of image </summary>
    std::vector<int> histogram;
    /// <summary> CDF of image computed from histogram </summary>
    std::vector<float> CDF;


    /// <summary>
    /// Converts loaded RGB image to grayscale only.
    /// </summary>
    /// <param name="image">Input array of loaded RGB image pixels.</param>
    /// <param name="nu">Width of image</param>
    /// <param name="nv">Height of image</param>
    void RGBToLuminanceImage(unsigned char* image, int nu, int nv);
    
    /// <summary>
    /// Compute images histogram.
    /// </summary>
    void computeHistogram();
    
    /// <summary>
    /// Compute images CDF from its histogram.
    /// </summary>
    void computeCDF();

    /// <summary>
    /// Compute histogram of image.
    /// </summary>
    void negative();

    /// <summary>
    /// Thresholds value of image pixels based on given value.
    /// </summary>
    /// <param name="value">Input value of operation</param>
    void threshold(float value);

    /// <summary>
    /// Modify image brightness by given value.
    /// </summary>
    /// <param name="value">Input value of operation</param>
    void brightness(float value);

    /// <summary>
    /// Modify image contrast by given value.
    /// </summary>
    /// <param name="value">Input value of operation</param>
    void contrast(float value);

    /// <summary>
    /// Do gamma corection of image.
    /// </summary>
    /// <param name="value">Input value of operation</param>
    void gammaCorrection(float value);

    /// <summary>
    /// Reduce number of color levels in image to given value.
    /// </summary>
    /// <param name="value">Input value of operation</param>
    void quantization(int value);

    /// <summary>
    /// Equalize image histogram.
    /// </summary>
    void histogramEqualization();
};

