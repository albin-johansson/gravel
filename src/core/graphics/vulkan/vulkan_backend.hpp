#pragma once

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "engine/backend.hpp"
#include "graphics/vulkan/allocator.hpp"
#include "graphics/vulkan/device.hpp"
#include "graphics/vulkan/instance.hpp"
#include "graphics/vulkan/render_pass.hpp"
#include "graphics/vulkan/shading_pipeline.hpp"
#include "graphics/vulkan/surface.hpp"
#include "graphics/vulkan/swapchain.hpp"

namespace gravel::vlk {

class VulkanBackend final : public Backend {
 public:
  explicit VulkanBackend(SDL_Window* window);

  void stop() override;

  void on_init(Scene& scene) override;

  void on_quit() override;

  void on_event(const SDL_Event& event) override;

  void begin_frame() override;

  void end_frame() override;

  void render_scene(const Scene& scene,
                    const Vec2& framebuffer_size,
                    Dispatcher& dispatcher) override;

  [[nodiscard]] auto get_primary_framebuffer_handle() -> void* override;

  [[nodiscard]] auto should_quit() const -> bool override { return mQuit; }

 private:
  Instance mInstance;
  Surface mSurface;
  VkPhysicalDevice mGPU {};
  Device mDevice;
  Swapchain mSwapchain;
  RenderPass mRenderPass;
  ShadingPipeline mShadingPipeline;
  Allocator mAllocator;

  bool mQuit {false};

  void prepare_imgui_for_vulkan();
};

}  // namespace gravel::vlk
