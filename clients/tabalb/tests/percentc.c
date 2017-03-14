#include <stdio.h>

int main() {
	size_t test;
	printf("I can always print a size_t with %zd",test);
	printf("I can't always print a size_t with %d",test);
// %zu, %zx also!
}
