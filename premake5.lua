VULKAN_SDK = os.getenv("VULKAN_SDK")

workspace "LGFX"
    configurations { "Debug", "Release" }

    filter "action:gmake2"
        toolset "clang"
        buildoptions { "-fpermissive", "-g", "-gcodeview" }
        linkoptions { "-fuse-ld=lld", "-g"}

    filter "system:windows"
        defines { "WINDOWS", "GLFW_EXPOSE_NATIVE_WIN32" }
        system "windows"
        architecture "x86_64"

    filter "system:linux"
        defines { "LINUX", "POSIX", "GLFW_EXPOSE_NATIVE_X11" }
        system "linux"

    filter "system:macosx"
        defines { "MACOS", "POSIX", "GLFW_EXPOSE_NATIVE_COCOA" }
        system "macosx"

    include("dependencies/GLFW.lua")

    include("src")

    include("Astral.Canvas")

    include("examples/low-level-api/triangle")

    include("examples/high-level-api/triangle")

    include("examples/high-level-api/compute")