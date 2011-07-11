project(kinectarray)

cmake_minimum_required(VERSION 2.4.0)

# needed for VTK library
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# enable debugging, and show all warnings
set(CMAKE_CXX_FLAGS "-g -Wall")

# find and setup Qt4
# see http://cmake.org/cmake/help/cmake2.6docs.html#module:FindQt4 for details
find_package(Qt4 REQUIRED) 
set(QT_USE_QTOPENGL TRUE)
include(${QT_USE_FILE})
set(LIBS ${LIBS} ${QT_LIBRARIES})

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
set(LIBS ${LIBS} ${GLUT_LIBRARIES})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
set(LIBS ${LIBS} QVTK vtkCharts)

# OpenNI
include_directories("/usr/include/ni")
set(LIBS ${LIBS} OpenNI)

set(SRCS
    src/main.cpp
    src/MainWindow.cpp
    src/worf/coordinate_transform.cpp
    src/worf/log.cpp
    src/worf/WorfCamera.cpp
    src/worf/WorfQtGLWindow.cpp
    src/worf/WorfQtMainWindow.cpp
)

set(MOC_HEADERS
    src/MainWindow.h
)

qt4_wrap_cpp(MOC_OUTFILES ${MOC_HEADERS})

add_executable(kinectarray ${SRCS} ${MOC_OUTFILES})

target_link_libraries(kinectarray ${LIBS})
