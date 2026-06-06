# Migração Firmware ESP32 para ESP-IDF

Projeto de migração do firmware atual baseado em Arduino Core para uma arquitetura em ESP-IDF puro, mantendo o mesmo comportamento do produto final.

## Objetivo

Migrar o firmware existente para ESP-IDF, preservando as funcionalidades atuais:

- Interface gráfica com LVGL.
- Display e touch.
- Medição de altura via sensor DYP.
- Leitura de peso via balança serial.
- Comunicação com módulo de bioimpedância.
- Protocolo USB serial com comandos `#...$`.
- Controle de fita RGB.
- Controle de neon por PWM.
- Leitura de DIP switches.
- Calibração persistente em memória não volátil.
- Watchdog, logs e estabilidade operacional.

## Status geral

```text
[ ] Repositório Git criado
[ ] Repositório publicado no GitHub
[ ] Projeto ESP-IDF base criado
[ ] Build inicial validado
[ ] Display inicializado
[ ] Touch inicializado
[ ] LVGL rodando no ESP-IDF
[ ] Tela principal migrada
[ ] Tela de calibração migrada
[ ] GPIO/DIP switches migrados
[ ] NVS/calibração migrada
[ ] UART do sensor DYP migrada
[ ] UART da balança migrada
[ ] UART da bioimpedância migrada
[ ] Protocolo USB migrado
[ ] Neon PWM migrado
[ ] Fita RGB migrada
[ ] Teste em bancada executado
[ ] Produto validado com hardware real
```

## Funcionalidades atuais do produto

### 1. Display e interface

```text
[ ] Inicializar display
[ ] Inicializar touch
[ ] Rodar LVGL em task dedicada
[ ] Proteger chamadas LVGL com mutex
[ ] Atualizar tela principal
[ ] Atualizar tela de calibração
[ ] Alternar orientação conforme DIP switch
```

### 2. Sensor de altura DYP

```text
[ ] Enviar comando de medição
[ ] Ler frame de resposta
[ ] Validar header
[ ] Validar checksum
[ ] Calcular distância
[ ] Aplicar calibração
[ ] Responder ao comando USB #ALT$
[ ] Atualizar interface
```

### 3. Balança serial

```text
[ ] Configurar UART da balança
[ ] Ler frames com STX/ETX
[ ] Extrair peso
[ ] Aplicar escala correta
[ ] Detectar estabilidade
[ ] Atualizar peso no estado global
[ ] Responder ao comando USB #PES$
[ ] Atualizar interface
```

### 4. Bioimpedância

```text
[ ] Configurar UART da bioimpedância
[ ] Repassar comandos iniciados por F
[ ] Encaminhar resposta para USB
[ ] Evitar conflito com logs
```

### 5. Protocolo USB

Comandos esperados:

```text
#ALT$              -> medir altura
#PES$              -> retornar peso atual
#RGBrrrgggbbb$     -> definir cor RGB
#RG1$              -> ligar fita RGB
#RG0$              -> desligar fita RGB
#NE1$              -> ligar neon
#NE0$              -> desligar neon
#F...$             -> repassar para bioimpedância
```

Respostas esperadas:

```text
FAxxxx$            -> altura medida com sucesso
FEALT$             -> erro na medição de altura
PESxx.xxx$         -> peso atual
```

## Arquitetura planejada

```text
firmware/
  CMakeLists.txt
  sdkconfig.defaults
  main/
    app_main.cpp
  components/
    app_state/
      app_state.h
      app_state.cpp
    board_io/
      board_pins.h
      board_io.cpp
    storage/
      calibration_storage.h
      calibration_storage.cpp
    protocol/
      usb_protocol.h
      usb_protocol.cpp
    sensors/
      dyp_sensor.h
      dyp_sensor.cpp
      scale_sensor.h
      scale_sensor.cpp
      bio_serial.h
      bio_serial.cpp
    ui/
      ui_app.h
      ui_app.cpp
      ui_screens.cpp
      lvgl_v8_port.h
      lvgl_v8_port.cpp
    leds/
      neon_pwm.h
      neon_pwm.cpp
      rgb_strip.h
      rgb_strip.cpp
```

## Mapeamento Arduino Core para ESP-IDF

| Código atual | Substituição ESP-IDF |
|---|---|
| `setup()` | `app_main()` |
| `loop()` | tasks FreeRTOS |
| `Serial` | USB CDC, USB Serial/JTAG ou UART driver |
| `HardwareSerial` | `uart_driver_install`, `uart_read_bytes`, `uart_write_bytes` |
| `Preferences` | NVS nativo |
| `pinMode` | `gpio_config` |
| `digitalRead` | `gpio_get_level` |
| `delay` | `vTaskDelay` |
| `millis` | `esp_timer_get_time()` |
| `ledcAttachChannel` | `ledc_timer_config` + `ledc_channel_config` |
| `ledcWrite` | `ledc_set_duty` + `ledc_update_duty` |
| `FastLED` | driver RMT/LED strip compatível |
| `Serial.printf` | `ESP_LOGI`, `ESP_LOGW`, `ESP_LOGE` |

## Ordem de migração

### Fase 0 — Controle de versão

```text
[ ] Criar repositório local
[ ] Criar `.gitignore`
[ ] Adicionar README.md
[ ] Criar primeiro commit
[ ] Criar repositório no GitHub
[ ] Enviar branch main
```

### Fase 1 — Projeto ESP-IDF base

```text
[ ] Criar estrutura ESP-IDF
[ ] Criar `main/app_main.cpp`
[ ] Criar `CMakeLists.txt`
[ ] Criar `sdkconfig.defaults`
[ ] Validar `idf.py build`
[ ] Validar flash no hardware
```

### Fase 2 — Display/LVGL

```text
[ ] Migrar inicialização do display
[ ] Migrar touch
[ ] Migrar port LVGL
[ ] Validar renderização básica
[ ] Validar touch
[ ] Validar orientação
```

### Fase 3 — Estado global e eventos

```text
[ ] Criar `AppState`
[ ] Criar mutex de estado
[ ] Criar filas de eventos
[ ] Separar eventos de UI, sensores, USB e LEDs
```

### Fase 4 — Sensores

```text
[ ] Migrar sensor DYP
[ ] Migrar balança
[ ] Migrar bioimpedância
[ ] Testar cada UART isoladamente
```

### Fase 5 — Protocolo USB

```text
[ ] Implementar parser `#...$`
[ ] Implementar comandos ALT, PES, RGB, RG, NE e F
[ ] Garantir que logs não misturem com protocolo
[ ] Testar com software externo
```

### Fase 6 — LEDs

```text
[ ] Migrar neon para LEDC nativo
[ ] Migrar fita RGB para driver ESP-IDF
[ ] Validar consumo, fonte e ruído
[ ] Garantir que LED não bloqueie UI
```

### Fase 7 — Calibração e persistência

```text
[ ] Migrar Preferences para NVS
[ ] Validar leitura após reboot
[ ] Validar gravação de calibração
[ ] Validar valores padrão
```

### Fase 8 — Testes finais

```text
[ ] Teste de boot frio
[ ] Teste de reboot
[ ] Teste com USB conectado
[ ] Teste com USB desconectado
[ ] Teste contínuo por várias horas
[ ] Teste com balança
[ ] Teste com sensor DYP
[ ] Teste com bioimpedância
[ ] Teste com LEDs ligados
[ ] Teste de watchdog
```

## Estratégia Git

### Branches

```text
main                  -> versão estável
develop               -> integração da migração
feature/espidf-base   -> projeto ESP-IDF base
feature/lvgl-port     -> display, touch e LVGL
feature/sensors       -> DYP, balança e bioimpedância
feature/usb-protocol  -> protocolo #...$
feature/leds          -> neon e fita RGB
feature/storage-nvs   -> calibração em NVS
```

### Commits sugeridos

Formato:

```text
tipo: descrição curta
```

Tipos:

```text
chore: configuração, estrutura, build
feat: nova funcionalidade
fix: correção de bug
refactor: reorganização sem mudar comportamento
docs: documentação
test: testes
```

Exemplos:

```text
docs: adiciona roadmap da migracao ESP-IDF
chore: cria estrutura base do projeto ESP-IDF
feat: adiciona inicializacao do LVGL
feat: migra leitura do sensor DYP para UART nativa
fix: corrige parser do protocolo USB
refactor: separa estado global em AppState
```

## Comandos Git iniciais

Na raiz do projeto:

```bash
git init
git add README.md
git commit -m "docs: adiciona roadmap da migracao ESP-IDF"
```

Depois de criar o repositório no GitHub:

```bash
git branch -M main
git remote add origin git@github.com:SEU_USUARIO/SEU_REPOSITORIO.git
git push -u origin main
```

Alternativa usando HTTPS:

```bash
git remote add origin https://github.com/SEU_USUARIO/SEU_REPOSITORIO.git
git push -u origin main
```

## `.gitignore` recomendado

Criar arquivo `.gitignore` na raiz:

```gitignore
# ESP-IDF build
build/
managed_components/
sdkconfig.old

# Python / venv
.venv/
venv/
__pycache__/
*.pyc

# IDEs
.vscode/
.idea/

# OS
.DS_Store
Thumbs.db

# Logs
*.log

# Secrets
.env
*.pem
*.key
```

Observação: avaliar se `sdkconfig` será versionado. Em firmware embarcado, normalmente vale versionar `sdkconfig.defaults` e só versionar `sdkconfig` quando a configuração estiver estabilizada.

## Decisões técnicas pendentes

```text
[ ] Confirmar modelo exato do ESP32
[ ] Confirmar versão alvo do ESP-IDF
[ ] Confirmar driver USB: USB Serial/JTAG ou TinyUSB CDC
[ ] Confirmar driver da fita RGB compatível com WS2815
[ ] Confirmar pinagem final
[ ] Confirmar baud rate de cada UART
[ ] Confirmar se logs podem sair pela mesma USB do protocolo
[ ] Confirmar estratégia de senha de calibração
```

## Riscos conhecidos

### USB e logs

O protocolo USB não deve ser misturado com logs se o software externo espera respostas limpas.

Solução recomendada:

```text
- Canal USB dedicado ao protocolo; ou
- Logs por UART separada; ou
- Logs desativados/reduzidos em modo produção.
```

### Fita RGB WS2815

A fita RGB precisa de validação de timing e elétrica.

Checklist:

```text
[ ] Confirmar tensão de alimentação
[ ] Confirmar GND comum com ESP32
[ ] Confirmar nível lógico aceito
[ ] Confirmar driver compatível
[ ] Testar com poucos LEDs antes dos 300 LEDs
```

### Medição de altura bloqueante

A medição do DYP não deve rodar dentro da task/timer da UI.

Solução recomendada:

```text
UI/USB -> fila de pedido de medição -> task DYP -> evento de resultado -> UI/USB
```

### Estado global

Evitar variáveis globais acessadas diretamente por várias tasks.

Solução recomendada:

```text
AppState + mutex + filas de eventos
```

## Critério de pronto

A migração será considerada pronta quando:

```text
[ ] Produto liga sem crash
[ ] UI abre corretamente
[ ] Touch funciona
[ ] Orientação funciona por DIP
[ ] Peso aparece corretamente
[ ] Altura mede corretamente
[ ] Bioimpedância comunica corretamente
[ ] Protocolo USB responde igual ao firmware anterior
[ ] RGB funciona
[ ] Neon funciona
[ ] Calibração persiste após reboot
[ ] Watchdog não reseta em uso normal
[ ] Teste contínuo validado em bancada
```

## Histórico de progresso

### 2026-06-06

```text
[x] Análise inicial do firmware atual
[x] Confirmação de viabilidade da migração para ESP-IDF
[x] Definição inicial da arquitetura de migração
[x] Criação deste README.md
```

## Próximos passos

```text
[ ] Criar repositório Git local
[ ] Adicionar README.md
[ ] Criar `.gitignore`
[ ] Fazer primeiro commit
[ ] Publicar no GitHub
[ ] Criar branch `develop`
[ ] Criar branch `feature/espidf-base`
[ ] Criar projeto ESP-IDF mínimo
```
