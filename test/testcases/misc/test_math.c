#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "test_math.h"

// @requires (a > 0) && (a < 100) && (b > 0) && (b < 100) && (m > 1) && (m < 100)
// @ensures \result == (a * b) % m
long mul(long a, long b, long m){
    if (b == 1) {
        return a;
    }
    if (b % 2 == 0) {
        long t = mul(a, b / 2, m);
        return (2 * t) % m;
    }
    return (mul(a, b-1, m) + a) % m;
}

// @requires (a > 0) && (a < 100) && (b > 0) && (b < 100) && (m > 1) && (m < 100)
// @ensures \result > 0
long pows(long a, long b, long m){
    if (b == 0) {
        return 1;
    }
    if (b % 2 == 0) {
        long t = pows(a, b / 2, m);
        return mul(t , t, m) % m;
    }
    return (mul(pows(a, b - 1, m) , a, m)) % m;
}

// @requires (a > 0) && (a < 100) && (b >= 0) && (b < 100)
// @ensures ((a % \result) == 0) && ((b % \result) == 0)
long gcd(long a, long b){
    if(b == 0) {
        return a;
    }
    return gcd(b, a % b);
}

// @requires (n > 0) && (n < 1000000)
// @ensures (\result == true) || (\result == false)
int prime(long n){ 
    for(long i = 2; i <= sqrt(n); i++) {
        if(n % i == 0) {
            return 0;
        }
    }
    return 1;
}


// @requires (x > 0) && (x < 1000000)
// @ensures (\result == true) || (\result == false)
int ferma(long x){
    if(x == 2) {
        return 1;
    }
    srand(time(NULL));
    for(int i = 0; i < 100; i++) {
        long a = (rand() % (x - 2)) + 2;
        if (gcd(a, x) != 1)
            return 0;           
        if (pows(a, x-1, x) != 1)       
            return 0;           
    }
    return 1;
}