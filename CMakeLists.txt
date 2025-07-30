# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# Allow version to be passed from CI/CD or default to 1.0.0
if(NOT DEFINED PROJECT_VERSION)
    set(PROJECT_VERSION "1.0.0")
endif()

# Parse version components for CPack
string(REGEX MATCH "([0-9]+)\\.([0-9]+)\\.([0-9]+)" VERSION_MATCH ${PROJECT_VERSION})
if(VERSION_MATCH)
    set(PROJECT_VERSION_MAJOR ${CMAKE_MATCH_1})
    set(PROJECT_VERSION_MINOR ${CMAKE_MATCH_2})
    set(PROJECT_VERSION_PATCH ${CMAKE_MATCH_3})
else()
    # Fallback if version parsing fails
    set(PROJECT_VERSION_MAJOR 1)
    set(PROJECT_VERSION_MINOR 0)
    set(PROJECT_VERSION_PATCH 0)
endif()

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# prevent installing to system directories. 
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")


# Project name with version
project(KBDTrainer VERSION ${PROJECT_VERSION})

# C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)


add_subdirectory(SDL EXCLUDE_FROM_ALL)

set(SDLTTF_VENDORED ON) # tell SDL_ttf to build its own dependencies
set(SDLTTF_PLUTOSVG OFF)
add_subdirectory(SDL_ttf EXCLUDE_FROM_ALL)


set(SDLIMAGE_VENDORED ON)
set(SDLIMAGE_AVIF OFF)	# disable formats we don't use to make the build faster and smaller.
set(SDLIMAGE_BMP ON)	# enable BMP support for game assets
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
get_target_property(SDL3_DLL_PATH SDL3::SDL3 IMPORTED_LOCATION_DEBUG)
get_target_property(SDL3_IMAGE_DLL_PATH SDL3_image::SDL3_image IMPORTED_LOCATION_DEBUG)
get_target_property(SDL3_TTF_DLL_PATH SDL3_ttf::SDL3_ttf IMPORTED_LOCATION_DEBUG)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:SDL3::SDL3>"
        "${CMAKE_SOURCE_DIR}/bin"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:SDL3_image::SDL3_image>"
        "${CMAKE_SOURCE_DIR}/bin"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:SDL3_ttf::SDL3_ttf>"
        "${CMAKE_SOURCE_DIR}/bin"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "${CMAKE_SOURCE_DIR}/bin/assets"
    COMMENT "Copying SDL3 and SDL3_image DLLs to output directory"
)

# ==============================================================================
# PACKAGING CONFIGURATION
# ==============================================================================

# Install the executable
install(TARGETS ${PROJECT_NAME}
    DESTINATION .
    COMPONENT Runtime
)

# Install DLLs
install(FILES
    "$<TARGET_FILE:SDL3::SDL3>"
    "$<TARGET_FILE:SDL3_image::SDL3_image>"
    "$<TARGET_FILE:SDL3_ttf::SDL3_ttf>"
    DESTINATION .
    COMPONENT Runtime
)

# Install assets
install(DIRECTORY "${CMAKE_SOURCE_DIR}/assets/"
    DESTINATION assets
    COMPONENT Runtime
)

# Install documentation
install(FILES
    "${CMAKE_SOURCE_DIR}/README.md"
    "${CMAKE_SOURCE_DIR}/INSTALL.md"
    "${CMAKE_SOURCE_DIR}/setup.bat"
    DESTINATION .
    COMPONENT Runtime
    OPTIONAL
)

# License file
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

# WiX-specific settings
if(WIN32)
    set(CPACK_WIX_LICENSE_RTF "${CMAKE_SOURCE_DIR}/LICENSE.rtf")
    set(CPACK_WIX_UPGRADE_GUID "A1B2C3D4-E5F6-7890-ABCD-123456789012")
    set(CPACK_WIX_PRODUCT_GUID "*")  # Generate new GUID for each version
    # Fix for WiX empty directory component duplication
    set(CPACK_WIX_SKIP_PROGRAM_FOLDER TRUE)
    set(CPACK_COMPONENT_GROUP_RUNTIME_REQUIRED TRUE)
    set(CPACK_COMPONENTS_ALL Runtime)
endif()

# CPack configuration
if(WIN32)
    # Default to ZIP, but allow NSIS if available
    set(CPACK_GENERATOR "ZIP;NSIS")
else()
    set(CPACK_GENERATOR "TGZ")
endif()
set(CPACK_PACKAGE_NAME "KBD Trainer")
set(CPACK_PACKAGE_VENDOR "dkustec")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Korean Backdash Training Tool for Tekken")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(CPACK_PACKAGE_INSTALL_DIRECTORY "KBD Trainer")

# NSIS specific settings
set(CPACK_NSIS_PACKAGE_NAME "KBD Trainer")
set(CPACK_NSIS_DISPLAY_NAME "KBD Trainer - Tekken Input Training")
set(CPACK_NSIS_HELP_LINK "https://github.com/dkustec/kbd-trainer")
set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/dkustec/kbd-trainer")
set(CPACK_NSIS_CONTACT "github.com/dkustec")
set(CPACK_NSIS_MODIFY_PATH OFF)
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

# Create desktop shortcut
set(CPACK_NSIS_CREATE_ICONS_EXTRA
    "CreateShortCut '$DESKTOP\\\\KBD Trainer.lnk' '$INSTDIR\\\\KBDTrainer.exe'"
)

# Remove desktop shortcut on uninstall
set(CPACK_NSIS_DELETE_ICONS_EXTRA
    "Delete '$DESKTOP\\\\KBD Trainer.lnk'"
)

# Set executable icon (if we had one)
# set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/assets/icon.ico")
# set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/assets/icon.ico")

include(CPack)
