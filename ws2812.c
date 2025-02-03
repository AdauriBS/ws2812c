#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/irq.h"
#include "hardware/clocks.h"
#include "pico/multicore.h"
#include "ws2812.pio.h"

// Definições de constantes.
#define IS_RGBW false           // Define se o LED é do tipo RGBW
#define NUM_PIXELS 25           // Número total de LEDs na matriz
#define WS2812_PIN 7            // Pino conectado à fita de LEDs WS2812

#define LED_RED_PIN 13          // Pino do LED vermelho
#define BTN_A_PIN 5             // Pino do botão A (incrementar)
#define BTN_B_PIN 6             // Pino do botão B (decrementar)

#define DEBOUNCE_DELAY_US 30000 // Delay para debounce dos botões (em microsegundos)

volatile int counter = 0;       // Contador global de 0 a 9

// Matrizes de pixels para exibir números de 0 a 9
bool numbers[10][NUM_PIXELS] = {
    {0,1,1,1,0, 0,1,0,1,0, 0,1,0,1,0, 0,1,0,1,0, 0,1,1,1,0}, // 0
    {0,1,1,1,0, 0,0,1,0,0, 0,0,1,0,0, 0,1,1,0,0, 0,0,1,0,0}, // 1
    {0,1,1,1,0, 0,1,0,0,0, 0,1,1,1,0, 0,0,0,1,0, 0,1,1,1,0}, // 2
    {0,1,1,1,0, 0,0,0,1,0, 0,1,1,1,0, 0,0,0,1,0, 0,1,1,1,0}, // 3
    {0,1,0,0,0, 0,0,0,1,0, 0,1,1,1,0, 0,1,0,1,0, 0,1,0,1,0}, // 4
    {0,1,1,1,0, 0,0,0,1,0, 0,1,1,1,0, 0,1,0,0,0, 0,1,1,1,0}, // 5
    {0,1,1,1,0, 0,1,0,1,0, 0,1,1,1,0, 0,1,0,0,0, 0,1,1,1,0}, // 6
    {0,1,0,0,0, 0,0,0,1,0, 0,1,0,0,0, 0,0,0,1,0, 0,1,1,1,0}, // 7
    {0,1,1,1,0, 0,1,0,1,0, 0,1,1,1,0, 0,1,0,1,0, 0,1,1,1,0}, // 8
    {0,1,1,1,0, 0,0,0,1,0, 0,1,1,1,0, 0,1,0,1,0, 0,1,1,1,0}  // 9
};

// Função para enviar dados para o LED WS2812
static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

// Converte valores RGB em um único valor de 32 bits
static inline uint32_t rgb_to_grb(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

// Exibe o número com uma cor específica
void display_number(bool *buffer, uint8_t r, uint8_t g, uint8_t b) {
    // Reduz o brilho para 20% do máximo
    r = (r * 20) / 100;
    g = (g * 20) / 100;
    b = (b * 20) / 100;

    uint32_t color = rgb_to_grb(r, g, b);
    for (int i = 0; i < NUM_PIXELS; i++) {
        put_pixel(buffer[i] ? color : 0); // Liga ou desliga o LED com base no buffer
    }
}

// Configuração inicial dos pinos
void setup() {
    gpio_init(LED_RED_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);

    gpio_init(BTN_A_PIN);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_pull_up(BTN_A_PIN);

    gpio_init(BTN_B_PIN);
    gpio_set_dir(BTN_B_PIN, GPIO_IN);
    gpio_pull_up(BTN_B_PIN);
}

// Função de interrupção para tratar o pressionamento dos botões
void button_callback(uint gpio, uint32_t events) {
    static volatile uint64_t last_interrupt_time_a = 0;
    static volatile uint64_t last_interrupt_time_b = 0;
    uint64_t current_time = time_us_64();

    if (gpio == BTN_A_PIN && current_time - last_interrupt_time_a > DEBOUNCE_DELAY_US) {
        counter = (counter + 1) % 10; // Incrementa o contador
        last_interrupt_time_a = current_time;
    } 
    else if (gpio == BTN_B_PIN && current_time - last_interrupt_time_b > DEBOUNCE_DELAY_US) {
        counter = (counter - 1 + 10) % 10; // Decrementa o contador
        last_interrupt_time_b = current_time;
    }
}

// Piscar LED vermelho continuamente em um núcleo separado
void led_red_blink() {
    while (1) {
        gpio_put(LED_RED_PIN, 1);
        sleep_ms(100);
        gpio_put(LED_RED_PIN, 0);
        sleep_ms(100);
    }
}

int main() {
    stdio_init_all();
    setup();

    multicore_launch_core1(led_red_blink); // Executa o piscar do LED em outro núcleo

    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    gpio_set_irq_enabled_with_callback(BTN_A_PIN, GPIO_IRQ_EDGE_RISE, true, &button_callback);
    gpio_set_irq_enabled_with_callback(BTN_B_PIN, GPIO_IRQ_EDGE_RISE, true, &button_callback);

    // Definindo cores para cada número
    uint8_t colors[10][3] = {
        {5, 0, 0},    // Vermelho para 0
        {0, 5, 0},    // Verde para 1
        {0, 0, 5},    // Azul para 2
        {5, 5, 0},    // Amarelo para 3
        {0, 5, 5},    // Ciano para 4
        {5, 0, 5},    // Magenta para 5
        {2, 0, 5},    // Roxo para 6
        {5, 2, 0},    // Laranja para 7
        {2, 0, 5},    // Roxo para 8
        {0, 2, 5}     // Azul claro para 9
    };

    while (1) {
        display_number(numbers[counter], colors[counter][0], colors[counter][1], colors[counter][2]);
        sleep_ms(100);
    }

    return 0;
}
