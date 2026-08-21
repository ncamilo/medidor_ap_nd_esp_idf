# Protocolo de Comunicação

## Framing

O protocolo de aplicação utiliza comandos delimitados por `#` e `$`.

Exemplos:

```text
#ALT$
#PES$
#RGB255000128$
#RG1$
#NE0$
```

## Comandos conhecidos

| Comando | Função | Resposta esperada |
|---|---|---|
| `#ALT$` | Solicitar altura | `FAxxxx$` ou `FEALT$` |
| `#PES$` | Solicitar peso atual | `PESxx.xxx$` |
| `#RGBrrrgggbbb$` | Configurar RGB | a definir/compatibilidade atual |
| `#RG1$` | Ligar RGB | a definir |
| `#RG0$` | Desligar RGB | a definir |
| `#NE1$` | Ligar neon | a definir |
| `#NE0$` | Desligar neon | a definir |
| `#F...$` | Encaminhar à bioimpedância | resposta do módulo |

## Regras do parser

1. Ignorar bytes até encontrar `#`.
2. Após `#`, acumular bytes até `$`.
3. Reiniciar o frame se um novo `#` aparecer antes de `$`.
4. Rejeitar frames que excedam o buffer configurado.
5. Não executar parcialmente comandos incompletos.
6. Validar comprimento e formato dos parâmetros antes de alterar o estado.

## Separação entre transporte e aplicação

O framing `#...$` pertence à camada de protocolo. USB Serial/JTAG, USB CDC ou UART são transportes e devem poder evoluir sem reescrever as regras dos comandos.

Arquitetura alvo:

```text
transport_rx -> frame_parser -> command_dispatch -> application service
application result -> response_encoder -> transport_tx
```

## Logs

Logs de diagnóstico não fazem parte do protocolo. O projeto deve impedir que mensagens `ESP_LOG*` sejam interpretadas pelo software externo como respostas do equipamento.

Até a separação física/lógica estar validada, qualquer mudança na configuração de console deve ser tratada como item de teste de integração.

## Casos mínimos de teste

- frame válido simples: `#PES$`;
- frame dividido em várias leituras;
- lixo antes do `#`;
- novo `#` durante frame incompleto;
- frame sem `$`;
- frame maior que o buffer;
- comando desconhecido;
- parâmetros RGB curtos, longos e não numéricos;
- sequência rápida de comandos válidos;
- confirmação de que logs não aparecem no stream esperado pelo software externo.

## Compatibilidade

Alterações de formato de comando/resposta são breaking changes e devem ser explicitadas no PR. A migração para ESP-IDF deve preservar o protocolo esperado pelo software já existente, salvo decisão documentada em contrário.
