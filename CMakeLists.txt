cmake_minimum_required(VERSION 3.24)

project(NakedNPCFix VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find and include CommonLibSSE
find_package(CommonLibSSE REQUIRED)

add_library(${PROJECT_NAME} SHARED)

target_sources(${PROJECT_NAME} 
    PRIVATE 
        src/main.cpp
        src/VisibilityFixer.cpp
        src/Settings.h
        src/PCH.h
        exports.def
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        CommonLibSSE::CommonLibSSE
)

# SKSE Plugin information
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        SKSE_SUPPORT_SSE
        SKSE_SUPPORT_AE
        SKSE_SUPPORT_VR
        PROJECT_NAME="${PROJECT_NAME}"
)

# Use static runtime to avoid missing DLL errors on user machines
set_target_properties(${PROJECT_NAME} PROPERTIES 
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

# PCH support
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /MP /Zm200)
endif()
