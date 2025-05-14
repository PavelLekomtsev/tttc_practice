# Implicit Cast Counter Plugin
A Clang-based plugin that analyzes and counts implicit type conversions in C++ code.

## Build Instructions
1. Configure the project with CMake:

```bash
cmake -S implicit_plugin -B build -G Ninja
```
2. Build the plugin:

```bash
ninja -C build
```

## Usage
To analyze a source file, run:

```bash
/path/to/clang -fplugin=build/ImplicitCastCounter.so -c /path/to/source.cpp
```
Example (using the provided test files):

```bash
/home/pavel/Documents/Utils/tttc_practice/build_rel/bin/clang -fplugin=build/ImplicitCastCounter.so -c ./counter_tests/test1.cpp
```

## Example Outputs
**Test 1 Output (test1.cpp):**
```bash
Function `mul` float -> int: 1 double (int, float) -> double (*)(int, float): 1 float -> double: 1 double -> int: 1
Function `sum` int -> float: 1 float -> double: 1
```
**Test 2 Output (test2.cpp):**
```bash
Function `testMultipleConversions` float -> double: 1 int -> double: 1 double -> long: 1
Function `returnsChar` char -> short: 1
Function `outerFuncWithMultiConversions` float -> int: 2 int -> short: 2 float (short) -> float (*)(short): 2
Function `innerFuncWithMultiConversions` short -> int: 2 int -> float: 2
```
