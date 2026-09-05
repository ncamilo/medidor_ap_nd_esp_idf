# Requisitos de Software

Este documento estabelece a primeira baseline rastreável de requisitos do firmware do medidor antropométrico e de bioimpedância.

## Convenções

- `REQ-FUN-*`: requisito funcional.
- `REQ-NFR-*`: requisito não funcional.
- `REQ-HW-*`: requisito de interface com hardware.
- `REQ-PROTO-*`: requisito de protocolo.
- `REQ-TEST-*`: requisito de verificabilidade.

## Requisitos funcionais

### Altura

- **REQ-FUN-DYP-001** — O firmware deve iniciar uma medição de altura ao receber o comando `#ALT$`.
- **REQ-FUN-DYP-002** — O frame recebido do sensor DYP deve ser validado antes de atualizar o estado da aplicação.
- **REQ-FUN-DYP-003** — O firmware deve aplicar a calibração persistida à medição de altura.
- **REQ-FUN-DYP-004** — Uma falha na medição deve resultar em `FEALT$`.
- **REQ-FUN-DYP-005** — Uma medição válida deve resultar em `FAxxxx$`, conforme o protocolo vigente.

### Peso

- **REQ-FUN-SCALE-001** — O firmware deve receber continuamente frames da balança serial delimitados por STX/ETX.
- **REQ-FUN-SCALE-002** — Apenas frames válidos devem atualizar o peso no estado global.
- **REQ-FUN-SCALE-003** — O comando `#PES$` deve retornar o último peso válido conhecido.
- **REQ-FUN-SCALE-004** — A regra de escala do valor bruto deve ser configurável de acordo com o hardware/DIP e validada em bancada.

### Bioimpedância

- **REQ-FUN-BIO-001** — Comandos iniciados por `F` devem poder ser encaminhados ao módulo de bioimpedância.
- **REQ-FUN-BIO-002** — A resposta da bioimpedância deve retornar ao canal de protocolo sem ser misturada com logs.

### LEDs e saídas

- **REQ-FUN-LED-001** — `#RG1$` e `#RG0$` devem habilitar/desabilitar a fita RGB.
- **REQ-FUN-LED-002** — `#RGBrrrgggbbb$` deve atualizar a cor configurada da fita RGB.
- **REQ-FUN-NEON-001** — `#NE1$` e `#NE0$` devem habilitar/desabilitar o neon.

## Requisitos de arquitetura e concorrência

- **REQ-NFR-ARCH-001** — O protocolo USB não deve depender diretamente dos detalhes de implementação dos sensores.
- **REQ-NFR-ARCH-002** — Medições potencialmente bloqueantes devem ser executadas fora da task responsável pelo recebimento do protocolo.
- **REQ-NFR-ARCH-003** — O estado compartilhado da aplicação deve permanecer protegido contra acesso concorrente.
- **REQ-NFR-ARCH-004** — Novas funcionalidades devem preferir comunicação por filas, eventos ou APIs de componente em vez de dependências globais diretas.
- **REQ-NFR-LOG-001** — Logs de diagnóstico não devem corromper nem se misturar ao protocolo de comunicação da aplicação.

## Requisitos de hardware

- **REQ-HW-PINS-001** — A pinagem usada pelo firmware deve existir em um único arquivo de configuração de placa.
- **REQ-HW-PINS-002** — A UART da balança deve suportar operação apenas com RX quando o equipamento externo for somente transmissor.
- **REQ-HW-VALID-001** — Pinagem, baud rate, polaridade de DIP e escalas de sensores devem ser validados em hardware real antes de serem considerados definitivos.

## Requisitos de teste

- **REQ-TEST-001** — Parsers de protocolo e de frames de sensores devem possuir casos de teste para entradas válidas, inválidas, incompletas e fora de faixa.
- **REQ-TEST-002** — Cada requisito funcional implementado deve possuir ao menos uma estratégia de verificação documentada.
- **REQ-TEST-003** — Alterações que dependam de hardware devem registrar o resultado do teste de bancada no PR ou em documento de validação.

## Itens ainda pendentes de validação física

1. Posição e semântica exata dos bytes de distância no frame do DYP.
2. Checksum e tratamento de timeout do DYP.
3. Regra exata de escala da balança (`/100`, `/10` ou outra).
4. Polaridade e função final de cada DIP switch.
5. Separação definitiva entre console de logs e canal de protocolo.
6. Comportamento temporal e frames reais da bioimpedância.

## Critério de pronto

Uma funcionalidade só deve ser considerada concluída quando houver:

1. requisito identificado;
2. implementação revisada;
3. build válido;
4. teste automatizado quando aplicável;
5. teste em hardware quando aplicável;
6. evidência da validação no PR.
