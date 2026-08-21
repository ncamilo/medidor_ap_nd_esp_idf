# Baseline de Hardware

Este documento separa o que já está codificado do que ainda precisa de validação em bancada.

## Plataforma

- MCU alvo: ESP32-S3.
- Framework: ESP-IDF.
- Sensores/interfaces principais: DYP, balança serial, módulo de bioimpedância, DIP switches, neon PWM e fita RGB.

## Pinagem recuperada da branch `feature/espidf-base`

| Função | Recurso |
|---|---|
| DYP TX | GPIO 8 |
| DYP RX | GPIO 9 |
| Balança RX | GPIO 38 |
| Balança TX | não conectado |
| Bioimpedância TX | GPIO 43 |
| Bioimpedância RX | GPIO 44 |
| DIP orientação | GPIO 11 |
| DIP modo peso | GPIO 12 |
| DIP modo altura | GPIO 13 |
| DIP escala | GPIO 21 |
| Neon PWM | GPIO 15 |
| RGB data | GPIO 18 |

## UARTs

| Interface | UART | Baud rate |
|---|---:|---:|
| DYP | UART1 | 9600 |
| Balança | UART2 | 9600 |
| Bioimpedância | UART0 | 4800 |

> Os valores acima são baseline de implementação, não substituem a validação com o hardware real.

## Hipóteses ainda a validar

- DIP switches acionados para GND com pull-up interno e ativos em nível baixo.
- Balança opera como transmissor serial unidirecional, exigindo apenas RX no ESP32-S3.
- Baud rate da bioimpedância é 4800 baud.
- GPIOs 43/44 podem ser usados pela bioimpedância sem conflito com a estratégia definitiva de console/protocolo USB.

## Checklist de validação de bancada

- [ ] Confirmar alimentação e níveis lógicos de todas as UARTs.
- [ ] Confirmar TX/RX do DYP.
- [ ] Capturar e anexar pelo menos 3 frames reais do DYP.
- [ ] Confirmar RX da balança em GPIO 38.
- [ ] Capturar frames STX/ETX reais da balança em diferentes pesos.
- [ ] Confirmar escala do valor bruto.
- [ ] Confirmar UART e baud rate da bioimpedância.
- [ ] Confirmar polaridade e função de cada DIP.
- [ ] Confirmar GPIO e frequência adequada do neon PWM.
- [ ] Confirmar tipo elétrico e protocolo da fita RGB.

## Regra de mudança

Toda alteração de pinagem deve atualizar este documento e `board_pins.h` no mesmo PR, com justificativa e evidência de teste quando a mudança vier de observação do hardware.
