#pragma once

#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/primitives.hpp"

namespace gravel::vlk {

class Device final {
 public:
  GRAVEL_DELETE_COPY(Device);
  GRAVEL_DELETE_MOVE(Device);

  Device();

  ~Device();

  /// Submits rendering commands to the graphics queue.
  void submit(VkCommandBuffer cmd_buffer,
              VkSemaphore image_available_semaphore,
              VkSemaphore render_finished_semaphore,
              VkFence in_flight_fence);

  /// Presents a swapchain image to the presentation queue.
  auto present(VkSwapchainKHR swapchain,
               uint32 swapchain_image_index,
               VkSemaphore render_finished_semaphore) -> VkResult;

  [[nodiscard]] auto get() -> VkDevice { return mDevice; }
  [[nodiscard]] auto get_graphics_queue() -> VkQueue { return mGraphicsQueue; }
  [[nodiscard]] auto get_present_queue() -> VkQueue { return mPresentQueue; }

 private:
  VkDevice mDevice {VK_NULL_HANDLE};
  VkQueue mGraphicsQueue {VK_NULL_HANDLE};
  VkQueue mPresentQueue {VK_NULL_HANDLE};
};

}  // namespace gravel::vlk
