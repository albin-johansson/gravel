#include "fence.hpp"

#include <limits>  // numeric_limits

#include "common/debug/assert.hpp"
#include "common/primitives.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/util/vk_call.hpp"

namespace gravel::vk {
namespace {

inline constexpr uint64 kMaxU64 = std::numeric_limits<uint64>::max();

}  // namespace

void FenceDeleter::operator()(VkFence fence) noexcept
{
  vkDestroyFence(get_device(), fence, nullptr);
}

auto create_fence(const bool signaled) -> Fence
{
  GRAVEL_ASSERT(get_device() != VK_NULL_HANDLE);

  const VkFenceCreateFlags flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

  const VkFenceCreateInfo create_info {
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      .pNext = nullptr,
      .flags = flags,
  };

  VkFence fence = VK_NULL_HANDLE;
  GRAVEL_VK_CALL(vkCreateFence(get_device(), &create_info, nullptr, &fence),
                 "[VK] Could not create fence");

  return Fence {fence};
}

void reset_fence(VkFence fence)
{
  GRAVEL_VK_CALL(vkResetFences(get_device(), 1, &fence), "[VK] Could not reset fence");
}

void wait_fence(VkFence fence)
{
  GRAVEL_VK_CALL(vkWaitForFences(get_device(), 1, &fence, VK_TRUE, kMaxU64),
                 "[VK] Could not wait for fence");
}

}  // namespace gravel::vk
