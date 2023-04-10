#include "pipeline_cache.hpp"

#include "common/debug/assert.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/util/vk_call.hpp"

namespace gravel::vk {

void PipelineCacheDeleter::operator()(VkPipelineCache cache) noexcept
{
  vkDestroyPipelineCache(get_device(), cache, nullptr);
}

auto create_pipeline_cache() -> PipelineCache
{
  GRAVEL_ASSERT(get_device() != VK_NULL_HANDLE);

  const VkPipelineCacheCreateInfo create_info {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .initialDataSize = 0,
      .pInitialData = nullptr,
  };

  VkPipelineCache cache = VK_NULL_HANDLE;
  GRAVEL_VK_CALL(vkCreatePipelineCache(get_device(), &create_info, nullptr, &cache),
                 "[VK] Could not create pipeline cache");

  return PipelineCache {cache};
}

}  // namespace gravel::vk
