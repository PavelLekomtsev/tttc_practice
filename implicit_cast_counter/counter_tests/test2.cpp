// Тест 1: Вложенные вызовы функций
float innerFuncWithMultiConversions(short s) {
    // short->float (два раза)
    float a = s * 1.5f;       // short->float
    float b = s / 2.0f;       // short->float
    return a + b;
}

int outerFuncWithMultiConversions(int a) {
    // Два вызова с int->short
    float temp1 = innerFuncWithMultiConversions(a);    // int->short
    float temp2 = innerFuncWithMultiConversions(a+1); // int->short
    
    // Два float->int
    int res1 = temp1;          // float->int
    int res2 = temp2;          // float->int
    return res1 + res2;
}

// Тест 2: Преобразования в возвращаемых значениях
short returnsChar() {
    char c = 'Z';
    return c;               // char->short
}

// Тест 3: Множественные преобразования в одном выражении
void testMultipleConversions() {
    int a = 10;
    float b = 20.5f;
    double c = 30.7;
    long d = a + b * c;     // float->double, double->long
}