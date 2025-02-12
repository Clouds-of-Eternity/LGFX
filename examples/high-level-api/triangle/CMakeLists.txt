add_executable(AstralCanvasTriangle)

target_sources(AstralCanvasTriangle PRIVATE
    "Program.cpp"
    "Json.cpp")

target_link_libraries(AstralCanvasTriangle PUBLIC
    LGFX
    Astral.Canvas.LGFX)

target_include_directories(AstralCanvasTriangle PUBLIC
    "../../../Astral.Core/Astral.Core")

add_custom_command(
    TARGET AstralCanvasTriangle  
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/Triangle.shaderobj"
        $<TARGET_FILE_DIR:AstralCanvasTriangle>
)