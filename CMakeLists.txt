cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(LGFX)

find_package(Vulkan REQUIRED)

if(Vulkan_FOUND)
    message(STATUS "Using Vulkan Lib at: " ${Vulkan_LIBRARY})
else()
    message(FATAL_ERROR "Vulkan was not found")
endif()

add_subdirectory(dependencies/glfw)

if (WIN32)
   set(VOLK_STATIC_DEFINES "VK_USE_PLATFORM_WIN32_KHR")
endif()

add_subdirectory(dependencies/volk)
add_subdirectory(Astral.Canvas)

add_subdirectory(src)
add_subdirectory(examples/high-level-api/triangle)
add_subdirectory(examples/high-level-api/compute)