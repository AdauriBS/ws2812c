# TF-2FEV - Contador Digital com Controle de LEDs e Interrupções

Este projeto visa o controle de LEDs e interrupções em um contador digital utilizando a placa RP2040, com foco na prática de manipulação de hardware e controle de displays. O sistema implementa dois botões (A e B) para incrementar e decrementar um número exibido em uma matriz de LEDs WS2812, com tratamento de *debouncing* via software. Além disso, utiliza LEDs RGB para efeitos visuais e aprendizado sobre o uso de interrupções.

## Funcionalidades

### LED RGB
- O LED vermelho pisca continuamente a 5Hz.

### Botão A
- Incrementa o número exibido na matriz de LEDs a cada pressionamento.

### Botão B
- Decrementa o número exibido na matriz de LEDs a cada pressionamento.

### Matriz de LEDs WS2812
- Exibe números de 0 a 9 com um formato fixo em estilo digital.

### Debouncing de Botões
- Implementação de software para evitar múltiplos disparos ao pressionar os botões.

## Requisitos

### Uso de Interrupções
- Todas as interações com os botões são controladas por interrupções (IRQ).

### Debouncing de Botões
- Implementação do tratamento de *debouncing* de maneira eficiente.

### Controle de LEDs
- Demonstra o controle de LEDs RGB e WS2812, com visualização clara dos números no display.

### Estrutura do Código
- O código está bem estruturado, comentado e organizado para facilitar a compreensão e a manutenção.

## Objetivo

Este projeto é ideal para quem deseja aprender sobre:
- Controle de microcontroladores e LEDs
- Uso de interrupções em sistemas embarcados
- Implementação de *debouncing* para evitar problemas de leitura de botões
- Trabalhar com displays de LEDs, tanto comuns quanto endereçáveis (WS2812)

## Vídeos
Disponível em:
- (https://drive.google.com/drive/folders/14NSxYmJy8ZGOrdKPMl8W56xIfk0t0z8d?usp=sharing)

