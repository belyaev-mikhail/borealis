#include <stdio.h>

int main(int argc, char** argv) {
	int arr[30000];
	int i = 0;
	
	for (;;) {
		if(i <= 30000) arr[i] = 0;
		i+=2;
	}
}
