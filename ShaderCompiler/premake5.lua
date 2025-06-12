project "ShaderCompiler"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++14"
    staticruntime "Off"
    targetdir "bin/%{cfg.buildcfg}"
    includedirs {
        "%{VULKAN_SDK}/include",
        "../Astral.Core/Astral.Core"
    }
    files {
        "ShaderCompiler.cpp",
        "HeaderImpls.cpp",
        "main.cpp"
    }
    links { "%{VULKAN_SDK}/Lib/slang" }