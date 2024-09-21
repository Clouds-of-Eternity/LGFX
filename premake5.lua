workspace "LGFX"
    configurations { "Debug", "Release" }

    filter "system:windows"
        defines { "WINDOWS", "GLFW_EXPOSE_NATIVE_WIN32" }
        system "windows"

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
            defines { "_GLFW_WIN32", "_CRT_SECURE_NO_WARNINGS" }
            files {
                "src/win32_module.c",
                "src/win32_init.c",
                "src/win32_joystick.c",
                "src/win32_monitor.c",
                "src/win32_time.c",
                "src/win32_thread.c",
                "src/win32_window.c",
                "src/wgl_context.c",
                "src/egl_context.c",
                "src/osmesa_context.c"
            }
    
        filter "system:macosx"
            defines { "_GLFW_COCOA", "GLFW_EXPOSE_NATIVE_COCOA" }
            files {
                "src/posix_module.c",
                "src/cocoa_init.m",
                "src/cocoa_monitor.m",
                "src/cocoa_window.m",
                "src/cocoa_joystick.m",
                "src/cocoa_time.c",
                "src/nsgl_context.m",
                "src/posix_thread.c",
                "src/osmesa_context.c",
                "src/egl_context.c"
            }
    
        filter "system:linux"
            defines { "_GLFW_X11" }
            files {
                "src/posix_module.c",
                "src/x11_init.c",
                "src/x11_monitor.c",
                "src/x11_window.c",
                "src/xkb_unicode.c",
                "src/posix_time.c",
                "src/posix_thread.c",
                "src/glx_context.c",
                "src/egl_context.c",
                "src/osmesa_context.c",
                "src/linux_joystick.c"
            }

    project "LGFX"
        kind "StaticLib"
        language "C"
        rtti "Off"
        exceptionhandling "Off"
        staticruntime "Off"
        targetdir "bin/%{cfg.buildcfg}"
        includedirs {
            "include", 
            "dependencies/glfw/include",
            "dependencies/volk",
            "Astral.Core/Astral.Core"
        }
        links {"GLFW"}

        files { 
            "src/**.c", 
            "src/**.cpp", 
            "src/**.h", 
            "src/**.hpp", 
            "include/**.h",
            "dependencies/volk/volk.c"
        }

        filter "system:windows"
            systemversion "latest"
            defines { "ASTRALCANVAS_VULKAN", "VK_USE_PLATFORM_WIN32_KHR" }

        filter "system:macosx"
            defines { "ASTRALCANVAS_METAL", "VK_USE_PLATFORM_MACOS_MVK" }
            files "src/**.mm"

        filter "system:linux"
            links { "%{VULKAN_SDK}/Lib/vulkan-1.lib" }
            includedirs "%{VULKAN_SDK}/Include"
            defines { "ASTRALCANVAS_VULKAN", "VK_USE_PLATFORM_XLIB_KHR" }
            
        filter "configurations:Debug"
            defines { "DEBUG" }
            symbols "On"

        filter "configurations:Release"
            defines { "NDEBUG" }
            optimize "On"