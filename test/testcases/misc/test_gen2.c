
int foo(int a, int b) {
	if (a % 2) return a;
	if (b > 23 && b <= 32) return b;

	int x = a * b;
	if(x < 0) return x;
	if(x/b < 42) return 42;
	return x + 1;
}

int main() {
    int res = foo(1, 124);
    return 0;
}