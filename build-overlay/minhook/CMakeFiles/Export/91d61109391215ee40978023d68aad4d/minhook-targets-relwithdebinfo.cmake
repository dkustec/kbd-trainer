#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "minhook::minhook" for configuration "RelWithDebInfo"
set_property(TARGET minhook::minhook APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(minhook::minhook PROPERTIES
  IMPORTED_IMPLIB_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/minhook.x64.lib"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/minhook.x64.dll"
  )

list(APPEND _cmake_import_check_targets minhook::minhook )
list(APPEND _cmake_import_check_files_for_minhook::minhook "${_IMPORT_PREFIX}/lib/minhook.x64.lib" "${_IMPORT_PREFIX}/bin/minhook.x64.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
