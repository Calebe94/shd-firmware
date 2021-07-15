![](https://gitlab.com/projeto-leitor-hidrometro/shd-firmware/badges/main/pipeline.svg)

# SHD - Firmware

Firmware dos dispositivos leitores de fluxo de água do projeto **Hidrometro Digital**.

## Hardware

* [EasyEda - Placa](https://easyeda.com/editor#id=7732b4349d4646e5ac3e3764baabef0e);

### Especificações

* Microcontrolador: 
    * Principal: [ESP32 LilyGO T-PCIE + Módulo SIM7070G](https://github.com/Xinyuan-LilyGO/LilyGo-T-PCIE)
    * Auxiliar: [DOIT ESP32 DEVKIT V1](https://docs.platformio.org/en/latest/boards/espressif32/esp32doit-devkit-v1.html#id1)
* Controlador RS485: [MAX485](https://www.maximintegrated.com/en/products/interface/transceivers/MAX485.html#tech-docs)
* Sensor de fluxo: [YF-S201](https://www.sparkfun.com/datasheets/Robotics/QR_QRE1113.GR.pdf)

### Terminais

#### Principal

| GPIO | Função | Periférico |
|:----:|:------:|:----------:|
| 23 | TXD | MAX485 - DI |
| 22 | RXD | MAX485 - RO |
| 18 | RTS | MAX485 - DE/RE |
| 27 | TXD | SIM7070G - RX |
| 26 | RXD | SIM7070G - TX |
| 36 | RTS | SIM7070G INT |
| 4 | Digital c/ interrupção | Sensor de Fluxo |

#### Auxiliar

| GPIO | Função | Periférico |
|:----:|:------:|:----------:|
| 23 | TXD | MAX485 - DI |
| 22 | RXD | MAX485 - RO |
| 18 | RTS | MAX485 - DE/RE |
| 4 | Digital c/ interrupção | Sensor de Fluxo |

## Dependências

* [platformio];
* [pandoc]
* [ssg5-pandoc] - para a geração da página web;

Este projeto foi testado e compilado no seguinte ambiente:

* Debian GNU/Linux 11 (bullseye)
* PlatformIO Core, version 5.1.1

```
PACKAGES:
 - framework-espidf 3.40200.210118 (4.2.0)
 - tool-cmake 3.16.4
 - tool-esptoolpy 1.30000.201119 (3.0.0)
 - tool-ninja 1.7.1
 - toolchain-esp32ulp 1.22851.191205 (2.28.51)
 - toolchain-xtensa32 2.80400.210211 (8.4.0)
```
## Funcionamento

Os hardwares devem ler o fluxo de água utilizando o sensor de fluxo YF-S201. 
Através das informações lidas pelo sensor será possível calcular quantos litros de água passaram pelo sensor.

O microcontrolador será responsável por ler todas estas informações e armazenar na memória flash para previnir se houver falta de luz.

Após um período de tempo (configurável) o dispositivo **MESTRE** deverá enviar uma requisição aos dispositivos **ESCRAVOS** através da interface **RS485** para que os mesmos retornem os valores do volume de água lidos.

As configurações de modo de operação (**MESTRE** ou **ESCRAVO**) e **ID** serão realizadas através de uma interface WEB.

### MESTRE e ESCRAVO

O dispositivo **MESTRE** é o dispositivo que possui o módulo **GPRS** [SIM7070G]. Módulo este para enviar os dados para o servidor.

O **MESTRE** é responsável por requisitar as leituras realizadas por todos os dispositivos **ESCRAVOS** na sua rede **RS485**.

O **MESTRE** também realiza a leitura de passagem de água.

Já o dispositivo **ESCRAVO** é responsável por realiza a leitura de passagem de água.

### Interface WEB

Sempre que o dispositivo for ligado, uma rede Wifi será acionada com o nome `hidrometro-digital` e a senha será `hidrometro`.

Após estabelecer conexão com a rede, é possível acessar a página de configurações do dispositivo através do IP `http://192.168.4.1`.

A página apresentada possui informações das configurações disponíveis e que podem ser realizadas na aba `Configurações`.

Na aba `Leituras` o usuário poderá acompanhar a leitura do sensor (litros de água) em tempo real.

### Interrupções

Toda vez que houver um fluxo de água, o sensor de fluxo irá girar a
roleta interna, ativando o sensor de efeito hall gerando uma interrupção
no terminal `GPIO4` do `ESP32`.

Quando a interrupção for gerada, o programa irá enviar esta informação
para uma fila, onde será incrementada uma variável de pulsos.

Haverá uma `task` rodando a cada segundo que irá pegar esta informção e
calcular quantos litros de água passou neste último segundo.

Mais informações podem ser encontradas [neste artigo](https://how2electronics.com/iot-water-flow-meter-using-esp8266-water-flow-sensor/).

## Utilização

Para compilar o projeto é necessário a instalação do [platformio], e após rodar o seguinte comando:

```sh
$ pio run
```

Para gerar a página web de configurações do dispositivo, rode:

```sh
make webpage
```

Para gravar o projeto no microcontrolador basta rodar:

```sh
$ pio run -t upload
```

Para gravar o arquivo de configuração e os arquivos da página de configuração, basta rodar:

```sh
$ pio run -t uploads
```

Se for necessário analisar o log serial do microcontrolador, rode:

```sh
$ pio device monitor --baud 115200 --port /dev/ttyUSB0 -f colorize
```

## Desenvolvedor

| ![](https://assets.gitlab-static.net/uploads/-/system/user/avatar/2382314/avatar.png?width=200) |
|:------:|
| [Edimar Calebe Castanho(calebe94@disroot.org)](https://gitlab.com/Calebe94) |

## Referências

* [Platformio: Development Platforms - Espressif 8266](https://docs.platformio.org/en/latest/platforms/espressif8266.html);
* [esp-idf: UART RS485 Echo Example](https://github.com/espressif/esp-idf/tree/master/examples/peripherals/uart/uart_echo_rs485);
* [esp_lte_modem](https://github.com/olliiiver/esp_lte_modem)

[platformio]: https://docs.platformio.org/en/latest/core/installation.html
[pandoc]: https://pandoc.org/
[ssg5-pandoc]: https://gitlab.com/Calebe94/ssg5-pandoc
