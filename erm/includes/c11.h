/**
 * c11.h
 *
 * Dave Taht
 * 2017-03-11
 */

#ifndef C11_H
#define C11_H
// #include <tgmath.h>

// http://blog.smartbear.com/codereviewer/c11-a-new-c-standard-aiming-at-safer-programming/
// http://stackoverflow.com/questions/17302913/generics-for-multiparameter-c-functions-in-c11
/* Get the name of a type */
#define typename(x) _Generic((                                               \
x), _Bool : "_Bool", char : "char", signed char : "signed char",             \
unsigned char : "unsigned char", short int : "short int",                    \
unsigned short int : "unsigned short int", int : "int",                      \
unsigned int : "unsigned int", long int : "long int",                        \
unsigned long int : "unsigned long int", long long int : "long long int",    \
unsigned long long int : "unsigned long long int", float : "float",          \
double : "double", long double : "long double", char* : "pointer to char",   \
void* : "pointer to void", int* : "pointer to int", default : "other")

#define is_compatible(x, T) _Generic((x), T : 1, default : 0)

//    is_compatible(x, typeof(y));

#define printf_dec_format(x)                                                 \
  _Generic((x), \
    char: "%c", \
    signed char: "%hhd", \
    unsigned char: "%hhu", \
    signed short: "%hd", \
    unsigned short: "%hu", \
    signed int: "%d", \
    unsigned int: "%u", \
    long int: "%ld", \
    unsigned long int: "%lu", \
    long long int: "%lld", \
    unsigned long long int: "%llu", \
    float: "%f", \
    double: "%f", \
    long double: "%Lf", \
    char *: "%s", \
    void *: "%p")

#define print(x) printf(printf_dec_format(x), x)
#define printnl(x) printf(printf_dec_format(x), x), printf("\n");

#define type_idx(T) _Generic((T), char : 1, int : 2, long : 3, default : 0)


#endif
