#include "../../drivers/include/renderer_vulkan.h"
#include "../../drivers/include/renderer_opengl.h"
#include "../../drivers/include/peripherals_sdl.h"
#include "../../io/include/io.h"
namespace binary {
typedef struct VulkanViewportInfo { 
  uint32_t* mips_levels{};
  VkSampler* texture_sampler{};
  VkImageView* texture_image_view{};
  VkDeviceMemory* texture_image_memory{};
  VkImage* texture_image{};
  VkDeviceSize* array_size{};
  VkDescriptorSet* texture_descriptor_set{};
  uint32_t w{};
  uint32_t h{};
} VulkanViewportInfo; 

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
  VulkanViewport(gbVulkanGraphicsHandler vulkan, SDL* sdl);
  ~VulkanViewport(); 
  void Destroy();
  void Free();
  void Update(void* array_data);
  void LoadFromPath(const char* file_path);
  void LoadFromArray(void* array_data, VkDeviceSize array_size, uint32_t w,
                     uint32_t h);
  VulkanViewportInfo GetViewportInfo();
 private:
  VkDescriptorSet texture_descriptor_set_;
  uint32_t mips_levels_{};
  VkSampler texture_sampler_{}; 
  VkImageView texture_image_view_{};
  VkDeviceMemory texture_image_memory_{};
  VkImage texture_image_{};
  VkDeviceSize array_size_{};
  uint32_t w_{};
  uint32_t h_{};
  
  // Pointers to vulkan logical device and its dependencies 
  VkAllocationCallbacks* allocator_ = VK_NULL_HANDLE;
  VkDevice* logical_device_;
  VkPhysicalDevice* physical_device_;
  VkQueue* graphics_queue_;
  VkCommandPool* command_pool_;
  VkDescriptorPool* descriptor_pool_;
  VkDescriptorPool* imgui_pool;


  // Pointer to the SDL class
  SDL* sdl_;

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
  void CreateTextureDescriptorSet();
};
}  // namespace binary

namespace binary::gui::mainmenu {
extern void Start(VulkanViewportInfo* texture);
extern void DrawMenuBar(VulkanViewportInfo* texture);
extern void Titles(VulkanViewportInfo* texture);
}

