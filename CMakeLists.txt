cmake_minimum_required (VERSION 3.22)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

project ("Rhyme")

file(GLOB COMPILER_SOURCE_FILES "Rhyme/src/Compiler/*.cpp")

file(GLOB COMPILER_HEADER_FILES "Rhyme/src/Compiler/*.h" "Rhyme/src/Utils/*.h")

add_subdirectory("vendor/fmt")

add_executable(Rhyme
    "Rhyme/src/Rhyme.cpp"
    ${COMPILER_SOURCE_FILES} 
    ${COMPILER_HEADER_FILES} 
    "Rhyme/src/MemoryPool.cpp")

set(CMAKE_PCH_INSTANTIATE_TEMPLATES ON)

target_precompile_headers(Rhyme PUBLIC "Rhyme/src/rhypch.h")

target_include_directories(Rhyme PUBLIC "Rhyme/src")
target_include_directories(Rhyme PUBLIC "vendor/fmt/include")

set_target_properties(Rhyme PROPERTIES OUTPUT_NAME "Rhyme")
