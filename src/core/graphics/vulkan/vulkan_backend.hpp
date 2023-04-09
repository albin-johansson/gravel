#pragma once

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/type/vector.hpp"
#include "engine/backend.hpp"
#include "graphics/buffers.hpp"
#include "graphics/vulkan/allocator.hpp"
#include "graphics/vulkan/buffer.hpp"
#include "graphics/vulkan/buffers.hpp"
#include "graphics/vulkan/cmd/command_pool.hpp"
#include "graphics/vulkan/device.hpp"
#include "graphics/vulkan/fence.hpp"
#include "graphics/vulkan/instance.hpp"
#include "graphics/vulkan/managed.hpp"
#include "graphics/vulkan/pipeline/descriptor_pool.hpp"
#include "graphics/vulkan/pipeline/pipeline_cache.hpp"
#include "graphics/vulkan/pipeline_builder.hpp"
#include "graphics/vulkan/render_pass.hpp"
#include "graphics/vulkan/sampler.hpp"
#include "graphics/vulkan/semaphore.hpp"
#include "graphics/vulkan/surface.hpp"
#include "graphics/vulkan/swapchain.hpp"
#include "util/arrays.hpp"

namespace gravel::vlk {

inline constexpr VkDescriptorPoolSize kDescriptorPoolSizes[] {
    {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 256},
    {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 256},
};

/// Stores resources required to allow for multiple frames in flight at the same time.
struct FrameData final {
  VkCommandBuffer command_buffer {VK_NULL_HANDLE};

  Semaphore image_available_semaphore;
  Semaphore render_finished_semaphore;
  Fence in_flight_fence {true};

  Buffer static_matrix_ubo {Buffer::uniform(sizeof(StaticMatrices))};
  Buffer material_ubo {Buffer::uniform(sizeof(MaterialBuffer))};

  DescriptorPool descriptor_pool {2'048,
                                  kDescriptorPoolSizes,
                                  array_length(kDescriptorPoolSizes)};
};

class VulkanBackend final : public Backend {
 public:
  GRAVEL_DELETE_COPY(VulkanBackend);
  GRAVEL_DELETE_MOVE(VulkanBackend);

  explicit VulkanBackend(SDL_Window* window);

  ~VulkanBackend() noexcept override;

  void stop() override;

  void on_init(Scene& scene) override;

  void on_quit() override;

  void on_event(const SDL_Event& event) override;

  auto begin_frame() -> Result override;

  void end_frame() override;

  void render_scene(const Scene& scene,
                    const Vec2& framebuffer_size,
                    Dispatcher& dispatcher) override;

  void set_environment_texture(Scene& scene, const Path& path) override;

  void load_model(Scene& scene, const Path& path) override;

  [[nodiscard]] auto get_primary_framebuffer_handle() -> void* override;

  [[nodiscard]] auto should_quit() const -> bool override { return mQuit; }

 private:
  Instance mInstance;
  Surface mSurface;
  VkPhysicalDevice mGPU {VK_NULL_HANDLE};
  Device mDevice;
  Allocator mAllocator;

  Swapchain mSwapchain;
  RenderPass mRenderPass;
  Sampler mSampler;

  PipelineCache mPipelineCache;
  DescriptorSetLayoutBuilder mDescriptorSetLayoutBuilder;
  PipelineLayoutBuilder mPipelineLayoutBuilder;
  PipelineBuilder mPipelineBuilder;

  DescriptorSetLayout mShadingDescriptorSetLayout;
  PipelineLayout mShadingPipelineLayout;
  Pipeline mShadingPipeline;

  CommandPool mCommandPool;

  Vector<FrameData> mFrames;
  usize mFrameIndex {0};

  MaterialBuffer mMaterialBuffer;
  StaticMatrices mStaticMatrices;

  bool mQuit {false};
  bool mResizedFramebuffer : 1 {false};

  void create_shading_pipeline();
  void create_frame_data();
  void prepare_imgui_for_vulkan();
};

}  // namespace gravel::vlk
