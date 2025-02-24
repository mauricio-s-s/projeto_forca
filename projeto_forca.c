#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "inc/ssd1306.h"
#include "inc/str_utils.h"

#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

// Biblioteca gerada pelo arquivo .pio durante compilação.
#include "ws2818b.pio.h"

#define I2C_SDA 14
#define I2C_SCL 15
#define BUTTON_JS 22           // Pino de leitura do botão do joystick
#define BUTTON_A 5
#define BUTTON_B 6
// Definição do número de LEDs e pino.

#define LED_COUNT 25
#define LED_PIN 7
//// MATRIZ DE LEDS




// Definição de pixel GRB
struct pixel_t {
  uint8_t G, R, B; // Três valores de 8-bits compõem um pixel.
};
typedef struct pixel_t pixel_t;
typedef pixel_t npLED_t; // Mudança de nome de "struct pixel_t" para "npLED_t" por clareza.

// Declaração do buffer de pixels que formam a matriz.
npLED_t leds[LED_COUNT];

// Variáveis para uso da máquina PIO.
PIO np_pio;
uint sm;

/**
 * Inicializa a máquina PIO para controle da matriz de LEDs.
 */
void npInit(uint pin) {

  // Cria programa PIO.
  uint offset = pio_add_program(pio0, &ws2818b_program);
  np_pio = pio0;

  // Toma posse de uma máquina PIO.
  sm = pio_claim_unused_sm(np_pio, false);
  if (sm < 0) {
    np_pio = pio1;
    sm = pio_claim_unused_sm(np_pio, true); // Se nenhuma máquina estiver livre, panic!
  }

  // Inicia programa na máquina PIO obtida.
  ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);

  // Limpa buffer de pixels.
  for (uint i = 0; i < LED_COUNT; ++i) {
    leds[i].R = 0;
    leds[i].G = 0;
    leds[i].B = 0;
  }
}

/**
 * Atribui uma cor RGB a um LED.
 */
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
  leds[index].R = r;
  leds[index].G = g;
  leds[index].B = b;
}

/**
 * Limpa o buffer de pixels.
 */
void npClear() {
    for (uint i = 0; i < LED_COUNT; ++i) {
        npSetLED(i, 0, 0, 0);
    }
    // Limpa buffer de pixels.
    for (uint i = 0; i < LED_COUNT; ++i) {
        leds[i].R = 0;
        leds[i].G = 0;
        leds[i].B = 0;
    }
    sleep_ms(100);
}

/**
 * Escreve os dados do buffer nos LEDs.
 */
void npWrite() {
  // Escreve cada dado de 8-bits dos pixels em sequência no buffer da máquina PIO.
  for (uint i = 0; i < LED_COUNT; ++i) {
    pio_sm_put_blocking(np_pio, sm, leds[i].G);
    pio_sm_put_blocking(np_pio, sm, leds[i].R);
    pio_sm_put_blocking(np_pio, sm, leds[i].B);
  }
  sleep_us(100); // Espera 100us, sinal de RESET do datasheet.
}

////////////////





// Função para configurar o joystick (pinos de leitura e ADC)
void setup_buttons()
{
  // Inicializa o pino do botão do joystick
  gpio_init(BUTTON_JS);             // Inicializa o pino do botão
  gpio_set_dir(BUTTON_JS, GPIO_IN); // Configura o pino do botão como entrada
  gpio_pull_up(BUTTON_JS);          // Ativa o pull-up no pino do botão para evitar flutuações

  gpio_init(BUTTON_A);
  gpio_set_dir(BUTTON_A, GPIO_IN);
  gpio_pull_up(BUTTON_A);

  gpio_init(BUTTON_B);
  gpio_set_dir(BUTTON_B, GPIO_IN);
  gpio_pull_up(BUTTON_B);

}



void updateDisplay(uint8_t *ssd, char text[][17], struct render_area frame_area){
    int y = 0;
    for (uint i = 0; i < 16; i++)
    {
        ssd1306_draw_string(ssd, 5, y, text[i]);
        y += 8;
    }
    render_on_display(ssd, &frame_area);
    }


//////////////////////



void updateLedsByErrors(int errors){
    if (errors > 0) { // até 1 erro: desenha a cabeça
        npSetLED(17, 0, 10, 0); 
    }
    if (errors > 1){  // até 2 erros: desenha o tronco
        npSetLED(12, 0, 10, 0); 
        npSetLED(7, 0, 10, 0); 
    }
    if (errors > 2){  // até 3 erros: desenha o braço esquerdo
        npSetLED(13, 0, 10, 0);
    }
    if (errors > 3){  // até 4 erros: desenha o braço direito
        npSetLED(11, 0, 10, 0); 
    }
    if (errors > 4){  // até 5 erros: desenha a perna esquerda
        npSetLED(3, 0, 10, 0); 
    }
    if (errors > 5){  // até 2 erros: desenha a perna direita
        npSetLED(1, 0, 10, 0); 
    }


    npWrite(); // Agora os LEDs acendem com as cores especificadas.

}


void draw_led_forca(uint8_t rForca, uint8_t gForca, uint8_t bForca){
    // Define a cor de alguns LEDs no buffer (RGB).
    npSetLED(4, rForca, gForca, bForca); // Define o LED de índice 4 para azul.
    npSetLED(5, rForca, gForca, bForca); // Define o LED de índice 5 para azul.
    npSetLED(14, rForca, gForca, bForca); // Define o LED de índice 14 para azul.
    npSetLED(15, rForca, gForca, bForca); // Define o LED de índice 15 para azul.
    npSetLED(16, rForca, gForca, bForca); // Define o LED de índice 16 para azul.
    npSetLED(18, rForca, gForca, bForca); // Define o LED de índice 18 para azul.
    npSetLED(21, rForca, gForca, bForca); // Define o LED de índice 21 para azul.
    npSetLED(22, rForca, gForca, bForca); // Define o LED de índice 22 para azul.
    npSetLED(23, rForca, gForca, bForca); // Define o LED de índice 23 para azul.
    npSetLED(24, rForca, gForca, bForca); // Define o LED de índice 24 para azul.

    // Envia os valores do buffer para os LEDs físicos.
    npWrite(); // Agora os LEDs acendem com as cores especificadas.
    }
    
void draw_led_win(){
    npClear(); // Garante que todos os LEDs comecem apagados.
    sleep_ms(100);
    npWrite(); // Agora os LEDs acendem com as cores especificadas.

    // Ascende de verde todos os leds do boneco
    npSetLED(17, 0, 10, 0); 
    npSetLED(12, 0, 10, 0); 
    npSetLED(7, 0, 10, 0); 
    npSetLED(13, 0, 10, 0);
    npSetLED(11, 0, 10, 0); 
    npSetLED(3, 0, 10, 0); 
    npSetLED(1, 0, 10, 0); 


    // Envia os valores do buffer para os LEDs físicos.
    sleep_ms(100);
    npWrite(); // Agora os LEDs acendem com as cores especificadas.
    sleep_ms(100);
    }


void draw_led_lose(){
    
    npClear(); // Garante que todos os LEDs comecem apagados.
    sleep_ms(100);
    npWrite(); // Agora os LEDs acendem com as cores especificadas.

    // Ascende de vermelho todos os leds do boneco
    npSetLED(17, 10, 0, 0); 
    npSetLED(12, 10, 0, 0); 
    npSetLED(7, 10, 0, 0); 
    npSetLED(13, 10, 0, 0);
    npSetLED(11, 10, 0, 0); 
    npSetLED(3, 10, 0, 0); 
    npSetLED(1, 10, 0, 0); 

    // Envia os valores do buffer para os LEDs físicos.
    sleep_ms(100);
    npWrite(); // Agora os LEDs acendem com as cores especificadas.
    sleep_ms(100);
    }


int main()
{
    stdio_init_all();   // Inicializa os tipos stdio padrão presentes ligados ao binário

    // Inicialização do i2c
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicialização dos botões 
    setup_buttons();

    // Processo de inicialização completo do OLED SSD1306
    ssd1306_init();

    // Preparar área de renderização para o display (ssd1306_width pixels por ssd1306_n_pages páginas)
    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);

    
    // Inicialização da matriz de LEDS:
    npInit(LED_PIN);


restart:
    // zera o display inteiro
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);


    bool main_screen_mode = true;
    bool letter_selection_mode = false;

    char *categorias[] = {
        "FRUTA",
        "PAIS" };

    char *frutas[] = {
        "BANANA",
        "PERA" };
    char *paises[] = {
        "BRASIL",
        "ALEMANHA"};

    // Escolha randômica de uma categoria e de uma palavra:

    srand(time(NULL));  // Inicializa a semente para gerar números aleatórios

    int categoriaIndice = rand() % 2;  // Escolhe entre 0 (FRUTA) e 1 (PAIS)

    printf("Categoria: %s\n", categorias[categoriaIndice]);
    char *categoria = categorias[categoriaIndice];
    char *word;

    if (categoriaIndice == 0) {  // FRUTA
        int palavraIndex = rand() % (sizeof(frutas) / sizeof(frutas[0]));
        word = frutas[palavraIndex];
    } else {  // PAIS
        int palavraIndex = rand() % (sizeof(paises) / sizeof(paises[0]));
        word = paises[palavraIndex];
    }

    //char *word = "PALAVRA";
    printf("Palavra selecionada: %s\n", word);

    


// Parte do código para exibir a mensagem no display 

    char mainScreenText[][17] = {
        "                ",     //0
        "                ",     //1 - CATEGORIA: 
        "                ",     //2
        "                ",     //3
        "                ",     //4
        "                ",     //5
        "                ",     //6
        "                "};    //7

    

    char selText[][17] = {
        " SELECIONE   B X",     //0
        " A LETRA:      X",     //1
        "               X",     //2
        " ABCDEFGHIJKLM X",     //3
        "               X",     //4
        " NOPQRSTUVWXYZ X",     //5
        "               X",     //6
        "               X"};    //7
    
    //nArray e strPos
    //nArray: número do array dentro da matriz (row - y)
    //strPos: string position, basicamente a coluna (col - x)
    int p_nArray = 1;
    int p_strPos = 13;
    
    int nArray = p_nArray;
    int strPos = p_strPos;

    char selectedLetter = ' ';


    
    char blankChar = 'W';
    int wordSize = strlen(word);
    printf("tamanho: %d", wordSize);
    char maskedWord[wordSize + 1]; // Array local com tamanho dinâmico

    // Preencher com o caractere desejado
    memset(maskedWord, blankChar, wordSize);

    // Adicionar o caractere nulo no final
    maskedWord[wordSize] = '\0';
    
    // atualiza categoria na tela principal
    strncpy(&mainScreenText[1][1], categoria, strlen(categoria));

    // atualiza masked word na tela principal
    strncpy(&mainScreenText[3][1], maskedWord, strlen(maskedWord));
    updateDisplay(ssd, mainScreenText, frame_area);


    int erros = 0;

    
    npClear(); // Garante que todos os LEDs comecem apagados.

    //Desenhar a forca

    uint8_t rForca = 10;
    uint8_t gForca = 0;
    uint8_t bForca = 0;
    draw_led_forca(rForca, gForca, bForca);

    sleep_ms(100);
    //npClear(); // Garante que todos os LEDs comecem apagados.
    sleep_ms(100);
    npWrite();

    // Loop infinito.
    while(true) {
        printf("Palavra selecionada: %s\n", word);
        sleep_ms(500);

        // Verificação dos botões   // HIGH = solto, LOW = pressionado
        bool button_js_state = !gpio_get(BUTTON_JS);  
        bool button_a_state = !gpio_get(BUTTON_A);    
        bool button_b_state = !gpio_get(BUTTON_B);
        bool letterSelected = false;


        // debug...
        if (button_a_state){
            printf("\nBotão A pressionado!\n");

        }
        if (button_b_state){
            printf("\nBotão B pressionado!\n");

        }


        if (main_screen_mode) {
            if (button_js_state){
                printf("\nBotão JS pressionado!\n");
                selText[p_nArray][p_strPos] = ' ';  
                selText[nArray][strPos] = 'X';  
                updateDisplay(ssd, selText, frame_area);


                letter_selection_mode = true;
                main_screen_mode = false;
                }
        }
        else { //letter_selection_mode
            if (button_js_state) {
                printf("\nBotão JS pressionado!\n");
                updateDisplay(ssd, mainScreenText, frame_area);
                letter_selection_mode = false;
                main_screen_mode = true;

                if (nArray == 1) {
                    printf("Nenhuma letra selecionada! \n");
                    letterSelected = false;
                    }
                else {
                    letterSelected = true;
                    selectedLetter = selText[nArray-1][strPos];
                    printf("Letra selecionada: %c \n", selectedLetter);

                    //LETRA SELECIONADA, PROCESSAR:
                    // 1. Verificar se o caractere existe
                    if (existe_caractere(word, selectedLetter)) {
                        printf("O caractere '%c' existe na string.\n", selectedLetter);

                        //// Processar acerto
                        int posicoes[wordSize + 1]; // Array para armazenar as posições (máximo de 100 ocorrências)
                    
                        // Encontrar todas as posições do caractere
                        int nOcorrencias = posicoes_caractere(word, selectedLetter, posicoes);
                    
                        if (nOcorrencias > 0) {
                            printf("O caractere '%c' aparece nas posições: ", selectedLetter);
                            for (int i = 0; i < nOcorrencias; i++) {
                                printf("%d ", posicoes[i]);
                            }
                            printf("\n");
                        }

                        // Substituir os caracteres blank encontrados pela letra efetiva da palavra
                        substituir_caracteres(maskedWord, posicoes, nOcorrencias, selectedLetter);
                        printf("String após substituição: %s\n", maskedWord);
                        
                        /////////////////// fim processar acerto

                        // Verifica se as duas palavras são iguais
                        if (comparar_strings(word, maskedWord) == 0) {
                            printf("As palavras são iguais agora: %s\n", maskedWord);
                            draw_led_win();

                            //Atualiza a tela principal: PARABÉNS! VOCÊ GANHOU!!
                            strncpy(&mainScreenText[5][1], "PARABENS", strlen("PARABENS"));
                            strncpy(&mainScreenText[6][1], "VOCE GANHOU", strlen("VOCE GANHOU"));
                            updateDisplay(ssd, mainScreenText, frame_area);

                            while (true){
                                // Pressionar joystick para reiniciar
                                bool button_js_state = !gpio_get(BUTTON_JS);  
                                if (button_js_state){
                                    goto restart; // Reinicia o jogo
                                }
                            }

                        }

                        // atualiza masked word na tela principal
                        strncpy(&mainScreenText[3][1], maskedWord, strlen(maskedWord));
                        updateDisplay(ssd, mainScreenText, frame_area);

                    }
                    else{
                        erros++;
                        updateLedsByErrors(erros);
                        printf("O caractere '%c' NÃO existe na string.\n", selectedLetter);
                        printf("String no momento: %s\n", maskedWord);
                        printf("ERROS: %d", erros);

                        if (erros == 6){
                            draw_led_lose();
                            strncpy(&mainScreenText[5][1], "QUE PENA", strlen("QUE PENA"));
                            strncpy(&mainScreenText[6][1], "VOCE PERDEU", strlen("VOCE GANHOU"));
                            updateDisplay(ssd, mainScreenText, frame_area);

                            while (true){
                                // Pressionar joystick para reiniciar
                                bool button_js_state = !gpio_get(BUTTON_JS);  
                                if (button_js_state){
                                    goto restart; // Reinicia o jogo
                                }
                            }
                        }
                    }
                }
            }

            else if (button_a_state){ //desce o cursor
                p_nArray = nArray;
                p_strPos = strPos;
                if (nArray == 1) nArray = 4;
                else if (nArray == 4) nArray = 6;
                else { // nArray = 6
                    if (strPos == 13) nArray = 1;
                    else {
                        nArray = 4;
                        }
                }
                printf("p_nArray: %d\n", p_nArray);
                printf("nArray: %d\n", nArray);

                selText[p_nArray][p_strPos] = ' ';  
                selText[nArray][strPos] = 'X';  
                updateDisplay(ssd, selText, frame_area);

                }
            else if (button_b_state){
                p_strPos = strPos;
                p_nArray = nArray;

                if (strPos < 13) strPos = strPos+1;
                else{ //strPos = 13
                    strPos = 1;
                    if (nArray == 1){
                        p_nArray = nArray;
                        nArray = 4;
                    }

                }
                printf("p_nArray: %d\n", p_nArray);
                printf("p_strPos: %d\n", p_strPos);
                printf("nArray: %d\n", nArray);
                printf("strPos: %d\n", strPos);

                selText[p_nArray][p_strPos] = ' ';  
                selText[nArray][strPos] = 'X';  
                updateDisplay(ssd, selText, frame_area);

            }
            
            
            }
    }
    return 0;
}

