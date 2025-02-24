#include <string.h>

// Verifica se determinada string contém um certo caracter 
int existe_caractere(const char *str, char c) {
    return strchr(str, c) != NULL;
}

int posicoes_caractere(const char *word, char c, int posicoes[]) {
    int contador = 0;
    int indice = 0;

    while (*word) { // Percorre a string até o '\0'
        if (*word == c) {
            posicoes[contador++] = indice; // Armazena a posição
        }
        indice++;
        word++; // Avança para o próximo caractere
    }

    return contador; // Retorna a quantidade de ocorrências
}

// Substitui os caracteres encontrados por outro
void substituir_caracteres(char *str, int posicoes[], int total, char novo_caractere) {
    
    for (int i = 0; i < total; i++) {
        str[posicoes[i]] = novo_caractere; // Substitui cada posição armazenada
    }
}

int comparar_strings(const char *str1, const char *str2) {
    // Percorre as duas strings até o fim ou até encontrar diferença
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }

    // Se as strings são iguais, ambos os ponteiros devem apontar para o terminador de string '\0'
    return (*str1 - *str2);
}

