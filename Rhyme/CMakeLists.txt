# Add source to this project's executable.
add_executable (Rhyme "src/Rhyme.cpp" "src/Rhyme.h" "src/MemoryPool.h" "src/MemoryPool.cpp" "src/Compiler/Tokenizer.cpp" "src/Compiler/Tokenizer.h" "src/Compiler/Parser.h" "src/Compiler/Parser.cpp" "src/Compiler/Generator.cpp" "src/Compiler/Generator.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Rhyme PROPERTY CXX_STANDARD 20)
endif()

