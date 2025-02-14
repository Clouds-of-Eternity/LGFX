add_library(LGFX STATIC)

set_target_properties(LGFX PROPERTIES C_STANDARD 99)

target_sources(LGFX PRIVATE
    "LGFXImpl.c"
    "vulkan/LGFXVulkan.c"
    "vulkan/vk_mem_alloc.cpp"
    "sync.h"
    "memory.h"
    "Logging.h")

set_source_files_properties(
    "LGFXImpl.c"
    "vulkan/LGFXVulkan.c" 
    "sync.h"
    "memory.h"
    "Logging.h"
    PROPERTIES LANGUAGE C)

target_include_directories(LGFX PUBLIC 
    "../dependencies/volk"
    "../src")

target_include_directories(LGFX PUBLIC 
    "../include")

target_link_libraries(LGFX PUBLIC 
    volk
    glfw)
target_include_directories(LGFX PUBLIC 
    "${GLFW_SOURCE_DIR}/include"
)

target_link_libraries(LGFX PRIVATE ${Vulkan_LIBRARY})
target_include_directories(LGFX PRIVATE ${Vulkan_INCLUDE_DIR})

if(WIN32)
    target_compile_definitions(LGFX PUBLIC
        "VK_USE_PLATFORM_WIN32_KHR;WINDOWS;GLFW_EXPOSE_NATIVE_WIN32")
elseif(APPLE)
    target_compile_definitions(LGFX PUBLIC
        "VK_USE_PLATFORM_MACOS_MVK")
elseif(UNIX)
    target_compile_definitions(LGFX PUBLIC
        "VK_USE_PLATFORM_XLIB_KHR")
endif()