#include "../include/renderer_vulkan.h"

// Vulkan Instance

// Creates a "Vulkan Instance" taking two parameters:
// Window: What window will Vulkan renderer to
// Application: The application Vulkan is under.
void retro::Vulkan::InitVulkanInstance(SDL_Window* window_,
                                          Application app) {
  // The Vulkan Instance sits at the top of the Vulkan hierarchy, the Vulkan
  // instance must be initiated with the application infomation ( name,
  // version, engine, etc). While also requiring the extensions of outside
  // libraries like SDL, GLSW, or other window libraries for Vulkan to render 
  // to a window.
  VkInstanceCreateInfo instance_info{};
  VkApplicationInfo app_info{};
  VkResult result;

  // Checks if validation layers are supported in this version of Vulkan
  // only prints to the console if the program was compiled in debug mode
  if (ValidationLayersEnabled && !VulkanValidationLayerSupported()) {
    spdlog::critical("Validation layers requested, but not available!");
  }

  // Init the application infomation to Vulkan
  app_info = {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName = app.name,
      .applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0),
      .pEngineName = app.name,
      .engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 0),
      .apiVersion = VK_API_VERSION_1_3,
  };
  // required_extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
  instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_info.pApplicationInfo = &app_info;

  // Vulkan Instances can have what are called "extensions". Extensions are
  // features which allow Vulkan to do more things it otherwise cannot do.
  // In this line of code, we are calling the get extension function
  // which will get the extensions needed to use SDL for the windowing
  // and the debug extension for the validation layers

  std::vector<const char*> sdl_extensions = GetExtensions(window_);
  instance_info.enabledExtensionCount =
      static_cast<uint32_t>(sdl_extensions.size());
  instance_info.ppEnabledExtensionNames = sdl_extensions.data();

  // If Validation layers were enabled, populate the debug infomation struct
  // and add validation layer data to the instance info.
  VkDebugUtilsMessengerCreateInfoEXT debug_info{};
  if (ValidationLayersEnabled) {
    instance_info.enabledLayerCount =
        static_cast<uint32_t>(validation_layers.size());
    instance_info.ppEnabledLayerNames = validation_layers.data();

    PopulateDebugMessengerCreateInfo(debug_info);
    instance_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_info;
  } else {
    instance_info.enabledLayerCount = 0;
    instance_info.pNext = nullptr;
  }

  // Create the Vulkan instance with all the infomation given above
  result = vkCreateInstance(&instance_info, allocator_, &instance_);
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to create Vulkan instance ", 
      VkResultToString(result));
    std::runtime_error("Failed to create Vulkan instance " +
      VkResultToString(result));
    return;
  }

  // Collect the infomation from the Vulkan instance, and print the extensions
  // to the terminal
  uint32_t extension_count = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
  std::vector<VkExtensionProperties> extensions(extension_count);
  vkEnumerateInstanceExtensionProperties(nullptr, &extension_count,
                                         extensions.data());
  spdlog::info("Available extensions:");
  for (const auto& extension : extensions) {
    spdlog::info("  {}", extension.extensionName);
  }
}

// Vulkan Extensions

// Gets extensions from the SDL library to allow Vulkan to do features it
// otherwise cannot do by itself
std::vector<const char*> retro::Vulkan::GetExtensions(SDL_Window* window_) {
  // Get the number of extensions sdl needs and increase the size of a vector
  // based on the number it gives, then push the extension names inside of
  // vector
  uint32_t sdl_extension_count = 0;
  if (!SDL_Vulkan_GetInstanceExtensions(window_, &sdl_extension_count,
                                        nullptr)) {
    spdlog::critical(
        "Failed to get the number of Vulkan instance extensions from SDL");
    return {};
  }

  std::vector<const char*> extensions(sdl_extension_count);
  if (sdl_extension_count > 0) {
    if (!SDL_Vulkan_GetInstanceExtensions(window_, &sdl_extension_count,
                                          extensions.data())) {
      spdlog::critical(
          "Failed to get the Vulkan instance extensions names "
          "from SDL");
      return {};
    }
  }

  // Adds the debug extensions if validation layers were enabled
  if (ValidationLayersEnabled) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
}