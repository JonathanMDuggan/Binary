# Binary
Personal project for reverse engineering retro hardware/software

# Building the Project

For users who want to compile the program, it is important to have the following
configurations:

 - SDL, SDL_Image, and SDL_Mixer are dynamically linked to the executable.
Therefore, you need to drag and drop the SDL DLLs into the folder where the
executable is located.

- You need to install the Vulkan SDK for the program to compile. You can find
the download location here: https://www.lunarg.com/vulkan-sdk/.

- Visual Studio 2022 is the IDE I used to build this program, and I highly
recommend using it.

- Even though I never used any platform-dependent libraries, I've only compiled
this program on Windows 11. I do not know if it can build on other operating
systems.
