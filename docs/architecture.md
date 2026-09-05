# Arquitetura do Firmware

## Objetivo

Manter o firmware modular, testável e previsível, reduzindo acoplamento entre protocolo, sensores, UI e hardware.

## Componentes atuais

- `app_state`: estado compartilhado da aplicação e sincronização.
- `board_io`: pinagem, GPIOs e abstrações de placa.
- `protocol`: recepção e tratamento do protocolo externo.
- `sensors`: DYP, balança e bioimpedância.
- `storage`: persistência de calibração/configurações.
- `ui`: display, touch e LVGL.
- `leds`: neon e fita RGB.

## Direção arquitetural

```text
                 +------------------+
USB / transporte |   Protocol Task  |
---------------->| parser + routing |
                 +--------+---------+
                          |
                       eventos
                          |
              +-----------v-----------+
              | Measurement / Control |
              |        Services       |
              +-----+-----------+-----+
                    |           |
                 +--v--+     +--v---+
                 | DYP |     |Scale |
                 +--+--+     +--+---+
                    |           |
                    +-----+-----+
                          |
                    +-----v-----+
                    | AppState  |
                    +-----+-----+
                          |
               +----------+----------+
               |                     |
          +----v----+           +----v----+
          |   UI    |           |Protocol |
          +---------+           |response |
                                +---------+
```

## Princípios

### 1. Protocolo não conhece hardware

O parser deve interpretar comandos e produzir intenções da aplicação. Detalhes de UART, GPIO, frame de sensor e timeout ficam nos componentes responsáveis.

### 2. Operações lentas não bloqueiam recepção

A task de protocolo não deve aguardar por centenas de milissegundos por um periférico. Requisições de medição devem evoluir para filas/eventos ou serviço assíncrono.

### 3. Estado compartilhado pequeno

`AppState` mantém apenas dados de estado necessários a mais de um componente. Buffers privados, estados de parser e detalhes de periféricos permanecem dentro dos componentes.

### 4. Dependências direcionadas

Preferência de dependência:

```text
app_main -> componentes
protocol -> serviços / app_state
sensors  -> board_io / app_state
ui       -> app_state
storage  -> app_state/configuração
```

Evitar dependências circulares.

### 5. Hardware configurado em um único lugar

Toda pinagem e baud rate específicos da placa devem ficar em `board_pins.h` ou futura configuração de board equivalente.

## Tasks

A arquitetura atual já usa FreeRTOS. A evolução deve manter responsabilidades claras:

- **protocol task**: framing e parsing de comandos externos;
- **scale task**: recepção contínua da balança;
- **bio task**: recepção/encaminhamento da bioimpedância;
- **UI task**: LVGL e interação de usuário;
- **I/O task**: leitura periódica de entradas lentas;
- **measurement task (planejada)**: coordenação de medições sob demanda, especialmente DYP.

## Estratégia para o DYP

Estado atual: uma solicitação `ALT` pode executar uma leitura bloqueante.

Estado alvo:

1. protocolo recebe `#ALT$`;
2. cria solicitação de medição;
3. task de medição conversa com DYP;
4. valida frame/checksum/timeout;
5. atualiza `AppState`;
6. envia resultado ao protocolo/UI.

## Transporte e logs

O canal de protocolo deve ter fronteira clara com logging. Durante a transição, nenhuma nova funcionalidade deve assumir que `stdout` e `ESP_LOG*` são canais independentes sem validação explícita da configuração do target.

## ADRs

Decisões arquiteturais relevantes devem ser registradas em `docs/adr/` quando forem consolidadas, principalmente:

- escolha do transporte USB definitivo;
- arquitetura assíncrona de medições;
- estratégia LVGL e mutex;
- política de persistência/calibração;
- estratégia de testes host/HIL.
