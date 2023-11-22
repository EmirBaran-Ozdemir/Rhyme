
add_subdirectory("../vendor/fmt" FMT)
target_include_directories(Rhyme PUBLIC "../vendor/fmt/include")

add_executable (Rhyme 
    "src/Rhyme.cpp" 
    "src/MemoryPool.cpp" 
    "src/Compiler/Tokenizer.cpp" 
    "src/Compiler/Parser.cpp" 
    "src/Compiler/Generator.cpp"
    "src/Utils/ExceptionHandler.cpp")

target_link_libraries(Rhyme PRIVATE fmt::fmt fmt-header-only) 

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Rhyme PROPERTY CXX_STANDARD 20)
endif()