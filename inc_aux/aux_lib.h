#include <string.h>

int existe_caractere(const char *str, char c) {
    return strchr(str, c) != NULL;
}