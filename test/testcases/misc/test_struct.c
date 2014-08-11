#define SIZE 2

struct Matrix{
    int matrix[SIZE][SIZE];
};

struct Matrix add(struct Matrix m1, struct Matrix m2) {
    struct Matrix res;
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            res.matrix[i][j] = m1.matrix[i][j] + m2.matrix[i][j];
        }
    }
    return res;
}

struct Vector {
    int vect[SIZE];
    int size;
};

// @requires v.size = 2
int internal_mult(struct Vector v) {
    int res = 0;
    for (int i = 0; i < SIZE; ++i) {
        res *= v.vect[i];
    }
    return res;
}