//
// Created by akhin on 6/7/16.
//

extern void  __VERIFIER_assert(int cond);
extern void  __VERIFIER_assume(int cond);
extern int   __VERIFIER_nondet_int();
extern void* __VERIFIER_nondet_pointer();
extern void  __VERIFIER_error();

int main() {
    int i = __VERIFIER_nondet_int();
    int j = __VERIFIER_nondet_int();
    char* p = __VERIFIER_nondet_pointer();

    __VERIFIER_assume(i > 0);
    __VERIFIER_assume(p);

    if (i <= 0) {
        __VERIFIER_error();
    }

    if (j <= 0) {
        __VERIFIER_error();
    }

    if (!p){
        __VERIFIER_error();
    }
}
