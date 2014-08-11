#include "test_struct.h"

int sumStruct(struct Struct str) {
    if (str.a < 222)
    return str.a + str.b;
    else if (str.a == 222 && str.b < 64)
    return str.a * str.b;
    else if (str.b == 64)
    return str.a - str.b;
    else
    return str.a;
}

// @requires str.a[7] >= 221
// @ensures \result == 562 || \result == -666 || \result == 300 || \result == -300
int sumStructArray(struct ArrayStruct str) {
    if (str.a[7] < 222)
    return 562;
    else if (str.a[3] == 222 && str.b[2] < 64)
    return -666;
    else if (str.b[4] == 64)
    return 300;
    else
    return -300;
}

// @ensures \result.a[3] == 28
struct ArrayStruct retStruct(int a, int b, int c) {
    struct ArrayStruct rv;
    rv.a[3] = 28;
    rv.b[1] = 300;
    rv.c = -8;
    return rv;
}

// @requires str != F
// @ensures \result != F
enum Enum enumFunc(enum Enum str) {
    if (str == F) return S;
    else if (str == S) return X;
    else return S;
}

enum Enum compFunc(struct Comp f) {
    if (f.en[2] == F) return F;
    else if (f.x[3] == 3) return S;
    else if (f.x[3] == 4) return X;
    else if (f.st[3].a == 10) return F;
    else if (f.st[3].a == 12) return S;
    else if (f.st[3].a == 13) return X;
    else return S;
}

struct Matrix add(struct Matrix m1, struct Matrix m2) {
    struct Matrix res;
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            res.matrix[i][j] = m1.matrix[i][j] + m2.matrix[i][j];
        }
    }
    return res;
}


int internal_mult(struct Vector v) {
    int res = 0;
    for (int i = 0; i < SIZE; ++i) {
        res *= v.vect[i];
    }
    return res;
}