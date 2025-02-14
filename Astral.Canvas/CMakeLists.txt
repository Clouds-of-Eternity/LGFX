add_library(Astral.Canvas.LGFX STATIC)

target_sources(Astral.Canvas.LGFX PRIVATE
    "Application.cpp"
    "InputState.cpp"
    "Shader.cpp"
    "ShaderState.cpp"
    "Window.cpp"
)

target_include_directories(Astral.Canvas.LGFX PUBLIC
    "../Astral.Core/Astral.Core"
    "../src"
    "${GLFW_SOURCE_DIR}/include"
)

target_link_libraries(Astral.Canvas.LGFX PUBLIC
    LGFX)
