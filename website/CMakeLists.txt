file(GLOB EMBED_INPUT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/source/*")
# set(WEBSITE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/website")
set(OUTPUT_PATH "${CMAKE_SOURCE_DIR}/data_image/index.html")

add_custom_command(
    OUTPUT ${OUTPUT_PATH}
    COMMAND ${CMAKE_COMMAND} -E echo "Running site_builder.py before build..."
    COMMAND ${CMAKE_COMMAND} -E env ${CMAKE_CURRENT_SOURCE_DIR}/venv/Scripts/python.exe ${CMAKE_CURRENT_SOURCE_DIR}/site_builder.py ${CMAKE_CURRENT_SOURCE_DIR}/source/index.html ${OUTPUT_PATH}
    DEPENDS ${EMBED_INPUT_FILES} 
    COMMENT "Embedding assets before build"
)

add_custom_target(embed_website DEPENDS ${OUTPUT_PATH})