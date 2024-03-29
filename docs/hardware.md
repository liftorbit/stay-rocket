# Documentação de hardware do foguete STAY

Esta documentação explica em detalhes o funcionamento e as conexões de sensores e servo motores do foguete. Para que o foguete consiga se conectar aos componentes, é necessário que todas as ligações estejam corretas.

Caso queira fazer uma sugestão de hardware, acesse o [arquivo CONTRIBUTING.md](https://github.com/liftorbit/stay-rocket/blob/master/CONTRIBUTING.md) para obter informações de como contribuir para o foguete STAY.

## Componentes

Os componentes utilizados no ESP32 estão totalmente de acordo com o software, ou seja, qualquer alteração no hardware afetará o funcionamento do foguete. Veja a lista dos componentes utilizados no STAY:

| Produto                   | Preço                    |
|---------------------------|--------------------------|
| ESP32                     | R$ 19,18                 |
| BMP280                    | R$ 2,12                  |
| BMI180                    | R$ 6,63                  |
| HC-12 SI4463              | R$ 11,68                 |
| USB-TO-TTL                | R$ 7,40                  |
| NEO6M (módulo GPS)        | R$ 13,57                 |
| Módulo cartão MicroSD     | R$ 2,40                  |
| Sensor infravermelho      | R$ 1,90                  |
| 02 Servos motores 9g      | R$ 11,30                 |
| Booster DC-DC MT3608      | R$ 2,18                  |
| Bateria 18650 6800mAh     | R$ 10,50                 |
| Suporte bateria 18650     | R$ 10,99                 |
| Placa de extensão ESP32   | R$ 19,13                 |
| 31 Jumpers Fêmea/Fêmea    | R$ --.--                 |
| **Sub-Total**             | **R$ 100,35**            |
| **Frete**                 | **R$ 100,23**            |
| **Total**                 | **R$ 200,58**            |

A maioria dos produtos listados abaixo foram comprados em sites estrangeiros. Além disso, nem todos os produtos foram adquiridos em **apenas um pedido**, por isso o valor do frete se torna um pouco alto, quase atingido o valor total dos produtos.

## Consumo energético

Todos os componentes do projeto consomem, se usados ao máximo, 1587.25 mA. Veja o **consumo máximo** de cada um dos componente utilizados no foguete:

| Dispositivo               | Consumo de Corrente (mA) |
|---------------------------|--------------------------|
| ESP32                     | 240                      |
| BMP280                    | 1                        |
| BMI180                    | 1,25                     |
| HC-12 SI4463              | 40                       |
| NEO6M (módulo GPS)        | 100                      |
| Módulo cartão MicroSD     | 200                      |
| Sensor infravermelho      | 5                        |
| 02 Servos motores 9g      | 1000 (1 A)               |
| **Total**                 | **1587,25 mA**           |

Para suprir todo esse consumo, uma bateria de 3,7 volts com 2000mah é necessária, dando uma margem de segurança de 400mah.

O ESP32 necessita de 5,0 volts para funcionar normalmente, para isso, um **impulsionador de tensão** DC-DC MT3608 será utilizado para aumentar a corrente de 3,7 para 5,0.

## Manual de conexão dos componentes

Para que o manual funcione corretamente, todos os componentes devem ser iguais aos listados na lista, evitando problemas de comunicação ou até danos ao componente.

> O pino `G26` do ESP32 é reservado para realizar a **ignição do motor principal**. A ignição deve ser feita usando um transistor.

### Componentes de alerta visual e sonoro

Um LED e ou buzzer são responsáveis por emitir alertas visuais e sonoros enquanto o foguete ainda estão no solo, permitindo acompanhar a sequência de status do foguete e monitorar erros. O pino `G25` será usado para um LED vermelho, enquanto o pino `G33` será conectado a um buzzer.

### BMP280 (barômetro)

O barômetro realiza medições de pressão, altitude e temperatura. O protocolo de comunicação utilizado é o SPI.

| BMP280 | ESP32 |
|--------|-------|
| VCC    | 3.3v  |
| SCL    | G13   |
| SDA    | G14   |
| CSB    | G27   |
| SDO    | G12   |

### BMI160 (acelerômetro/giroscópio)

O sensor BMI160 é uma unidade de medida inercial que fornece dados sobre orientação do foguete, usado principalmente para a manipulação do controle vetorial de empuxo. O protocolo de comunicação é I2C.

| BMI160 | ESP32 |
|--------|-------|
| VCC    | 3.3v  |
| SCL    | G22   |
| SDA    | G21   |

### Módulo cartão MicroSD

O módulo adaptador para cartão MicroSD é necessário para salvar registros importantes do foguete. O módulo utiliza o protocolo SPI.

| MicroSD | ESP32 |
|---------|-------|
| VCC     | 5.0v  |
| MISO    | G19   |
| MOSI    | G23   |
| SCK     | G18   |
| CS      | G5    |

### Servos motores 9g

Os servos motores são responsáveis por controlar o empuxo vetorial do foguete (TVC). Para isso, dois servos motores são utilizados.

| Servos (02) | ESP32 |
|-------------|-------|
| VCC         | 5.0v  |
| Servo 01 (X)| G2    |
| Servo 02 (Y)| G32   |

### Sensor infravermelho

O sensor infravermelho é responsável por informar ao computador de bordo se o motor está ligado ou desligado.

| Sensor infravermelho | ESP32 |
|----------------------|-------|
| VCC                  | 5.0v  |
| D0                   | G35   |

### GPS (neo-6m)

O módulo de GPS NEO6M fornece dados da localização do foguete após o desligamento do motor principal. O protocolo de comunicação utilizado é UART.

| GPS     | ESP32 |
|---------|-------|
| VCC     | 5.0v  |
| RX      | G15   |
| TX      | G04   |

### Módulo RF (HC12)

O módulo de Rádio Frequência HC12 transmite dados de telemetria do foguete durante o vôo, além de receber comandos para autorizações. O protocolo de comunicação utilizado é UART.

| HC12    | ESP32           |
|---------|-----------------|
| VCC     | 5.0v            |
| RX      | G17 (UART 2 TX) |
| TX      | G16 (UART 2 RX) |