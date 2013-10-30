#include "misc.h"

#include <stdint.h>
#include <ctype.h>
#include <string.h>

int has_zero_byte(uint32_t n)
{
    if ((n & 0xFF) == 0 || ((n >> 8) & 0xFF) == 0 || ((n >> 16) & 0xFF) == 0 || ((n >> 24) & 0xFF) == 0) {
        return 1;
    } else {
        return 0;
    }
}

void rstrip(char *text)
{
    int end;
    for (end = strlen(text) - 1; end >= 0 && isspace(text[end]); end--)
        ;
    text[end+1] = 0;
}

void tab_to_space(char *text)
{
    while (*text != 0) {
        if (*text == '\t')
            *text = ' ';
        text++;
    }
}
