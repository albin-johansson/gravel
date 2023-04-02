#include "sampler.hpp"

#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/util.hpp"

namespace gravel::vlk {

Sampler::Sampler()
{
  VkPhysicalDeviceProperties device_properties {};
  vkGetPhysicalDeviceProperties(get_gpu(), &device_properties);

  const VkSamplerCreateInfo create_info {
      .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,

      .magFilter = VK_FILTER_LINEAR,
      .minFilter = VK_FILTER_LINEAR,
      .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,

      .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
      .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
      .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,

      .mipLodBias = 0.0f,

      .anisotropyEnable = VK_TRUE,
      .maxAnisotropy = device_properties.limits.maxSamplerAnisotropy,

      .compareEnable = VK_FALSE,
      .compareOp = VK_COMPARE_OP_ALWAYS,

      .minLod = 0.0f,
      .maxLod = 0.0f,

      .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
      .unnormalizedCoordinates = VK_FALSE,
  };

  GRAVEL_VK_CALL(vkCreateSampler(get_device(), &create_info, nullptr, &mSampler),
                 "[VK] Could not create sampler");
}

Sampler::~Sampler() noexcept
{
  dispose();
}

void Sampler::dispose() noexcept
{
  if (mSampler != VK_NULL_HANDLE) {
    vkDestroySampler(get_device(), mSampler, nullptr);
  }
}

Sampler::Sampler(Sampler&& other) noexcept
    : mSampler {other.mSampler}
{
  other.mSampler = VK_NULL_HANDLE;
}

auto Sampler::operator=(Sampler&& other) noexcept -> Sampler&
{
  if (this != &other) {
    dispose();

    mSampler = other.mSampler;
    other.mSampler = VK_NULL_HANDLE;
  }

  return *this;
}

}  // namespace gravel::vlk
