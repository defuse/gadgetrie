#include <stdio.h>
#include <string.h>
#include <math.h>

void vulnerable(const char *attack)
{
    char buf[100];
    strcpy(buf, attack);
}

int main(int argc, char **argv) 
{
    double x = tan(1.0);
    printf("%f\n", x);
    if (argc > 1)
        vulnerable(argv[1]);
}
