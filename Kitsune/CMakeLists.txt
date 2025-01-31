set(PROJECT_NAME Kitsune)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "Src/Core/KitApplication.cpp"
    "Src/Core/KitApplication.h"
    "Src/Core/KitDefinitions.h"
    "Src/Core/KitInputController.cpp"
    "Src/Core/KitInputController.h"
    "Src/Core/KitLogs.h"
    "Src/Core/Scene/KitGameObject.cpp"
    "Src/Core/Scene/KitGameObject.h"
    "Src/Graphics/KitCamera.cpp"
    "Src/Graphics/KitCamera.h"
    "Src/Graphics/KitEngineDevice.cpp"
    "Src/Graphics/KitEngineDevice.h"
    "Src/Graphics/KitModel.cpp"
    "Src/Graphics/KitModel.h"
    "Src/Graphics/KitPipeline.cpp"
    "Src/Graphics/KitPipeline.h"
    "Src/Graphics/KitRenderer.cpp"
    "Src/Graphics/KitRenderer.h"
    "Src/Graphics/KitSwapChain.cpp"
    "Src/Graphics/KitSwapChain.h"
    "Src/Graphics/KitWindow.cpp"
    "Src/Graphics/KitWindow.h"
    "Src/Graphics/RenderSystems/KitBasicRenderSystem.cpp"
    "Src/Graphics/RenderSystems/KitBasicRenderSystem.h"
)
source_group("" FILES ${no_group_source_files})

set(Source_Files
    "Kitsune.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${no_group_source_files}
    ${Source_Files}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE Kitsune)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# Include directories
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "$ENV{VULKAN_SDK}/Include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Libraries/glfw/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Libraries/glm;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Libraries/spdlog/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/Src"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "$ENV{VULKAN_SDK}/Include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Libraries/glfw/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Libraries/glm;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Libraries/spdlog/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/Src"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od
            >
            $<$<CONFIG:Release>:
                /O2;
                /Oi;
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /Zc:__cplusplus;
            /utf-8;
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Y-
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od
            >
            $<$<CONFIG:Release>:
                /O2;
                /Oi;
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /Zc:__cplusplus;
            /utf-8;
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Y-
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE
        )
    endif()
endif()

################################################################################
# Dependencies
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "vulkan-1;"
        "glfw3"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "vulkan-1;"
        "glfw3"
    )
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_link_directories(${PROJECT_NAME} PRIVATE
        "$ENV{VULKAN_SDK}/Lib/;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Libraries/glfw/build/src/Release/"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_link_directories(${PROJECT_NAME} PRIVATE
        "$ENV{VULKAN_SDK}/Lib/;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Libraries/glfw/build/src/Release/"
    )
endif()

