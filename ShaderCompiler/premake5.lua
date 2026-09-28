project "ShaderCompiler"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++14"
    staticruntime "Off"
    targetdir "bin/%{cfg.buildcfg}"
    includedirs {
        "%{VULKAN_SDK}/include",
        "../Astral.Core/Astral.Core",
        "../Astral.Core/Astral.Plane"
    }
    files {
        "ShaderCompiler.cpp",
        "HeaderImpls.cpp",
        "main.cpp"
    }
    libdirs "%{VULKAN_SDK}/lib"
    libdirs "%{VULKAN_SDK}/Lib"
    links { "slang" }
    
    filter "system:macosx"
        runpathdirs "/deps/"