cmake_minimum_required  (VERSION 3.8...4.0.2)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project (valve_master_guard CXX)

#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# download CPM.cmake
file(
    DOWNLOAD
    https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.42.0/CPM.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
    EXPECTED_HASH SHA256=2020b4fc42dba44817983e06342e682ecfc3d2f484a581f11cc5731fbe4dce8a
)

include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

#CPMAddPackage("gh:g-truc/glm#1.0.1")

add_library(valve_master_guard SHARED
    "src/main.cpp"
)

set_property(TARGET valve_master_guard PROPERTY CXX_STANDARD 23)

target_include_directories(valve_master_guard PUBLIC
                            valve_master_guard/src
                            )

target_compile_features(valve_master_guard PUBLIC cxx_std_23)

add_compile_options(
    -Xclang -fno-sjlj-exceptions 
    -Xclang -fno-seh-exceptions

    -fno-exceptions 
    -fno-asynchronous-unwind-tables 
    -fno-unwind-tables 
    -fno-threadsafe-statics
    -fno-sanitize=undefined

    -finline-functions 
    -finline-hint-functions
    -fno-rtti
    -fno-ident
    -march=haswell
    -mtune=generic
    -mfpmath=sse
    
    -ffunction-sections
    -fdata-sections

    /MP
)

target_link_options(valve_master_guard PRIVATE 
                    /SAFESEH:NO
                    /MERGE:.tls=.data
                    /MERGE:_RDATA=.rdata
                    /MERGE:.gxfg=.rdata
                    )