#pragma once

#include <vector>

#include <fftw3.h>

#include "Kernel.hpp"
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
    /// Enum representing whether do operation on image data or on image spectrum.
    /// </summary>
    enum class OperationDataSource {
        IMAGE,
        SPECTRUM
    };

    /// <summary>
    /// Construct image from given path.
    /// </summary>
    /// <param name="path">Path to file with image to be loaded.</param>
    Image(std::string path);

    /// <summary>
    /// Construct image from given data.
    /// </summary>
    /// <param name="imageData">Vector of floats of image data.</param>
    /// <param name="path">Path to file where to be saved.</param>
    Image(std::vector<float>& imageData, std::string path, int width, int height, int components);

    ~Image();

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
    void save(std::string prefix = "", OperationDataSource dataSource = OperationDataSource::IMAGE);

    /// <summary>
    /// Performs given operation on image with specified value when needed.
    /// </summary>
    /// <param name="operation">Type of operation to do.</param>
    /// <param name="value">Input value of operation</param>
    void doOperation(MonadicOperationType operation, float value = 0.0f);

    /// <summary>
    /// Compute images histogram.
    /// </summary>
    void computeHistogram();

    /// <summary>
    /// Computes spectrum of image with usage of Fourier Transform.
    /// </summary>
    void computeSpectrum();

    /// <summary>
    /// Do convolution with given kernel with specified method.
    /// </summary>
    /// <param name="kernel"> Kernel to convolute with. </param>
    /// <param name="type"> 2D or lineary separated 2D. </param>
    /// <param name="destination"> Vector where to save convoluted image. </param>
    void Convolute(Kernel& kernel, Kernel::Type type, std::vector<float>& destination);

    /// <summary>
    /// Applies bilateral filtering with given parameters and returns "filtered" data.
    /// </summary>
    /// <param name="spatialSigma"> Parameter of distance influence </param>
    /// <param name="brightnessSigma"> Parameter of color difference influence </param>
    /// <param name="outData"> Vector where to save filtered data </param>
    void ApplyBilateralFilter(
        const float spatialSigma,
        const float brightnessSigma,
        std::vector<float>& outData
    );

    /// <summary>
    /// Converts 2D index to 1D index to array (using internal image width).
    /// </summary>
    /// <param name="x">X coord</param>
    /// <param name="y">Y coord</param>
    /// <returns>Corresponding index in 1D array</returns>
    inline int Index2Dto1D(int x, int y) {
        return x + y * width;
    }

    /// <summary>
    /// Replaces image content with reconstruction from spectrum (possibly modified) using Inverse FT.
    /// </summary>
    /// <returns></returns>
    std::vector<float> reconstructImageFromSpectrum();

    /// <summary> Image data representing each pixel as float <0,1> in grayscale </summary>
    std::vector<float> data;
private:
    /// <summary> </summary>
    std::string path;


    /// <summary> Histogram of image </summary>
    std::vector<int> histogram;
    /// <summary> CDF of image computed from histogram </summary>
    std::vector<float> CDF;

    /// <summary> Spectrum of image modified that it is possible to show it to user </summary>
    std::vector<float> spectrum;
    /// <summary> Spectrum of image created by FT </summary>
    fftw_complex* complexSpectrum = nullptr;


    /// <summary>
    /// Converts loaded RGB image to grayscale only.
    /// </summary>
    /// <param name="image">Input array of loaded RGB image pixels.</param>
    /// <param name="nu">Width of image</param>
    /// <param name="nv">Height of image</param>
    void RGBToLuminanceImage(unsigned char* image, int nu, int nv);
    
    /// <summary>
    /// Compute images CDF from its histogram.
    /// </summary>
    void computeCDF();

    /// <summary>
    /// Compute histogram of image.
    /// </summary>
    void negative(OperationDataSource dataSource = Image::OperationDataSource::IMAGE);

    /// <summary>
    /// Thresholds value of image pixels based on given value.
    /// </summary>
    /// <param name="value">Input value of operation</param>
    void threshold(float value, OperationDataSource dataSource = Image::OperationDataSource::IMAGE);

    /// <summary>
    /// Modify image brightness by given value.
    /// </summary>
    /// <param name="value">Input value of operation</param>
    void brightness(float value, OperationDataSource dataSource = Image::OperationDataSource::IMAGE);

    /// <summary>
    /// Modify image contrast by given value.
    /// </summary>
    /// <param name="value">Input value of operation</param>
    void contrast(float value, OperationDataSource dataSource = Image::OperationDataSource::IMAGE);

    /// <summary>
    /// Do gamma corection of image.
    /// </summary>
    /// <param name="value">Input value of operation</param>
    void gammaCorrection(float value, OperationDataSource dataSource = Image::OperationDataSource::IMAGE);

    /// <summary>
    /// Reduce number of color levels in image to given value.
    /// </summary>
    /// <param name="value">Input value of operation</param>
    void quantization(int value, OperationDataSource dataSource = Image::OperationDataSource::IMAGE);

    /// <summary>
    /// Equalize image histogram.
    /// </summary>
    void histogramEqualization(OperationDataSource dataSource = Image::OperationDataSource::IMAGE);

    /// <summary>
    /// Do convolution (classical 2D) with given kernel
    /// </summary>
    void Convolute2D(Kernel& kernel, std::vector<float>& destination);

    /// <summary>
    /// Do convolution with given gernel (split 2D kernel into two 1D for better performance)
    /// </summary>
    void Convolute1D(
        std::vector<float>& xDim,
        std::vector<float>& yDim,
        std::vector<float>& source,
        std::vector<float>& destination,
        Kernel::Direction direction
    );
};

