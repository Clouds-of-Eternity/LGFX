VULKAN_SDK = os.getenv("VULKAN_SDK")

workspace "LGFX"
    configurations { "Debug", "Release" }

    filter "system:windows"
        defines { "WINDOWS", "GLFW_EXPOSE_NATIVE_WIN32" }
        system "windows"
        architecture "x86_64"

    filter "system:linux"
        defines { "LINUX", "POSIX", "GLFW_EXPOSE_NATIVE_X11" }
        system "linux"

    filter "system:macosx"
        defines { "MACOS", "POSIX", "GLFW_EXPOSE_NATIVE_COCOA" }
        system "macosx"

    project "GLFW"
        kind "StaticLib"
        language "C"
        targetdir "bin/glfw/%{cfg.buildcfg}"
        staticruntime "Off"
        files {
            "dependencies/glfw/include/GLFW/glfw3.h",
            "dependencies/glfw/include/GLFW/glfw3native.h",
            "dependencies/glfw/src/glfw_config.h",
            "dependencies/glfw/src/context.c",
            "dependencies/glfw/src/init.c",
            "dependencies/glfw/src/input.c",
            "dependencies/glfw/src/monitor.c",
            "dependencies/glfw/src/vulkan.c",
            "dependencies/glfw/src/window.c",
            "dependencies/glfw/src/platform.c",
            "dependencies/glfw/src/null_init.c",
            "dependencies/glfw/src/null_joystick.c",
            "dependencies/glfw/src/null_monitor.c",
            "dependencies/glfw/src/null_joystick.c",
            "dependencies/glfw/src/null_window.c"
        }
    
        filter "configurations:Debug"
            defines { "DEBUG" }
            symbols "On"
    
        filter "configurations:Release"
            defines { "NDEBUG" }
            optimize "On"
    
        filter "system:windows"
            systemversion "latest"
            defines { "_GLFW_WIN32", "_CRT_SECURE_NO_WARNINGS", "GLFW_EXPOSE_NATIVE_WIN32" }
            files {
                "dependencies/glfw/src/win32_module.c",
                "dependencies/glfw/src/win32_init.c",
                "dependencies/glfw/src/win32_joystick.c",
                "dependencies/glfw/src/win32_monitor.c",
                "dependencies/glfw/src/win32_time.c",
                "dependencies/glfw/src/win32_thread.c",
                "dependencies/glfw/src/win32_window.c",
                "dependencies/glfw/src/wgl_context.c",
                "dependencies/glfw/src/egl_context.c",
                "dependencies/glfw/src/osmesa_context.c"
            }
    
        filter "system:macosx"
            defines { "_GLFW_COCOA", "GLFW_EXPOSE_NATIVE_COCOA" }
            files {
                "dependencies/glfw/src/posix_module.c",
                "dependencies/glfw/src/cocoa_init.m",
                "dependencies/glfw/src/cocoa_monitor.m",
                "dependencies/glfw/src/cocoa_window.m",
                "dependencies/glfw/src/cocoa_joystick.m",
                "dependencies/glfw/src/cocoa_time.c",
                "dependencies/glfw/src/nsgl_context.m",
                "dependencies/glfw/src/posix_thread.c",
                "dependencies/glfw/src/osmesa_context.c",
                "dependencies/glfw/src/egl_context.c"
            }
    
        filter "system:linux"
            defines { "_GLFW_X11" }
            files {
                "dependencies/glfw/src/posix_module.c",
                "dependencies/glfw/src/x11_init.c",
                "dependencies/glfw/src/x11_monitor.c",
                "dependencies/glfw/src/x11_window.c",
                "dependencies/glfw/src/xkb_unicode.c",
                "dependencies/glfw/src/posix_time.c",
                "dependencies/glfw/src/posix_thread.c",
                "dependencies/glfw/src/glx_context.c",
                "dependencies/glfw/src/egl_context.c",
                "dependencies/glfw/src/osmesa_context.c",
                "dependencies/glfw/src/linux_joystick.c"
            }

    project "LGFX"
        kind "StaticLib"
        language "C"
        cdialect "C99"
        rtti "Off"
        exceptionhandling "Off"
        staticruntime "Off"
        targetdir "bin/%{cfg.buildcfg}"
        includedirs {
            "include",
            "dependencies/volk",
            "%{VULKAN_SDK}/Include",
            "src",
        }

        files { 
            "src/**.c", 
            "src/**.cpp", 
            "src/**.h", 
            "include/**.h",
            "dependencies/volk/volk.c"
        }

        filter "system:windows"
            systemversion "latest"
            defines { "VK_USE_PLATFORM_WIN32_KHR" }

        filter "system:macosx"
            defines { "VK_USE_PLATFORM_MACOS_MVK" }
            files "src/**.mm"

        filter "system:linux"
            defines { "VK_USE_PLATFORM_XLIB_KHR" }
            
        filter "configurations:Debug"
            defines { "DEBUG" }
            symbols "On"

        filter "configurations:Release"
            defines { "NDEBUG" }
            optimize "On"

    include("Astral.Canvas")

    include("examples/low-level-api/triangle")

    include("examples/high-level-api/triangle")