# Base ESP-IDF - Medidor Altura/Peso

Esta estrutura é a base inicial da migração para ESP-IDF.

## Conteúdo

```text
main/
components/
  app_state/
  board_io/
  storage/
  sensors/
  protocol/
  leds/
  ui/
```

## Primeiro build

```powershell
idf.py set-target esp32s3
idf.py build
```

## Próximos ajustes obrigatórios

1. Ajustar `components/board_io/include/board_pins.h` com a pinagem real.
2. Migrar display/touch/LVGL real para `components/ui`.
3. Confirmar frame real do sensor DYP.
4. Confirmar frame real da balança.
5. Definir se USB será USB Serial/JTAG ou TinyUSB CDC dedicado.
6. Substituir stub de RGB por driver RMT/led_strip compatível com WS2815.