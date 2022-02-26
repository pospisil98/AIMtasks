#pragma once

namespace Utils {
	struct Color {
		float r;
		float g;
		float b;
	};

	inline float luminanceFromRGB(const float r, const float g, const float b)
	{
		return 0.2126f * r + 0.7152f * g + 0.0722f * b;
	}
}


