#define _CRT_SECURE_NO_WARNINGS

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


#include <iostream>
#include <vector>
#include <string>
#include <execution>
#include <algorithm>

#include "Image.hpp"

Image::Image(std::string path) {
    this->path = path;

    load(path);

    computeHistogram();
    computeCDF();
}

Image::Image(std::vector<float>& imageData, std::string path, int width, int height, int components) {
    this->path = path;
    this->width = width;
    this->height = height;
    this->components = components;


    this->data.resize(imageData.size());
    for (int i = 0; i < imageData.size(); i++) {
        this->data[i] = imageData[i];
    }
}

Image::~Image() {
    if (complexSpectrum != nullptr) {
        fftw_free(complexSpectrum);
    }
}

bool Image::load(std::string path) {
    int ok = stbi_info(path.c_str(), &width, &height, &components);

    if (ok == 1) {
        unsigned char* indata = stbi_load(path.c_str(), &width, &height, &components, 0);

        data.clear();
        data.resize(width * height);

        std::cout << "Comp: " << components << std::endl;
        if (components == 3) {
            RGBToLuminanceImage(indata, width, height);
        }

        stbi_image_free(indata);
        return true;
    }

    return false;
}

void Image::save(std::string prefix, OperationDataSource dataSource) {
    std::string out = prefix.append(path);
    std::cout << out << std::endl;

    std::vector<float>& imageData = dataSource == Image::OperationDataSource::IMAGE ? data : spectrum;
    std::vector<char> outputData(width * height * 3);

    for (int i = 0; i < imageData.size(); i++) {
        int val = imageData[i] * 255;

        outputData[i * 3 + 0] = val;
        outputData[i * 3 + 1] = val;
        outputData[i * 3 + 2] = val;
    }

    stbi_write_jpg(out.c_str(), width, height, components, static_cast<void*>(outputData.data()), quality);

}

void Image::doOperation(MonadicOperationType operation, float value) {
    switch (operation) {
    case MonadicOperationType::NEGATIVE:
        negative();
        save("n_");
        break;
    case MonadicOperationType::THRESHOLD:
        threshold(value);
        save("t_");
        break;
    case MonadicOperationType::BRIGHTNESS:
        brightness(value);
        save("b_");
        break;
    case MonadicOperationType::CONTRAST:
        contrast(value);
        save("c_");
        break;
    case MonadicOperationType::GAMMA_CORRECTION:
        gammaCorrection(value);
        save("g_");
        break;
    case MonadicOperationType::QUANTIZATION:
        quantization(static_cast<int>(value));
        save("q_");
        break;
    case MonadicOperationType::HISTOGRAM_EQUALIZATION:
        histogramEqualization();
        save("h_");
        break;
    default:
        break;
    }
}

void Image::RGBToLuminanceImage(unsigned char* image, int nu, int nv)
{
    for (int u = 0; u < nu; u++) {
        for (int v = 0; v < nv; v++) {
            int index = v * nu + u;

            float r = image[index * 3];
            float g = image[index * 3 + 1];
            float b = image[index * 3 + 2];

            float l = Utils::luminanceFromRGB(r, g, b) / 256.0f;

            data[index] = l;
        }
    }
}

void Image::computeHistogram() {
    histogram.clear();
    histogram.resize(256);

    for (float value : data) {
        int level = std::round(value * 255);

        histogram[level] += 1;
    }
}

void Image::computeCDF() {
    float pixelCount = width * height;

    if (histogram.empty()) {
        computeHistogram();
    }

    CDF.clear();
    CDF.resize(256);

    CDF[0] = histogram[0] / pixelCount;
    for (int i = 1; i < 256; i++) {
        CDF[i] = CDF[i - 1] + histogram[i] / pixelCount;
    }
}

void Image::computeSpectrum() {
    int imageSize = width * height;
    std::vector<float> tempSpectrum(imageSize);

    fftw_complex* sourceImage = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * imageSize);
    complexSpectrum = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * imageSize);

    fftw_plan fwPlan = fftw_plan_dft_2d(width, height, sourceImage, complexSpectrum, FFTW_FORWARD, FFTW_ESTIMATE);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            sourceImage[y * width + x][0] = (double)data[y * width + x];
            sourceImage[y * width + x][1] = 0.0;
        }
    }

    fftw_execute(fwPlan);
    fftw_destroy_plan(fwPlan);
    fftw_free(sourceImage);

    
    // Modify generated complex spectrum to be able to display it
    double maximalMagnitude = 0.0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double re = complexSpectrum[y * width + x][0];
            double im = complexSpectrum[y * width + x][1];
            double mag = sqrt(re * re + im * im);

            maximalMagnitude = std::max(maximalMagnitude, mag);
        }
    }

    //const double factor = log(1.0 + maximalMagnitude);

    spectrum.resize(imageSize);  
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double re = complexSpectrum[y * width + x][0];
            double im = complexSpectrum[y * width + x][1];
            
            double mag = sqrt(re * re + im * im);
            /*
            tempSpectrum[y * width + x] = mag;

            //spectrum[y * width + x] = mag;
            //spectrum[Index2Dto1D((x + (width / 2 + 1)) % width, (y + (height / 2) + (x > width / 2 ? 1 : 0)) % height)] = mag;
            */
            spectrum[Index2Dto1D((x + (width / 2 + 1)) % width, (y + (height / 2)) % height)] = log10(1.0 + mag);
        }
    }
}

std::vector<float> Image::reconstructImageFromSpectrum() {
    int imageSize = width * height;

    fftw_complex* restored = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * imageSize);
    fftw_plan bwPlan = fftw_plan_dft_2d(width, height, complexSpectrum, restored, FFTW_BACKWARD, FFTW_ESTIMATE);

    fftw_execute(bwPlan);

    // Rescale computed values
    for (int i = 0; i < imageSize; i++) {
        restored[i][0] = restored[i][0] / imageSize;
        restored[i][1] = restored[i][1] / imageSize;
    }

    // Save magnitude to image
    std::vector<float> restoredImage(imageSize);
    for (int i = 0; i < imageSize; i++) {
        double re = restored[i][0];
        double im = restored[i][1];
        double mag = sqrt(re * re + im * im);

        restoredImage[i] = (float)mag;
    }

    fftw_destroy_plan(bwPlan);
    fftw_free(restored);

    return restoredImage;
}

void Image::negative(OperationDataSource dataSourceType) {
    std::vector<float>& imageData = dataSourceType == Image::OperationDataSource::IMAGE ? data : spectrum;

    std::for_each(
        std::execution::par_unseq,
        imageData.begin(),
        imageData.end(),
        [](auto&& item) {
            item = 1 - item;
        }
    );
}

void Image::threshold(float value, OperationDataSource dataSourceType) {
    std::vector<float>& imageData = dataSourceType == Image::OperationDataSource::IMAGE ? data : spectrum;

    std::for_each(
        std::execution::par_unseq,
        imageData.begin(),
        imageData.end(),
        [value](auto&& item) {
            item = item < value ? 0.0f : 1.0f;
        }
    );
}

void Image::brightness(float value, OperationDataSource dataSourceType) {
    std::vector<float>& imageData = dataSourceType == Image::OperationDataSource::IMAGE ? data : spectrum;

    std::for_each(
        std::execution::par_unseq,
        imageData.begin(),
        imageData.end(),
        [value](auto&& item) {
            item = std::clamp(item + value, 0.0f, 1.0f);
        }
    );
}

void Image::contrast(float value, OperationDataSource dataSourceType) {
    std::vector<float>& imageData = dataSourceType == Image::OperationDataSource::IMAGE ? data : spectrum;

    std::for_each(
        std::execution::par_unseq,
        imageData.begin(),
        imageData.end(),
        [value](auto&& item) {
            item = std::clamp(item * value, 0.0f, 1.0f);
        }
    );
}

void Image::gammaCorrection(float value, OperationDataSource dataSourceType) {
    std::vector<float>& imageData = dataSourceType == Image::OperationDataSource::IMAGE ? data : spectrum;

    std::for_each(
        std::execution::par_unseq,
        imageData.begin(),
        imageData.end(),
        [value](auto&& item) {
            item = std::clamp(std::powf(item, value), 0.0f, 1.0f);
        }
    );
}

void Image::quantization(int value, OperationDataSource dataSourceType) {
    std::vector<float>& imageData = dataSourceType == Image::OperationDataSource::IMAGE ? data : spectrum;

    std::for_each(
        std::execution::par_unseq,
        imageData.begin(),
        imageData.end(),
        [value](auto&& item) {
            item = std::clamp((std::floor(item * value) / value), 0.0f, 1.0f);
        }
    );
}

void Image::histogramEqualization(OperationDataSource dataSourceType) {
    std::vector<float>& imageData = dataSourceType == Image::OperationDataSource::IMAGE ? data : spectrum;

    for (int i = 0; i < imageData.size(); i++) {
        imageData[i] = std::clamp(CDF[imageData[i] * 255], 0.0f, 1.0f);
    }
}

void Image::Convolute(Kernel& kernel, Kernel::Type type, std::vector<float>& destination) {
    switch (type) {
        case Kernel::Type::Kernel_1D: {
            std::vector<float> xDim;
            std::vector<float> yDim;
            kernel.SplitInto1DKernels(xDim, yDim);

            std::vector<float> tmpData;
            Convolute1D(xDim, yDim, data, tmpData, Kernel::Direction::Dir_X);
            Convolute1D(xDim, yDim, tmpData, destination, Kernel::Direction::Dir_Y);
            break;
        } case Kernel::Type::Kernel_2D: {
            Convolute2D(kernel, destination);
            break;
        }
    }
}

void Image::Convolute2D(Kernel& kernel, std::vector<float>& destination) {
    int center = kernel.size / 2;
    destination.resize(width * height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float newPixelValue = 0.0f;
            
            for (int kY = 0; kY < kernel.size; kY++) {
                for (int kX = 0; kX < kernel.size; kX++) {
                    int xFinal = std::clamp(x + (kX - center), 0, width - 1);
                    int yFinal = std::clamp(y + (kY - center), 0, height - 1);

                    newPixelValue += data[Index2Dto1D(xFinal, yFinal)] * kernel.values[kX + kY * kernel.size];
                }
            }

            destination[Index2Dto1D(x, y)] = newPixelValue;
        }
    }
}

void Image::Convolute1D(
    std::vector<float>& xDim,
    std::vector<float>& yDim,
    std::vector<float>& source,
    std::vector<float>& destination,
    Kernel::Direction direction 
) {
    int kernelSize = xDim.size();
    int center = kernelSize / 2;
    destination.resize(source.size());

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float newPixelValue = 0.0f;

            for (int i = 0; i < kernelSize; i++) {
                int offset = i - center;

                if (direction == Kernel::Direction::Dir_X) {
                    int xFinal = std::clamp(x + offset, 0, width - 1);
                    int yFinal = y;

                    newPixelValue += source[Index2Dto1D(xFinal, yFinal)] * xDim[i];
                } else {
                    int xFinal = x;
                    int yFinal = std::clamp(y + offset, 0, height - 1);
    
                    newPixelValue += source[Index2Dto1D(xFinal, yFinal)] * yDim[i];
                }
            }

            destination[Index2Dto1D(x, y)] = newPixelValue;
        }
    }
}

void Image::ApplyBilateralFilter(
    const float spatialSigma,
    const float brightnessSigma,
    std::vector<float>& outData
) {
    outData.resize(data.size());

    int filterSize = 6 * spatialSigma + 1;
    int center = filterSize / 2;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float intensitySum = 0.0f;
            float normalization = 0.0f;

            for (int fy = 0; fy < filterSize; fy++) {
                int yOffset = fy - center;
                for (int fx = 0; fx < filterSize; fx++) {
                    int xOffset = fx - center;

                    int imageFilterPosY = std::clamp(y + yOffset, 0, height - 1);
                    int imageFilterPosX = std::clamp(x + xOffset, 0, width - 1);

                    // Get distance from center of "kernel"
                    float deltaX = (float)(xOffset - fx) / (float)filterSize;
                    float deltaY = (float)(yOffset - fy) / (float)filterSize;
                    float distFromCenter = sqrtf((deltaX * deltaX) + (deltaY * deltaY));

                    // Get difference in intensities with log scaling
                    float intensityDiff = logf(data[y * width + x]) - logf(data[imageFilterPosY * width + imageFilterPosX]);
                    
                    // Eval gaussians
                    float distGauss = Utils::GaussianValue(distFromCenter, spatialSigma);
                    float intensityGauss = Utils::GaussianValue(intensityDiff, brightnessSigma);

                    // Calc intesity and store that for later usage
                    float intensity = distGauss * intensityGauss * data[imageFilterPosY * width + imageFilterPosX];
                    intensitySum += intensity;
                    normalization += (distGauss * intensityGauss);
                }
            }

            // Save normalized value
            outData[y * width + x] = intensitySum / normalization;
        }
    }
}
