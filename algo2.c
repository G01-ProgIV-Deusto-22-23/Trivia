#include <stdio.h>

#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x)        STRINGIFY_HELPER (x)

#define a 65

int main (void) {
    printf ("a: %s", STRINGIFY (a));
}