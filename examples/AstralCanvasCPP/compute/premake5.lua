project "ComputeCPP"
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
    links {"LGFX", "AstralCanvasCPP", "GLFW"}

    files {
        "Program.cpp"
    }

    postbuildcommands { 
        "{COPYFILE}	 \"../../Shaders/DrawParticles.sfn\" \"bin/%{cfg.buildcfg}/DrawParticles.sfn\"",
        "{COPYFILE}	 \"../../Shaders/UpdateParticles.sfn\" \"bin/%{cfg.buildcfg}/UpdateParticles.sfn\""
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