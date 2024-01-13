#pragma once

#include <cmath>

#define Pi 3.141592654
#define Tau (Pi * 2)

namespace mwengine::Math {
	template<typename T>
	inline T Sin(T x) {
		return sin(x);
	}

	template<typename T>
	inline T Cos(T x) {
		return cos(x);
	}

	template<typename T>
	inline T Tan(T x) {
		return tan(x);
	}

	template<typename T>
	inline T Asin(T x) {
		return asin(x);
	}

	template<typename T>
	inline T Acos(T x) {
		return acos(x);
	}

	template<typename T>
	inline T Atan(T x) {
		return atan(x);
	}

	template<typename T>
	inline T Radians(T degrees) {
		return degrees / 180 * Pi;
	}

	template<typename T>
	inline T Degrees(T radians) {
		return radians * 180 / Pi;
	}
}