#pragma once

#include <vector>

class Kernel
{
public:
	/// <summary> Used for managing Convolute function - use normal 2D kernel or lineary separate it </summary>
	enum class Type {
		Kernel_1D,
		Kernel_2D
	};
	/// <summary> Direction in lineary separable kernels </summary>
	enum class Direction {
		Dir_X,
		Dir_Y
	};

	/// <summary> Size of kernel in pixels (width or height - square) </summary>
	int size;

	/// <summary> Values of kernel </summary>
	std::vector<float> values;

	/// <summary> Creates kernel with given size (ceil to nearest odd number) </summary>
	Kernel(int size);

	/// <summary> Resizes kernel to new size </summary>
	void Resize(int newSize);

	/// <summary> Generates values of kernel corresponding to Gaussian distribution </summary>
	void CreateGauss(double sigma = 1.0);

	/// <summary> Creates kernel from specified values for own kernels (edge detection, ...) </summary>
	void CreateFromValues(std::vector<float>& values);

	/// <summary> Splits current 2D kernel into two 1D ones </summary>
	void SplitInto1DKernels(std::vector<float>& xDim, std::vector<float>& yDim);

	/// <summary> Prints kernel values into console </summary>
	void Print();

	/// <summary>
	/// Converts 2D index to 1D index to array (using internal image width).
	/// </summary>
	/// <param name="x">X coord</param>
	/// <param name="y">Y coord</param>
	/// <returns>Corresponding index in 1D array</returns>
	inline int Index2Dto1D(int x, int y) {
		return x + y * size;
	}
};