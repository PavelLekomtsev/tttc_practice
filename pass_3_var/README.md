# MyLoopPass - LLVM пасс для инструментации циклов
## Компиляция
```bash
cmake -S src -B build
cmake --build build --config Release
```

## Запуск
1. Компилируем тестовый файл в LLVM IR:
```bash
cd test
clang++ -S -emit-llvm test_1.cpp -o test_1.ll
```

2. Применяем пасс: 
opt -load ../build/LoopPAss.so -my_loop_pass test_1.ll -o test_1_instrumented.ll

3. Смотрим результат
```bash
grep -A2 -B2 "loop_start\|loop_end" test_1_instrumented.ll
```

## Результат
В файле test_1_instrumented.ll появились вызовы:
- call void @loop_start() - в начале цикла
- call void @loop_end() - при выходе из цикла

Пример:
```llvm
call void @loop_start()
; Логика цикла
call void @loop_end()
```