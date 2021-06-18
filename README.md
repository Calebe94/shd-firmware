# slave-firmware

## Hardware

* [EasyEda - Placa](https://easyeda.com/editor#id=7732b4349d4646e5ac3e3764baabef0e);

### Especificações

* Microcontrolador: [DOIT ESP32 DEVKIT V1](https://docs.platformio.org/en/latest/boards/espressif32/esp32doit-devkit-v1.html#id1)
* Controlador CAN Bus: [MCP2515](https://www.microchip.com/wwwproducts/en/en010406)
* Sensor IR: [QRE1113](https://www.sparkfun.com/datasheets/Robotics/QR_QRE1113.GR.pdf)

### Terminais

| GPIO | Função | Periférico |
|:----:|:------:|:----------:|
| 4 | CS | SPI - MCP2515 |
| 13 | MOSI | SPI - MCP2515 |
| 12 | MISO | SPI - MCP2515 |
| 14 | SCK | SPI - MCP2515 |
| 5 | INT | SPI - MCP2515 |
| 2 | Digital c/ interrupção | Sensor IR |

## Dependências

* [platformio];

Este projeto foi testado e compilado no seguinte ambiente:

* Debian GNU/Linux 11 (bullseye)
* PlatformIO Core, version 5.1.1

```
PACKAGES:
 - framework-arduinoespressif8266 3.30000.210519 (3.0.0)
 - tool-esptool 1.413.0 (4.13)
 - tool-esptoolpy 1.30000.201119 (3.0.0)
 - toolchain-xtensa 2.100200.0 (10.2.0)
```
## Funcionamento

A placa escrava deve ler rosetta giratória do hidrômetro através de um sensor IR, 
que irá pulsar toda vez que a aleta da rosetta interromper o feixe de luz, 
gerando assim uma interrupção digital no microcontrolador.

O microcontrolador será responsável por ler todas estas informações e armazenar na memória flash para previnir se houver falta de luz.

Após um período de tempo configurável o microcontrolador deverá enviar estas informações através do protocolo CAN para o dispositivo **MESTRE**.

A configuração do período de tempo pode ser feita através da interface serial.

### Interrupções

Toda vez que houver um fluxo de água, o sensor de fluxo irá girar a
roleta interna, ativando o sensor de efeito hall gerando uma interrupção
no terminal `GPIO4` do `ESP32`.

Quando a interrupção for gerada, o programa irá enviar esta informação
para uma fila, onde será incrementada uma variável de pulsos.

Haverá uma `task` rodando a cada segundo que irá pegar esta informção e
calcular quantos litros de água passou neste último segundo.

Mais informações podem ser encontradas [neste
artigo](https://how2electronics.com/iot-water-flow-meter-using-esp8266-water-flow-sensor/).

## Utilização

Para compilar o projeto é necessário a instalação do [platformio], e após rodar o seguinte comando:

```sh
$ pio run
```

Para gravar o projeto no microcontrolador basta rodar:

```sh
$ pio run -t upload
```

Se for necessário analisar o log serial do microcontrolador, rode:

```sh
$ pio device monitor --baud 115200 --port /dev/ttyUSB0
```

## Desenvolvedor

## Mantanedor

| ![](https://assets.gitlab-static.net/uploads/-/system/user/avatar/2382314/avatar.png?width=200) |
|:------:|
| [Edimar Calebe Castanho(calebe94@disroot.org)](https://gitlab.com/Calebe94) |

## Referências

* [Platformio: Development Platforms - Espressif 8266](https://docs.platformio.org/en/latest/platforms/espressif8266.html)

[platformio]: https://docs.platformio.org/en/latest/core/installation.html
