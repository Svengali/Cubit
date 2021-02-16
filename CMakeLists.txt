cmake_minimum_required (VERSION 3.6)

message("*** Cubit BEGIN")


include(${CMAKE_TOOLS_DIR}/Macros.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#add_subdirectory(../cblib)

if (MSVC)
    # For precompiled header.
    # Set 
    # "Precompiled Header" to "Use (/Yu)"
    # "Precompiled Header File" to "stdafx.h"
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yustdafx.h /FIstdafx.h")
endif()


file(
  GLOB_RECURSE CUBIT_HEADERS 
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  src/*.h src/*.hpp
)

file(
    GLOB_RECURSE CUBIT_SRC
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    src/*.cpp
    ../../DiligentTools/ThirdParty/imgui/imgui_demo.cpp
)

GenerateSourceGroups(src)


message("CUBIT_HEADERS: ${CUBIT_HEADERS}")
message("CUBIT_SRC: ${CUBIT_SRC}")


set_source_files_properties(src/stdafx.cpp
    PROPERTIES
    COMPILE_FLAGS "/Ycstdafx.h"
)


set(SHADERS
    src/assets/shaders/BasicStructures.fxh
)


set(ASSETS
)

set(SHADERS ${PIXEL_SHADERS} ${VERTEX_SHADERS})

set(COMPILED_SHADERS_DIR ${CMAKE_CURRENT_BINARY_DIR}/CompiledShaders)

file(MAKE_DIRECTORY "${COMPILED_SHADERS_DIR}")

set_source_files_properties(${SHADERS} PROPERTIES
    VS_SHADER_ENTRYPOINT "%(Filename)"
    VS_SHADER_MODEL 5.0
    VS_SHADER_VARIABLE_NAME "g_%(Filename)"
    VS_SHADER_OUTPUT_HEADER_FILE "${COMPILED_SHADERS_DIR}/%(Filename).h"
)
set_source_files_properties(${PIXEL_SHADERS} PROPERTIES VS_SHADER_TYPE Pixel)
set_source_files_properties(${VERTEX_SHADERS} PROPERTIES VS_SHADER_TYPE Vertex)
set_source_files_properties(src/cubit_ps.hlsl PROPERTIES VS_SHADER_MODEL 5.1)



if(WIN32)
    add_executable(Cubit WIN32 
        ${CUBIT_HEADERS} 
        ${CUBIT_SRC}
        ${IMGSRC}
    )
    set_target_properties(Cubit PROPERTIES 
        LINK_FLAGS "/SUBSYSTEM:WINDOWS"
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
    copy_required_dlls(Cubit)

else()
    message(FATAL_ERROR "Unsupported platform")
endif()

target_include_directories(Cubit
PRIVATE
    Src
    SDK/Include
    ${CMAKE_CURRENT_BINARY_DIR}/CompiledShaders
    ../../DiligentTools/ThirdParty/imgui/
    ../../DiligentTools/Imgui/interface
)

get_supported_backends(ENGINE_LIBRARIES)

target_include_directories(Cubit PUBLIC ../../third_party/cblib/src)
target_include_directories(Cubit PUBLIC ../../third_party/Pham/src)



target_link_libraries(Cubit
PRIVATE 
    Diligent-BuildSettings
PUBLIC
    cblib
    Pham
    Diligent-Common
    Diligent-GraphicsTools
    Diligent-TextureLoader
    Diligent-TargetPlatform
    Diligent-Imgui
    ${ENGINE_LIBRARIES}
    Diligent-NativeAppBase
    d3d11.lib
    d3d12.lib
    ninput.lib
    winmm.lib
    dxgi.lib
    shcore.lib
)

set_common_target_properties(Cubit)

if(MSVC)
    target_compile_definitions(Cubit PRIVATE NOMINMAX)
    # Disable MSVC-specific warnings
    # - w4201: nonstandard extension used: nameless struct/union
    # - w4324: structure was padded due to alignment specifier
    # - w4238: nonstandard extension used: class rvalue used as lvalue
    target_compile_options(Asteroids PRIVATE /wd4201 /wd4324 /wd4238)
endif()


set_source_files_properties(${SHADERS} PROPERTIES
    VS_DEPLOYMENT_LOCATION "shaders"
)

source_group("src\\assets\\shaders" FILES ${SHADERS})



#source_group("src" FILES ${CUBIT_SRC})
#source_group("include" FILES ${CUBIT_HEADERS})
#source_group("shaders" FILES 
#    ${SHADERS}
#    src/common_defines.h
#    src/shader_common.h
#)
#source_group("SDK" FILES SDK/Include/d3dx12.h)
#source_group("GUI" FILES ${GUI})
#source_group("media" FILES ${MEDIA})

message("*** Cubit END")
