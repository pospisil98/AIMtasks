#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

#include "Kernel.hpp"

Kernel::Kernel(int size)
{
    Resize(size);
}

void Kernel::Resize(int newSize) {
    this->size = (newSize % 2) == 0 ? newSize + 1 : newSize;
}

void Kernel::CreateGauss(double sigma)
{
    Resize(6 * sigma + 1);
    std::cout << "Resized kernel because of Gauusian to " << size << std::endl;

    /* https ://www.geeksforgeeks.org/gaussian-filter-generation-c/ */
    int center = size / 2;

    values.resize(size * size);

    // initialising standard deviation to 1.0
    double r, s = 2.0 * sigma * sigma;

    // sum is for normalization
    double sum = 0.0;

    // generating size^2 kernel
    for (int x = -center; x <= center; x++) {
        for (int y = -center; y <= center; y++) {
            r = sqrt(x * x + y * y);
            values[Index2Dto1D(x + center, y + center)] = (exp(-(r * r) / s));
            sum += values[Index2Dto1D(x + center, y + center)];
        }
    }

    // normalising the Kernel
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            values[Index2Dto1D(i, j)] /= sum;
}

void Kernel::CreateFromValues(std::vector<float>& values)
{
	this->size = static_cast<int>(sqrt(values.size()));
	std::copy(values.begin(), values.end(), this->values.begin());
}

void Kernel::SplitInto1DKernels(std::vector<float>& xDim, std::vector<float>& yDim)
{
    int center = size / 2;
    float sumX = 0.0f;
    float sumY = 0.0f;

    xDim.resize(size);
    yDim.resize(size);

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i == center) {
                xDim[j] = values[Index2Dto1D(i, j)];
                sumX += xDim[j];
            }

            if (j == center) {
                yDim[i] = values[Index2Dto1D(i, j)];
                sumY += yDim[i];
            }
        }
    }

    for (int i = 0; i < size; i++) {
        xDim[i] = xDim[i] / sumX;
        yDim[i] = yDim[i] / sumY;
    }
}

void Kernel::Print()
{
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            std::cout << values[Index2Dto1D(i, j)] << "   ";
        }
        std::cout << std::endl;
    }
}
