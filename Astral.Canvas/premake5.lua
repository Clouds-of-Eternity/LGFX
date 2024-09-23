project "Astral.Canvas.LGFX"
    kind "StaticLib"
    language "C++"
    cppdialect "C++11"
    staticruntime "Off"
    targetdir "bin/%{cfg.buildcfg}"
    includedirs {
        "../include",
        "../Astral.Core/Astral.Core",
        "../src"
    }
    links {"LGFX" }

    files {
        "**.cpp"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"