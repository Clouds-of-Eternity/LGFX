project "LGFX"
    kind "StaticLib"
    language "C"
    cdialect "C99"
    rtti "Off"
    exceptionhandling "Off"
    staticruntime "Off"
    targetdir "../bin/%{cfg.buildcfg}"
    includedirs {
        "../include",
        "../dependencies/volk",
        "%{VULKAN_SDK}/include",
        "./",
    }

    files { 
        "**.c", 
        "**.cpp", 
        "**.h", 
        "../include/**.h",
        "../dependencies/volk/volk.c"
    }

    filter "system:windows"
        systemversion "latest"
        defines { "VK_USE_PLATFORM_WIN32_KHR" }

    filter "system:macosx"
        defines { "VK_USE_PLATFORM_MACOS_MVK" }

    filter "system:linux"
        defines { "VK_USE_PLATFORM_XLIB_KHR" }
        
    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"