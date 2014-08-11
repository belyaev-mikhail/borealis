#ifndef _TEST_STRUCT_
#define _TEST_STRUCT_

struct Struct {
    int a;
    int b;
};

struct ArrayStruct {
    int a[8];
    int b[8];
    int c;
};

enum Enum {
    S,
    F,
    L,
    X
};

struct Comp {
    enum Enum en[8];
    int x[8];
    int y;
    struct Struct st[8];
};

#define SIZE 2

struct Matrix{
    int matrix[SIZE][SIZE];
};

struct Vector {
    int vect[SIZE];
};

int sumStruct(struct Struct str);
int sumStructArray(struct ArrayStruct str);
struct ArrayStruct retStruct(int a, int b, int c);
enum Enum enumFunc(enum Enum str);
enum Enum compFunc(struct Comp f);
struct Matrix add(struct Matrix m1, struct Matrix m2);
int internal_mult(struct Vector v);

#endif