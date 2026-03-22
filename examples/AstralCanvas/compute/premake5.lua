project "Compute"
    kind "ConsoleApp"
    language "C"
    cdialect "C11"
    staticruntime "Off"
    targetdir "bin/%{cfg.buildcfg}"
    includedirs {
        "../../../include",
        "../../../Astral.Core/Astral.Base",
        "../../../dependencies/glfw/include"
    }
    links {"LGFX", "AstralCanvas", "GLFW"}

    files {
        "Program.c"
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
    filter "system:linux"
        defines "_GLFW_X11"
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