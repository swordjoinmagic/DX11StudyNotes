cmake_minimum_required(VERSION 3.15)

# set the project name
project(DX11Lab)

SET(HEADER_FILES 
    D3dFrameWork/d3dApp.h
    D3dFrameWork/Input.h
    D3dFrameWork/Light.h
    D3dFrameWork/Mesh.h
    D3dFrameWork/Light.h
    D3dFrameWork/Shader.h
    D3dFrameWork/Texture2D.h
    D3dFrameWork/CBVaribleConfig.h

    DirectXTex/DDSTextureLoader.h
    DirectXTex/WICTextureLoader.h

    Utils/D3DUtils.h
    Utils/DxTrace.h
    Utils/GameTimer.h
    Utils/MathF.h
    Utils/MeshLoader.h
    Utils/ObjLoader.h
    Utils/RenderStates.h
    Utils/DefaultVertex.h
    )

SET(SOURCE_FILES 
    D3dFrameWork/d3dApp.cpp
    D3dFrameWork/Input.cpp
    D3dFrameWork/Mesh.cpp
    D3dFrameWork/Shader.cpp
    D3dFrameWork/Texture2D.cpp
    D3dFrameWork/CBVaribleConfig.cpp

    DirectXTex/DDSTextureLoader.cpp
    DirectXTex/WICTextureLoader.cpp

    Utils/D3DUtils.cpp
    Utils/DxTrace.cpp
    Utils/GameTimer.cpp
    Utils/MathF.cpp
    Utils/MeshLoader.cpp
    Utils/ObjLoader.cpp
    Utils/RenderStates.cpp

    Source/main.cpp
    )


source_group("D3dFrameWork" FILES
    D3dFrameWork/d3dApp.h
    D3dFrameWork/Input.h
    D3dFrameWork/Light.h
    D3dFrameWork/Mesh.h
    D3dFrameWork/Light.h
    D3dFrameWork/Shader.h
    D3dFrameWork/Texture2D.h
    D3dFrameWork/CBVaribleConfig.h
    D3dFrameWork/d3dApp.cpp
    D3dFrameWork/Input.cpp
    D3dFrameWork/Mesh.cpp
    D3dFrameWork/Shader.cpp
    D3dFrameWork/Texture2D.cpp
    D3dFrameWork/CBVaribleConfig.cpp
)

source_group("DirectXTex" FILES
    DirectXTex/DDSTextureLoader.h
    DirectXTex/WICTextureLoader.h
    DirectXTex/DDSTextureLoader.cpp
    DirectXTex/WICTextureLoader.cpp
)

source_group("Utils" FILES
    Utils/D3DUtils.h
    Utils/DxTrace.h
    Utils/GameTimer.h
    Utils/MathF.h
    Utils/MeshLoader.h
    Utils/ObjLoader.h
    Utils/RenderStates.h
    Utils/D3DUtils.cpp
    Utils/DxTrace.cpp
    Utils/GameTimer.cpp
    Utils/MathF.cpp
    Utils/MeshLoader.cpp
    Utils/ObjLoader.cpp
    Utils/RenderStates.cpp
    Utils/DefaultVertex.h
)

# add assimp
# aux_source_directory(./assimp/ ASSIMP_FILES)
FILE(GLOB ASSIMP_SOURCE_FILES "./assimp/*.cpp" "./assimp/*.c")
FILE(GLOB ASSIMP_HEADER_FILES "./assimp/*.h" "./assimp/*.hpp")

source_group("assimp" FILES
    ${ASSIMP_SOURCE_FILES}
    ${ASSIMP_HEADER_FILES}
)

# add the executable
add_executable(DX11Lab ${HEADER_FILES} ${SOURCE_FILES} ${ASSIMP_SOURCE_FILES} ${ASSIMP_HEADER_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC
    d2d1.lib
    dwrite.lib
    assimpd.lib
    d3d11.lib
    dxgi.lib
    dxguid.lib
    D3DCompiler.lib
    winmm.lib
)

target_include_directories(${PROJECT_NAME} PUBLIC
                           ${PROJECT_BINARY_DIR}
                           ${PROJECT_SOURCE_DIR}/D3dFrameWork
                           ${PROJECT_SOURCE_DIR}/Utils
                           ${PROJECT_SOURCE_DIR}/DirectXTex
                           ${PROJECT_SOURCE_DIR}
                           )