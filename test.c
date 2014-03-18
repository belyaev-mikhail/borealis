#include <CUnit/Basic.h>

int foo(int, char);
int bar(int);

void testFoo_0(void) {
    int a = 1;
    char b = 3;
    int res = foo(a, b);
}

void testFoo_1(void) {
    int a = 0;
    char b = 0;
    int res = foo(a, b);
}

void testBar_0(void) {
    int a = 1;
    int res = bar(a);
}

int main() {
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();
    CU_pSuite pSuiteFOO = CU_add_suite("Suite for foo", NULL, NULL);
    if (pSuiteFOO == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (CU_add_test(pSuiteFOO, "Test #0 for foo", testFoo_0) == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (CU_add_test(pSuiteFOO, "Test #1 for foo", testFoo_1) == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    CU_pSuite pSuiteBAR = CU_add_suite("Suite for bar", NULL, NULL);
    if (pSuiteBAR == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (CU_add_test(pSuiteBAR, "Test #0 for bar", testBar_0) == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
