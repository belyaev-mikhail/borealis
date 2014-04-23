#include <stdio.h>

float foo(double a, float b) {
    float c = 0.0;
    if ((a == 0.3) && (b == 3.9)) {
        c = a + b;
        if ((int)b == a) {
            c = a - b;
        }
    }
    return c;
}

int bar(double d, unsigned i) {
    return i > d;
}

int main() {
    float a = 1.0;
    float b = 2.0;
    return 3.0 == foo(a, b);
}
