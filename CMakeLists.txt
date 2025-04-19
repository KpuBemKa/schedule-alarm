# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# file(GLOB EMBED_INPUT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/website/source/*")
# set(WEBSITE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/website")
# set(OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/data_image/index.html")

# add_custom_command(
#     OUTPUT ${OUTPUT_PATH}
#     COMMAND ${CMAKE_COMMAND} -E echo "Running site_builder.py before build..."
#     COMMAND ${CMAKE_COMMAND} -E env ${CMAKE_CURRENT_SOURCE_DIR}/website/venv/Scripts/python.exe ${WEBSITE_DIR}/site_builder.py ${WEBSITE_DIR}/source/index.html ${OUTPUT_PATH}
#     DEPENDS ${EMBED_INPUT_FILES} 
#     COMMENT "Embedding assets before build"
# )

# add_custom_target(generate_embedded_html ALL DEPENDS ${OUTPUT_PATH})

# include(${CMAKE_CURRENT_SOURCE_DIR}/website/CMakeLists.txt)
add_subdirectory(website)

project(http_server)

# spiffs_create_partition_image(my_spiffs_partition my_folder FLASH_IN_PROJECT)
