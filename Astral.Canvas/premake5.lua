project "Astral.Canvas.LGFX"
    kind "StaticLib"
    language "C++"
    rtti "Off"
    cppdialect "C++14"
    exceptionhandling "Off"
    staticruntime "Off"
    targetdir "bin/%{cfg.buildcfg}"
    includedirs {
        "../include",
        "../Astral.Core/Astral.Core",
        "../src",
        "../dependencies/glfw/include"
    }
    defines "GLFW_EXPOSE_NATIVE_WIN32"
    links {"LGFX", "GLFW" }

    files {
        "**.cpp"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"