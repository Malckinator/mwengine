#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <optional>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <any>
#include <variant>
#include <chrono>
#include <thread>

#include "Core.h"
#include "Logger.h"

#define EXIT(x) __debugbreak()

// math

#include "src/Math/MathLib.h"

#define GLM_FORCE_RADIANS
#define GLM_LEFT_HANDED  
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "vendor/GLM/glm/glm.hpp"
#include "vendor/GLM/glm/gtc/matrix_transform.hpp"
#include "vendor/GLM/glm/gtx/string_cast.hpp"
#include "vendor/GLM/glm/gtc/quaternion.hpp"