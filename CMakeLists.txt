cmake_minimum_required(VERSION 3.15)

project(BasicServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(app src/main.cpp src/server.cpp src/clientSession.cpp src/uuid.cpp)

# Platform-specific sources
if(WIN32)
    target_sources(app PRIVATE
        src/platform/windows/socket.cpp src/platform/windows/eventLoop.cpp
    )

elseif(LINUX)
    target_sources(app PRIVATE
        src/platform/linux/socket.cpp src/platform/linux/eventLoop.cpp
    )
endif()

# Platform-specific linker libs
if(WIN32)
    target_link_libraries(app ws2_32)
endif()

# Include headers
target_include_directories(app PRIVATE include)