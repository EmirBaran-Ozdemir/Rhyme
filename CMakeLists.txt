cmake_minimum_required (VERSION 3.22)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

project ("RhymeProject")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

file(GLOB COMPILER_SOURCE_FILES "Rhyme/src/Compiler/*.cpp")
file(GLOB COMPILER_HEADER_FILES "Rhyme/src/Compiler/*.h" "Rhyme/src/Utils/*.h")
file(GLOB UTILS_SOURCE_FILES "Rhyme/src/Utils/*.cpp")
file(GLOB UTILS_HEADER_FILES "Rhyme/src/Utils/*.h")
file(GLOB CORE_SOURCE_FILES "Rhyme/src/Core/*.cpp")
file(GLOB CORE_HEADER_FILES "Rhyme/src/Core/*.h")
add_subdirectory("vendor/fmt")

add_executable(Rhyme
    "Rhyme/src/Rhyme.cpp"
    ${COMPILER_SOURCE_FILES} 
    ${COMPILER_HEADER_FILES} 
    ${UTILS_SOURCE_FILES}
    ${UTILS_HEADER_FILES}
    ${CORE_SOURCE_FILES}
    ${CORE_HEADER_FILES}
    "Rhyme/src/Core/MemoryPool.cpp")


set(CMAKE_PCH_INSTANTIATE_TEMPLATES ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(RHY_ENABLE_DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(RHY_ENABLE_RELEASE)
endif()

target_precompile_headers(Rhyme PUBLIC "Rhyme/src/rhypch.h")

target_include_directories(Rhyme PUBLIC "Rhyme/src")
target_include_directories(Rhyme PUBLIC "vendor/fmt/include")

target_link_libraries(Rhyme PRIVATE fmt::fmt fmt-header-only)

set_target_properties(Rhyme PROPERTIES OUTPUT_NAME "Rhyme")
