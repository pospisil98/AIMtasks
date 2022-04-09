#pragma once

/// <summary>
/// Namespace providing some utility functions or structures for using in AIM tasks.
/// </summary>
namespace Utils
{
	/// <summary>
	/// Struct for representing color of image pixels.
	/// </summary>
	struct Color {
		float r;
		float g;
		float b;
	};

	/// <summary>
	/// Converts RGB to luminance value.
	/// </summary>
	/// <param name="r">Value of Red component of color</param>
	/// <param name="g">Value of Green component of color</param>
	/// <param name="b">Value of Blue component of color</param>
	/// <returns>Luminance value computed from color</returns>
	inline float luminanceFromRGB(const float r, const float g, const float b)
	{
		return 0.2126f * r + 0.7152f * g + 0.0722f * b;
	}

	inline float GaussianValue(float value, float sigma)
	{
		return expf(-(value * value) / 2.0f * (sigma * sigma));
	}
}


