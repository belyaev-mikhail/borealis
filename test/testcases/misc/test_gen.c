#include <stdio.h>

int foo(int a, char b) {
    int c = 0;
    if ((a == 1) && (b == 3)) {
        c = 1;
        if (b == 2) {
            c = 2;
        }
    } else {
        c = 3;
    }
    return c;
}

int bar(int a) {
    return ++a;
}

int main() {
    int res = foo(1, 2);
    return bar(res);
}
