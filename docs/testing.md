# Estratégia de Testes

## Pirâmide de validação

### 1. Testes de host / unidade

Prioridade para lógica que não depende do hardware:

- parser `#...$`;
- parser da balança;
- parser/validação do DYP;
- encoding de respostas;
- regras de calibração;
- conversões e limites.

### 2. Testes de integração no ESP32-S3

- inicialização das tasks;
- filas/eventos;
- NVS;
- concorrência em `AppState`;
- UARTs;
- separação de logs e protocolo;
- watchdog.

### 3. HIL — Hardware-in-the-loop

- frames reais de sensores;
- temporização;
- cabos desconectados;
- sensores sem resposta;
- reboot durante operação;
- sequência repetida de medições;
- comunicação com o software externo real.

## Matriz inicial de rastreabilidade

| Teste | Requisito | Tipo |
|---|---|---|
| TEST-PROTO-001 | REQ-FUN-SCALE-003 | unidade/integração |
| TEST-PROTO-002 | REQ-NFR-LOG-001 | integração/HIL |
| TEST-SCALE-001 | REQ-FUN-SCALE-001 | unidade/HIL |
| TEST-SCALE-002 | REQ-FUN-SCALE-002 | unidade |
| TEST-DYP-001 | REQ-FUN-DYP-001 | integração/HIL |
| TEST-DYP-002 | REQ-FUN-DYP-002 | unidade/HIL |
| TEST-DYP-003 | REQ-FUN-DYP-004 | integração/HIL |
| TEST-HW-001 | REQ-HW-PINS-002 | HIL |

## Evidência mínima no PR

Para alteração somente documental:

- revisão de consistência.

Para alteração de lógica sem hardware:

- build;
- teste de unidade aplicável;
- descrição do cenário validado.

Para alteração dependente de hardware:

- build;
- identificação da placa;
- passos executados;
- resultado observado;
- logs/frame capturado quando útil.

## Casos adversos prioritários

- DYP sem responder por timeout;
- frame DYP com checksum inválido;
- frame de balança incompleto;
- balança transmitindo continuamente durante comandos USB;
- bioimpedância sem resposta;
- buffer do protocolo excedido;
- comandos consecutivos sem intervalo significativo;
- reinicialização do equipamento com valores de calibração persistidos;
- acesso simultâneo ao estado por UI, sensores e protocolo.

## Definition of Done

Uma issue não deve ser encerrada apenas porque o código foi escrito. O PR deve demonstrar quais requisitos foram atendidos e como foram verificados.
