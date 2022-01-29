#pragma once

// Standart
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <memory>
#include <thread>
#include <utility>
#include <cstdlib>
#include <cstddef>
#include <filesystem>
#include <cstring>
#include <string>
#include <stack>
#include <deque>
#include <array>
#include <vector>
#include <utility>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <optional>

// OPENGL/VULKAN
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

// ENTT
//#include <entt/entt.hpp>

// GLM
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_XYZW_ONLY
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>
#include "glm/gtx/hash.hpp"

namespace vec3 {
    GLM_INLINE GLM_CONSTEXPR glm::vec3 right = glm::vec3{1,0,0};
    GLM_INLINE GLM_CONSTEXPR glm::vec3 left = glm::vec3{-1,0,0};
    GLM_INLINE GLM_CONSTEXPR glm::vec3 up = glm::vec3{0,1,0};
    GLM_INLINE GLM_CONSTEXPR glm::vec3 down = glm::vec3{0,-1,0};
    GLM_INLINE GLM_CONSTEXPR glm::vec3 forward = glm::vec3{0,0,1};
    GLM_INLINE GLM_CONSTEXPR glm::vec3 back = glm::vec3{0,0,-1};
    GLM_INLINE GLM_CONSTEXPR glm::vec3 zero = glm::vec3{0,0,0};
}
namespace vec2 {
    GLM_INLINE GLM_CONSTEXPR glm::vec2 right = glm::vec2{1,0};
    GLM_INLINE GLM_CONSTEXPR glm::vec2 left = glm::vec2{-1,0};
    GLM_INLINE GLM_CONSTEXPR glm::vec2 up = glm::vec2{0,1};
    GLM_INLINE GLM_CONSTEXPR glm::vec2 down = glm::vec2{0,-1};
    GLM_INLINE GLM_CONSTEXPR glm::vec2 zero = glm::vec2{0,0};
}