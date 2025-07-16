# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)


# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# prevent installing to system directories. 
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")


# Project name
project(KBDTrainer VERSION 1.0)

# C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)


add_subdirectory(SDL EXCLUDE_FROM_ALL)

set(SDLTTF_VENDORED ON) # tell SDL_ttf to build its own dependencies
set(SDLTTF_PLUTOSVG OFF)
add_subdirectory(SDL_ttf EXCLUDE_FROM_ALL)


set(SDLIMAGE_VENDORED ON)
set(SDLIMAGE_AVIF OFF)	# disable formats we don't use to make the build faster and smaller.
set(SDLIMAGE_BMP OFF)
set(SDLIMAGE_JPEG OFF)
set(SDLIMAGE_WEBP OFF)
add_subdirectory(SDL_image EXCLUDE_FROM_ALL)

set(HEADER_FILES
    include/input.h
    include/render.h
    include/game.h
)

add_definitions(-D_AMD64_)

# Manually specify source files
set(SOURCE_FILES
    src/main.c
    src/input.c
    src/render.c
    src/game.c
)

include_directories(include)

# Add XInput library for Windows
if(WIN32)
    link_libraries(xinput)
endif()

# Create executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC 
	SDL3_ttf::SDL3_ttf      # remove if you are not using SDL_ttf
	SDL3_image::SDL3_image	# remove if you are not using SDL_image
    	SDL3::SDL3              # If using satelite libraries, SDL must be the last item in the list. 
)
target_compile_definitions(${PROJECT_NAME} PUBLIC SDL_MAIN_USE_CALLBACKS)

# Build the exe in the base project folder
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin"
)

# Get DLL locations
get_target_property(SDL3_DLL_PATH SDL3::SDL3 $<TARGET_FILE>)
get_target_property(SDL3_IMAGE_DLL_PATH SDL3_image::SDL3_image $<TARGET_FILE>)
get_target_property(SDL3_TTF_DLL_PATH SDL3_ttf::SDL3_ttf $<TARGET_FILE>)


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL3_DLL_PATH}"
        "${CMAKE_SOURCE_DIR}/bin"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL3_IMAGE_DLL_PATH}"
        "${CMAKE_SOURCE_DIR}/bin"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL3_TTF_DLL_PATH}"
        "${CMAKE_SOURCE_DIR}/bin"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "${CMAKE_SOURCE_DIR}/bin/assets"
    COMMENT "Copying SDL3 and SDL3_image DLLs to output directory"
)
