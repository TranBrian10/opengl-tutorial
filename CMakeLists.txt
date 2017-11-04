
cmake_minimum_required(VERSION 2.8.9)
project(opengl-tutorial)

#Bring the headers
include_directories(${CMAKE_SOURCE_DIR}/dependencies/include)

IF(APPLE)

  #Add GLFW static library
  add_library(GLFW_LIBRARY STATIC IMPORTED) # or SHARED instead of STATIC
  set_target_properties(GLFW_LIBRARY PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/dependencies/lib/macos/libglfw3.a"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/dependencies/include")

  #Add GLEW static library
  add_library(GLEW_LIBRARY STATIC IMPORTED) # or SHARED instead of STATIC
  set_target_properties(GLEW_LIBRARY PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/dependencies/lib/macos/libGLEW.a"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/dependencies/include")

  #Add FreeType static library
  add_library(FREETYPE_LIBRARY STATIC IMPORTED) # or SHARED instead of STATIC
  set_target_properties(FREETYPE_LIBRARY PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/dependencies/lib/macos/libfreetype.a"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/dependencies/include")

  #Add bzip2 static library
  add_library(BZIPTWO_LIBRARY STATIC IMPORTED) # or SHARED instead of STATIC
  set_target_properties(BZIPTWO_LIBRARY PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/dependencies/lib/macos/libbz2.a"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/dependencies/include")

  #Add zlib static library
  add_library(ZLIB_LIBRARY STATIC IMPORTED) # or SHARED instead of STATIC
  set_target_properties(ZLIB_LIBRARY PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/dependencies/lib/macos/libz.a"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/dependencies/include")

  #Add png16 static library
  add_library(PNG_LIBRARY STATIC IMPORTED) # or SHARED instead of STATIC
  set_target_properties(PNG_LIBRARY PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/dependencies/lib/macos/libpng16.a"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/dependencies/include")

   #SET(GUI_TYPE MACOSX_BUNDLE)
   FIND_LIBRARY(IOKIT_LIBRARY IOKit)
   FIND_LIBRARY(COCOA_LIBRARY Cocoa)
   FIND_LIBRARY(CORE_VIDEO_LIBRARY CoreVideo)
   MARK_AS_ADVANCED (IOKIT_LIBRARY
                     COCOA_LIBRARY
                     CORE_VIDEO_LIBRARY)
   SET(EXTRA_LIBS ${IOKIT_LIBRARY} ${COCOA_LIBRARY}
    ${CORE_VIDEO_LIBRARY})

#the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES src/sina/*.c src/sina/*.cpp src/sina/*.hpp)

add_executable(testProj ${SOURCES})
target_link_libraries(testProj GLEW_LIBRARY GLFW_LIBRARY FREETYPE_LIBRARY PNG_LIBRARY BZIPTWO_LIBRARY ZLIB_LIBRARY ${EXTRA_LIBS})

ENDIF (APPLE)
