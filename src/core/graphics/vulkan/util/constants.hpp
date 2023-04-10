#pragma once

#include <vulkan/vulkan.h>

#include "common/primitives.hpp"

namespace gravel::vk {

inline constexpr usize kMaxFramesInFlight = 2;

/// Names of the validations layers we use.
inline constexpr const char* kValidationLayerNames[] {
    "VK_LAYER_KHRONOS_validation",
};

/// Names of the extensions that the Vulkan implementation must provide.
inline constexpr const char* kRequiredDeviceExtensions[] {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME,
    VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,

#ifdef GRAVEL_USE_VULKAN_SUBSET
    "VK_KHR_portability_subset",
#endif  // GRAVEL_USE_VULKAN_SUBSET
};

}  // namespace gravel::vk
