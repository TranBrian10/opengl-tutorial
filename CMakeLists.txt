
cmake_minimum_required(VERSION 2.8.9)
project(opengl-tutorial)

#Bring the headers
include_directories(${CMAKE_SOURCE_DIR}/dependencies/include)

IF(APPLE)
  #Add GLFW static library
  add_library(GLFW_LIBRARY STATIC IMPORTED) # or SHARED instead of STATIC
  set_target_properties(GLFW_LIBRARY PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/dependencies/lib/macos/libglfw3.a"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/dependencies/include/glfw")

   #SET(GUI_TYPE MACOSX_BUNDLE)
   FIND_LIBRARY(IOKIT_LIBRARY IOKit)
   FIND_LIBRARY(COCOA_LIBRARY Cocoa)
   FIND_LIBRARY(CORE_VIDEO_LIBRARY CoreVideo)
   MARK_AS_ADVANCED (IOKIT_LIBRARY
                     COCOA_LIBRARY
                     CORE_VIDEO_LIBRARY)
   SET(EXTRA_LIBS ${IOKIT_LIBRARY} ${COCOA_LIBRARY}
    ${CORE_VIDEO_LIBRARY})
ENDIF (APPLE)

#the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "src/*.c*")

add_executable(testProj ${SOURCES})
target_link_libraries(testProj GLFW_LIBRARY ${EXTRA_LIBS})
