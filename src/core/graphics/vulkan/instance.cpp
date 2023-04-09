#include "instance.hpp"

#include <exception>  // terminate

#include <SDL2/SDL_vulkan.h>
#include <spdlog/spdlog.h>

#include "common/debug/error.hpp"
#include "common/primitives.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/util.hpp"
#include "init/window.hpp"
#include "util/arrays.hpp"

namespace gravel::vlk {
namespace {

[[nodiscard]] auto get_instance_extension_names(SDL_Window* window) -> Vector<const char*>
{
  uint32 extension_count = 0;
  SDL_Vulkan_GetInstanceExtensions(window, &extension_count, nullptr);

  Vector<const char*> extensions;
  extensions.resize(extension_count);
  SDL_Vulkan_GetInstanceExtensions(window, &extension_count, extensions.data());

#ifdef GRAVEL_USE_VULKAN_SUBSET
  extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif  // GRAVEL_USE_VULKAN_SUBSET

  if constexpr (kDebugBuild) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
}

VKAPI_ATTR auto debug_message_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                       VkDebugUtilsMessageTypeFlagsEXT,
                                       const VkDebugUtilsMessengerCallbackDataEXT* data,
                                       void*) -> VkBool32
{
  auto level = spdlog::level::debug;

  if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    level = spdlog::level::warn;
  }
  else if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    level = spdlog::level::err;
  }

  spdlog::log(level, "[Vulkan] {}", data->pMessage);

  if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    std::terminate();
  }

  return VK_FALSE;
}

template <typename T>
void load_function(VkInstance instance, T& func, const char* name)
{
  func = reinterpret_cast<T>(vkGetInstanceProcAddr(instance, name));
}

}  // namespace

Instance::Instance()
{
  spdlog::debug("[VK] Creating Vulkan instance...");

  const auto extension_names = get_instance_extension_names(get_window());

  const VkApplicationInfo application_info {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,

      .pApplicationName = "Gravel",
      .applicationVersion = VK_MAKE_VERSION(0, 1, 0),

      .pEngineName = "No Engine",
      .engineVersion = VK_MAKE_VERSION(0, 1, 0),

      .apiVersion = VK_API_VERSION_1_2,
  };

  VkInstanceCreateInfo instance_create_info {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,

      .flags = 0,
      .pApplicationInfo = &application_info,

      .enabledLayerCount = 0,
      .ppEnabledLayerNames = nullptr,

      .enabledExtensionCount = static_cast<uint32>(extension_names.size()),
      .ppEnabledExtensionNames = extension_names.data(),
  };

#ifdef GRAVEL_USE_VULKAN_SUBSET
  // Allow implementations that only provide a subset of the Vulkan spec, e.g. MoltenVK.
  instance_create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif  // GRAVEL_USE_VULKAN_SUBSET

  if constexpr (kDebugBuild) {
    spdlog::debug("[VK] Enabling validation layers");

    // Enable validation layers in debug builds
    instance_create_info.ppEnabledLayerNames = kValidationLayerNames;
    instance_create_info.enabledLayerCount = array_length(kValidationLayerNames);
  }

  GRAVEL_VK_CALL(vkCreateInstance(&instance_create_info, nullptr, &mInstance),
                 "[VK] Could not create instance");
  set_instance(mInstance);

  auto& functions = get_extension_functions();

  if constexpr (kDebugBuild) {
    load_function(mInstance,
                  functions.vkCreateDebugUtilsMessengerEXT,
                  "vkCreateDebugUtilsMessengerEXT");
    load_function(mInstance,
                  functions.vkDestroyDebugUtilsMessengerEXT,
                  "vkDestroyDebugUtilsMessengerEXT");

    init_debug_messenger();
  }

  load_function(mInstance,
                functions.vkCmdPushDescriptorSetKHR,
                "vkCmdPushDescriptorSetKHR");
  load_function(mInstance,
                functions.vkCmdPushDescriptorSetWithTemplateKHR,
                "vkCmdPushDescriptorSetWithTemplateKHR");
}

Instance::~Instance() noexcept
{
  if constexpr (kDebugBuild) {
    destroy_debug_messenger(mInstance, mDebugMessenger);
  }

  vkDestroyInstance(mInstance, nullptr);
}

void Instance::init_debug_messenger()
{
  const VkDebugUtilsMessengerCreateInfoEXT create_info {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .pNext = nullptr,
      .flags = 0,

      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,

      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,

      .pfnUserCallback = &debug_message_callback,
      .pUserData = nullptr,
  };

  GRAVEL_VK_CALL(create_debug_messenger(mInstance, &create_info, &mDebugMessenger),
                 "[VK] Could not create debug messenger");
}

auto get_physical_devices(VkInstance instance) -> Vector<VkPhysicalDevice>
{
  uint32 gpu_count = 0;
  vkEnumeratePhysicalDevices(instance, &gpu_count, nullptr);

  Vector<VkPhysicalDevice> gpus;
  gpus.resize(gpu_count);

  vkEnumeratePhysicalDevices(instance, &gpu_count, gpus.data());

  return gpus;
}

}  // namespace gravel::vlk