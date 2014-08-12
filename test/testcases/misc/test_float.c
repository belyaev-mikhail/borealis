#include <stdio.h>
#include <math.h>

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

// @ensures (\result <= 1) && (\result >= -1)
float sine(float x) {
    float res = 0, pow = x, fact = 1;
    int ii;
    if (x == M_PI) {
        return 0.0;
    } else if (x == M_PI / 2) {
        return 1.0;
    }
    for(ii = 0; ii < 100; ++ii) {
        float i = (float) ii;
        res+=pow / fact;
        pow *= x * x;
        fact *= (2 * (i + 1)) * (2 * (i + 1) + 1);
    }
    return res;
}
