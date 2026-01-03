project "AstralCanvasCompute"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++14"
    staticruntime "Off"
    targetdir "bin/%{cfg.buildcfg}"
    includedirs {
        "../../../include",
        "../../../Astral.Core/Astral.Core",
        "../../../dependencies/glfw/include"
    }
    links {"LGFX", "Astral.Canvas.LGFX", "GLFW"}

    files {
        "Program.cpp",
        "Json.cpp"
    }

    postbuildcommands { 
        "{COPYFILE}	 \"DrawParticles.func\" \"bin/%{cfg.buildcfg}/DrawParticles.func\"",
        "{COPYFILE}	 \"UpdateParticles.func\" \"bin/%{cfg.buildcfg}/UpdateParticles.func\""
    }

    filter "system:windows"
        links {
            "gdi32",
            "user32",
            "shell32",
            "comdlg32",
            "ws2_32"
        }
    filter "system:macosx"
        links {
            "Cocoa.framework",
            "IOKit.framework",
            "CoreFoundation.framework",
            "QuartzCore.framework"
        }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
        symbols "On"