#include <stdio.h>
#include <math.h>

// Try this with -mfpmath=387

//#include "preprocessor.h"

double test(double a, double b) {
 double c;

 c = 1.0 + sin (a + b*b);

 return c;
}

int main() {
	printf("sin %g\n", test(56.9,32.1));
}
