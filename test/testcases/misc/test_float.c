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

int bar(double d, unsigned i) {
    if (d > 2.8)
        return i > d;
    else 
        return 0;
}

double cosinus(double x) {
    double t , s ;
    int p;
    p = 0;
    s = 1.0;
    t = 1.0;
    while(fabs(t/s) > 0.001) {
        p++;
        t = (-t * x * x) / ((2 * p - 1) * (2 * p));
        s += t;
    }
    return s;
}

// @ensures (\result <= 1) && (\result >= -1)
float sine(float x) {
    float res = 0, pow = x, fact = 1;
    int ii;
    if (x == M_PI) {
        return 0;
    } else if (x == M_PI / 2) {
        return 1;
    }
    for(ii = 0; ii < 100; ++ii) {
        float i = (float) ii;
        res+=pow / fact;
        pow *= x * x;
        fact *= (2 * (i + 1)) * (2 * (i + 1) + 1);
    }
    return res;
}


float smth(float a) {
    for (int i = 0; i < 100; ++i) {
        a = a + 2.0;
        float ii = (float)i;
    }
    return a;
}
