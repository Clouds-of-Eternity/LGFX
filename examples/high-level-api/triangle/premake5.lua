project "AstralCanvasTriangle"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++11"
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
        "{COPYFILE}	 \"Triangle.shaderobj\" \"bin/%{cfg.buildcfg}/Triangle.shaderobj\""
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

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"