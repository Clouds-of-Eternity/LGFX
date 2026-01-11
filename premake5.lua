VULKAN_SDK = os.getenv("VULKAN_SDK")
if VULKAN_SDK == nil then
    VULKAN_SDK = "/usr/local";
end

workspace "LGFX"
    configurations { "Debug", "Release" }

    newoption {
        trigger = "x11",
        description = "Compile with xlib support"
    }
    newoption {
        trigger = "wayland",
        description = "Compile with wayland support"
    }
    newoption {
        trigger = "clang",
        description = "Utilise the clang compiler toolchain"
    }
    
    filter "options:clang"
        toolset "clang"
        buildoptions { "-g", "-gcodeview" }
        linkoptions { "-fuse-ld=lld", "-g" }

    filter "system:windows"
        defines { "WINDOWS" }
        system "windows"
        architecture "x86_64"

    filter "system:linux"
        defines { "LINUX", "POSIX" }
        system "linux"
        architecture "x86_64"
        filter "options:x11"
            defines {"X11", "GLFW_EXPOSE_NATIVE_X11"}
        filter "options:wayland"
            defines {"WAYLAND", "GLFW_EXPOSE_NATIVE_WAYLAND"}

    filter "system:macosx"
        defines { "MACOS", "POSIX", "GLFW_EXPOSE_NATIVE_COCOA" }
        system "macosx"

    include("dependencies/GLFW.lua")

    include("src")

    include("Astral.Canvas")
    
    include("ShaderCompiler")

    include("examples/high-level-api/triangle")

    include("examples/high-level-api/compute")