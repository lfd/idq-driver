project(EasyQuantis)
cmake_minimum_required(VERSION 2.6.0)

# ########## Generates custom header definitions ##########
configure_file(
  "${PROJECT_SOURCE_DIR}/EasyQuantisConfig.h.in"
  "${PROJECT_BINARY_DIR}/EasyQuantisConfig.h"
)
# Include directory containing generated files
include_directories(${PROJECT_BINARY_DIR})


# Search boost
find_package(Boost REQUIRED COMPONENTS date_time filesystem program_options system thread)
include_directories(${Boost_INCLUDE_DIRS})
if("${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}" LESS 1.37)
  message(FATAL_ERROR "Requires Boost 1.37 or newer (found ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION})")
endif()

# rt library is required by boost-date_time (but not on Mac OSX), but it is not automatically added on every platform...
if(NOT CMAKE_SYSTEM_NAME MATCHES "Darwin")
  find_package(Rt REQUIRED)
  include_directories(${Rt_INCLUDE_DIRS})
endif()


# Base sources
set(EasyQuantis_SRCS
  BinaryFileWriter.cpp
  EasyQuantis.cpp
  EasyQuantisCmd.cpp
  Quantis2File.cpp
  Utils.cpp
)

# GUI sources
if(NOT DISABLE_EASYQUANTIS_GUI)
  # Search Qt4
  find_package(Qt4 REQUIRED)

  # the next line sets up include and link directories and defines some variables that we will use.
  # you can modify the behavior by setting some variables, e.g.
  #   set(QT_USE_OPENGL TRUE)
  # -> this will cause cmake to include and link against the OpenGL module
  include(${QT_USE_FILE})
  # see http://cmake.org/cmake/help/cmake2.6docs.html#module:FindQt4 for a complete list


  # Scientific plots
  #find_package ( Qwt5 REQUIRED )

  set(EasyQuantis_GUI_SRCS
    EasyQuantisGuiMain.cpp
    EasyQuantisGuiProgressDialog.cpp
    EasyQuantisGuiTextEditDialog.cpp
  )

  # Qt - MOC
  SET(EasyQuantis_MOC_HDRS
    EasyQuantisGuiMain.hpp
    EasyQuantisGuiProgressDialog.hpp
    EasyQuantisGuiTextEditDialog.hpp
  )

  QT4_WRAP_CPP(EasyQuantis_MOC_SRCS ${EasyQuantis_MOC_HDRS})

  # Qt - UI
  #set(EasyQuantis_UIS
  #  dialog.ui
  #)

  #QT4_WRAP_UI(EasyQuantis_UIS_H ${EasyQuantis_UIS})

  # Include output directory, otherwise the UI file won't be wrapped!
  #include_directories(${CMAKE_CURRENT_BINARY_DIR})

  # Qt - Resource files
  set(EasyQuantis_RCCS
    EasyQuantisGui.qrc
  )

  QT4_ADD_RESOURCES(EasyQuantis_RCCS_SRCS ${EasyQuantis_RCCS})
endif()

# Build executable

#Executable name
set (ExeName "EasyQuantis")
   
add_executable(${ExeName} 
  ${EasyQuantis_SRCS}
  ${EasyQuantis_GUI_SRCS}
  ${EasyQuantis_MOC_SRCS}
#  ${EasyQuantis_UIS_H}
  ${EasyQuantis_RCCS_SRCS}
)


target_link_libraries(${ExeName}
    Quantis-static
    Quantis_Extensions-static
    ${QT_LIBRARIES}
    ${Boost_LIBRARIES}
    ${Rt_LIBRARIES}
)
if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set_target_properties(${ExeName} PROPERTIES
         LINK_FLAGS "-framework IOKit -framework CoreFoundation -lSystem")
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  # Statically link to libstdc++
  # This allows to execute the application with older glibc

  # Get full path of libstdc++.a
  execute_process(
    COMMAND g++ -print-file-name=libstdc++.a
    RESULT_VARIABLE ret_var
    OUTPUT_VARIABLE libstdc_filename
  )

  # Strip newline character
  string(REGEX REPLACE "\n" "" libstdc_filename "${libstdc_filename}")

  # Create a symbolic link to libstdc++.a
  execute_process(
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMAND ln -sf "${libstdc_filename}" .
    RESULT_VARIABLE ret_var
    OUTPUT_VARIABLE output_var
  )

  # Tell the linker to link against the static libstdc++
  set_target_properties(${ExeName} PROPERTIES
    LINK_FLAGS "-static-libgcc -L."
  )
endif()

install(TARGETS ${ExeName} DESTINATION ${CMAKE_INSTALL_BIN_DIR})

