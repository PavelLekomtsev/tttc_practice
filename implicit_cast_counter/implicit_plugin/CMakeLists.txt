cmake_minimum_required(VERSION 3.13)
project(ImplicitCastCounter)

# Указываю пути к LLVM и Clang
set(LLVM_DIR "/home/pavel/Documents/Utils/tttc_practice/build_rel/lib/cmake/llvm")
set(LLVM_INCLUDE_DIRS "/home/pavel/Documents/Utils/tttc_practice/llvm/include/llvm")

set(Clang_DIR "/home/pavel/Documents/Utils/tttc_practice/build_rel/lib/cmake/clang")
set(CLANG_INCLUDE_DIRS "/home/pavel/Documents/Utils/tttc_practice/clang/include/clang")

# Ищу пакеты
find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

# Добавляюинклюд-директории
include_directories(
    ${LLVM_INCLUDE_DIRS}
    ${CLANG_INCLUDE_DIRS}
)

# Получение необходимых LLVM компонентов
llvm_map_components_to_libnames(LLVM_LIBS
    Core
    Support
    Demangle
)

# Список необходимых библиотек Clang
set(CLANG_LIBS
    clangAST
    clangBasic
    clangFrontend
    clangTooling
    clangSerialization
)

include(AddLLVM)

add_llvm_library(ImplicitCastCounter MODULE 
    ImplicitCastCounter.cpp 
    PLUGIN_TOOL clang)