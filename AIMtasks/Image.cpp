#define _CRT_SECURE_NO_WARNINGS

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "Image.h"

Image::Image(std::string path) {
    this->path = path;

    load(path);

    computeHistogram();
    computeCDF();
}

bool Image::load(std::string path) {
    int ok = stbi_info(path.c_str(), &width, &height, &components);

    if (ok == 1) {
        unsigned char* indata = stbi_load(path.c_str(), &width, &height, &components, 0);

        data.clear();
        data.resize(width * height);

        RGBToLuminanceImage(indata, width, height);

        stbi_image_free(indata);
        return true;
    }

    return false;
}

void Image::save(std::string prefix) {
    std::vector<char> outputData(width * height * 3);

    for (int i = 0; i < data.size(); i++)
    {
        outputData[i * 3 + 0] = data[i] * 255;
        outputData[i * 3 + 1] = data[i] * 255;
        outputData[i * 3 + 2] = data[i] * 255;
    }

    std::string out = prefix.append(path);

    std::cout << out << std::endl;

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

void Image::negative() {
    std::for_each(
        std::execution::par_unseq,
        data.begin(),
        data.end(),
        [](auto&& item) {
            item = 1 - item;
        }
    );
}

void Image::threshold(float value) {
    std::for_each(
        std::execution::par_unseq,
        data.begin(),
        data.end(),
        [value](auto&& item) {
            item = item < value ? 0.0f : 1.0f;
        }
    );
}

void Image::brightness(float value) {
    std::for_each(
        std::execution::par_unseq,
        data.begin(),
        data.end(),
        [value](auto&& item) {
            item = std::clamp(item + value, 0.0f, 1.0f);
        }
    );
}

void Image::contrast(float value) {
    std::for_each(
        std::execution::par_unseq,
        data.begin(),
        data.end(),
        [value](auto&& item) {
            item = std::clamp(item * value, 0.0f, 1.0f);
        }
    );
}

void Image::gammaCorrection(float value) {
    std::for_each(
        std::execution::par_unseq,
        data.begin(),
        data.end(),
        [value](auto&& item) {
            item = std::clamp(std::powf(item, value), 0.0f, 1.0f);
        }
    );
}

void Image::quantization(int value) {
    std::for_each(
        std::execution::par_unseq,
        data.begin(),
        data.end(),
        [value](auto&& item) {
            item = std::clamp((std::floor(item * value) / value), 0.0f, 1.0f);
        }
    );
}

void Image::histogramEqualization() {
    for (int i = 0; i < data.size(); i++) {
        data[i] = std::clamp(CDF[data[i] * 255], 0.0f, 1.0f);
    }
}

