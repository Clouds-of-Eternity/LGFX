project "AstralCanvas"
    kind "StaticLib"
    language "C"
    cdialect "C11"
    staticruntime "Off"
    targetdir "bin/%{cfg.buildcfg}"
    includedirs {
        "../include",
        "./",
        "../Astral.Core/Astral.Base",
        "../dependencies/glfw/include",
        "%{VULKAN_SDK}/include"
    }
    links {"LGFX", "GLFW" }

    files {
        "**.c"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"