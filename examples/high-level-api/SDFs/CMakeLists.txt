add_executable(AstralCanvasSDFs)

target_sources(AstralCanvasSDFs PRIVATE
    "Program.cpp"
    "Json.cpp"
    "ModelLoader.cpp")

target_link_libraries(AstralCanvasSDFs PUBLIC
    LGFX
    Astral.Canvas.LGFX)

target_include_directories(AstralCanvasSDFs PUBLIC
    "../../../Astral.Core/Astral.Core")

add_custom_command(
    TARGET AstralCanvasSDFs  
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/DrawSDFs.shaderobj"
        "${CMAKE_CURRENT_SOURCE_DIR}/CreateMeshSDF.shaderobj"
        $<TARGET_FILE_DIR:AstralCanvasSDFs>
)