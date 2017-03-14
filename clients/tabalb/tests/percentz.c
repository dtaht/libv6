#include <stdio.h>
#include <stdlib.h>

// Will this always compile?

int main() {
	size_t test = 0;
	printf("I can always print a size_t with %zd\n",test);
	printf("I can't always print a size_t with %d",test);
	exit(0);
}
