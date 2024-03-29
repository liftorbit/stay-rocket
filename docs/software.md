# Documentação do software do computador de bordo do foguete STAY

Esta documentação explica em detalhes o funcionamento do código que será utilizado no computador de bordo do foguete.

## Telemetria

A telemetria de dados é feita utilizando um módulo de rádio frequência de 433mhz, alcançando no máximo 1 quilômetro e 800 metros em área aberta. Existem duas etapas na telemetria do foguete, elas mudam de acordo com a etapa de lançamento:

- **Telemetria básica:** essa etapa da telemetria é iniciada após a *ignição do motor principal*, onde apenas dados básicos são enviados *10 vezes por segundo*, como:
  - Status do motor
  - Temperatura (Grau Celsius)
  - Aceleração (m/s²)
  - Altitude em relação ao chão (metros)
  - Pressão (Pa)
- **Telemetria avançada:** após o desligamento do motor principal (MECO), a telemetria avançada passa a ser transmitida a *1 vez por segundo*. Todos os dados básicos são enviados, porém com *dados de localização*, ou seja, latitude e longitude.

### Configuração do transceptor

Configurar o transceptor HC12 é necessário para aumentar a distância em que o sinal pode percorrer, garantido a telemetria do foguete em área aberta. As configurações são:

- Taxa de transmissão: Os BPS (bits por segundo) devem ser de `2400bps`. Na telemetria avançada, são transmitidos **200 bits por segundo**, já na telemetria básica, são **1360 bits por segundo**.
- Potência do modo: A potência do módulo deve ser de `20dBm` (ou P8 no HC12).

Execute cada um dos comandos abaixo no módulo utilizando uma conexão USB to TTL para configurar o transceptor:

```at
AT+B2400
AT+P8
```

## Etapas de lançamento

As etapas de lançamento são as ações que o foguete realiza antes do lançamento, durante o voo e pouso. Essas etapas incluem verificações, autorizações, e coleta de dados.

1. `SETUP`: Inicialização do sistema de log, sensores e comunicação.
2. `AUTHORIZATION`: Aguarda a autorização do lançamento do foguete.
3. `COUNTDOWN`: Quando autorizado, uma contagem de 10 segundos será iniciada internamente, nesse período, o Controle Vetorial de Empuxo é ativado. A contagem **pode ser interrompida** pela comunicação com o comando `SLC` (Stop Launch Countdown).
4. `LAUNCH`: Inicialização do motor, controle vetorial de empuxo e telemetria.
5. `MECO`: Desligamento do Motor Principal (Main Engine Cut Off).

### Etapa `SETUP`

Sendo a primeira etapa após a inicialização do foguete, ela é responsável por preparar e obter dados de teste dos sensores instalados, como barômetro, acelerômetro e infravermelho. Os testes e registros obtidos são enviados para a base e o computador de bordo aguarda a autorização de lançamento.

### Etapa `LAUNCH`

Todas as etapas são simples até a decolagem, onde é requerido um bom gerenciamento de tempo e de ordem na coleta de dados e controle do foguete. Nesta etapa, 03 coisas devem acontecer:

- Telemetria de dados
- Registro de ações realizadas pelo foguete (logging)
- Manipular o controle vetorial de empuxo enquanto o motor está ligado

A telemetria de dados e a manipulação do controle vetorial de empuxo devem ocorrer ao mesmo tempo, permitindo que os dois sistemas funcionem sem interrupções.

Após a ignição do motor, uma thread é criada para enviar dados 10 vezes por segundos de telemetria do foguete e a thread principal é utilizada para manipular o controle vetorial de empuxo.

### Etapa `MECO`

Após o desligamento do motor principal, o controle vetorial de empuxo é **desligado** e a telemetria passa a trabalhar na thread principal enviando dados (incluindo localização) *2 vezes por segundo*.

> Caso o sensor infravermelho que detecta o funcionamento do motor falhe, os dados de localização serão enviados se for detectado uma queda de altitude.