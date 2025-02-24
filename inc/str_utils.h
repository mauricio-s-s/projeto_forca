#ifndef STR_UTILS_H
#define STR_UTILS_H


int existe_caractere(const char *str, char c);
int posicoes_caractere(const char *word, char c, int posicoes[]);
void substituir_caracteres(char *str, int posicoes[], int total, char novo_caractere);
int comparar_strings(const char *str1, const char *str2);


#endif 