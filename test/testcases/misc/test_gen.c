#include <stdio.h>

int foo(int a, char b) {
    int c = 0;
    if ((a == 1) && (b == 3)) {
        c = 1;
        if (b == 2) {
            c = 2;
        }
    }
    return c;
}

// @requires a > 0
// @ensures \result > 0
// @ensures \result == (a + 1)
int bar(int a) {
    return ++a;
}

/*
    void testBAR(){
        int a = 1;
        int res = bar(a);
        CU_ASSERT(res > 0);
        CU_ASSERT(res == (a + 1));
    }
*/

// @requires arg > 0
// @requires arg < 15
// @ensures \result > 0
long factoriall(long arg) {
    int i; // This causes a bug in test generation
    long res = 1;
    for (i = arg; i > 0; i--) {
        res *= i;
    }
    return res;
}