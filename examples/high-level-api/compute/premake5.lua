project "AstralCanvasCompute"
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
    links {"Astral.Canvas.LGFX", "GLFW"}

    files {
        "Program.cpp",
        "Json.cpp"
    }

    -- postbuildcommands { 
    --     "{COPYFILE}	 \"Triangle.shaderobj\" \"bin/%{cfg.buildcfg}/Triangle.shaderobj\""
    -- }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"