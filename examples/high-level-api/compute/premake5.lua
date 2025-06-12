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
        "{COPYFILE}	 \"DrawParticles.shaderobj\" \"bin/%{cfg.buildcfg}/DrawParticles.shaderobj\"",
        "{COPYFILE}	 \"UpdateParticles.shaderobj\" \"bin/%{cfg.buildcfg}/UpdateParticles.shaderobj\""
    }

    filter "system:windows"
        links {
            "gdi32",
            "user32",
            "shell32",
            "comdlg32",
            "ws2_32"
        }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
        symbols "On"