#include "../../drivers/include/renderer_vulkan.h"
#include "../../drivers/include/renderer_opengl.h"
#include "../../drivers/include/peripherals_sdl.h"
namespace retro {
class GUI {
 public: 
  virtual void StartGUI();
  virtual void MainMenu();
};
class VulkanGUI : public GUI {
  void StartGUI();
};

class OpenGLGUI : public GUI {
  void StartGUI();
};
extern void DefaultImGuiStyle();

class VulkanViewport {
 public:
  VulkanViewport(gbVulkanGraphicsHandler vulkan, std::unique_ptr<SDL> sdl);
  void Destory();
  void Free();
  void Update(void* array_data);
  void LoadFromPath(const char* file_path);
  void LoadFromArray(void* array_data, VkDeviceSize array_size, uint32_t w,
                     uint32_t h);
  
  VkDescriptorSet descriptor_set_;  
 private:
  uint32_t mips_levels_{};
  VkSampler texture_sampler_{};
  VkImageView texture_image_view_{};
  VkDeviceMemory texture_image_memory_{};
  VkImage texture_image_{};
  VkDeviceSize array_size_{};
  uint32_t w_{};
  uint32_t h_{};

  // Pointers to vulkan logical device and its dependencies 
  std::unique_ptr<VkAllocationCallbacks> allocator_ = VK_NULL_HANDLE;
  std::unique_ptr<VkDevice> logical_device_;
  std::unique_ptr<VkPhysicalDevice> physical_device_;
  std::unique_ptr<VkQueue> graphics_queue_;
  std::unique_ptr<VkCommandPool> command_pool_;
  std::unique_ptr<VkDescriptorPool> descriptor_pool_;
  std::unique_ptr<VkDescriptorSetLayout> descriptor_set_layout_;
  std::unique_ptr<VkDescriptorSetLayout> texture_descriptor_set_layout;

  // Pointer to the SDL class
  std::unique_ptr<SDL> sdl_;

  // Texture Function
  void ViewPortCreateTextureImage(const char* image_path);
  //void CreateTextureImageView();
  //void ViewPortCreateTextureSampler();
  //void ViewPortCreateTextureDescriptorSet();
  //void ViewPortLoadImageFromArray(void* image_data, VkDeviceSize image_size,
  //                                uint32_t w, uint32_t h);

  // These functions are direct copies of Vulkan's memeber functions
  void LoadImageFromArray(void* image_data, VkDeviceSize image_size, uint32_t w,
                          uint32_t h);
  void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties, VkBuffer& buffer,
                    VkDeviceMemory& buffer_memory);

  void CreateImage(uint32_t width, uint32_t height, VkFormat format,
                   VkImageTiling tiling, VkImageUsageFlags usage,
                   VkMemoryPropertyFlags properties, VkImage& image,
                   VkDeviceMemory& image_memory);
  uint32_t FindMemoryType(uint32_t type_filter,
                          VkMemoryPropertyFlags properties); 
  void TransitionImageLayout(VkImage image, VkFormat format,
                             VkImageLayout old_layout, 
                             VkImageLayout new_layout);
  void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width,
                         uint32_t height);
  void CreateTextureImageView();
  void CreateImageViews();
  void CreateTextureSampler();
  VkImageView CreateImageView(VkImage image, VkFormat format,
                       VkImageAspectFlags aspect_flag); 
};
}  // namespace retro

namespace retro::gui::mainmenu {
extern void Start(); 
extern void DrawMenuBar();
extern void Titles();
}

