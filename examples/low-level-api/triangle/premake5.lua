project "Triangle"
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
    links {"LGFX", "GLFW"}

    files {
        "Program.cpp"
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