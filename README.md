# IDF_Lab

Este repositório contém diversos projetos para ESP-IDF. O projeto **BT** demonstra o uso do Bluetooth clássico no ESP32, com comunicação via SPP (Serial Port Profile).

## Projeto BT

O projeto BT, localizado na pasta `BT`, exemplifica como:

- Inicializar o Bluetooth clássico com suporte a NVS (Non-Volatile Storage).
- Configurar o dispositivo para ser **conectável** e **discoverable**.
- Criar um servidor SPP que permite a comunicação Bluetooth.
- Implementar duas funções principais:
  - **bt_get_message()**: Espera e recebe uma mensagem via SPP, exibindo-a na saída serial USB.
  - **bt_set_message()**: Envia uma mensagem via SPP para um dispositivo conectado.

## Estrutura do Projeto BT

BT/ ├── components/ │ └── my_bt_component/ │ ├── CMakeLists.txt │ ├── my_bt_component.h │ └── my_bt_component.c ├── main/ │ ├── CMakeLists.txt │ └── main.c ├── sdkconfig.defaults └── .gitignore


- **components/my_bt_component/**  
  Contém o componente responsável pela inicialização do Bluetooth, configuração do SPP e implementação das funções de envio e recepção de mensagens.

- **main/**  
  Contém o código principal que chama o componente de BT e utiliza tarefas do FreeRTOS para gerenciar o envio e a recepção de mensagens.

- **sdkconfig.defaults**  
  Arquivo de configurações padrão (por exemplo, tamanho do flash, modo do BT) que pode ser sobrescrito via `idf.py menuconfig`.

- **.gitignore**  
  Lista arquivos e pastas que não devem ser versionados, como diretórios de build e arquivos temporários.

## Pré-Requisitos

- ESP-IDF (recomendado v5.4 ou superior)
- Ambiente de desenvolvimento configurado (Python, Git, etc.)
- ESP32 com suporte a Bluetooth clássico

## Configuração do Projeto

1. **Clone o repositório:**
   ```bash
   git clone <URL_DO_REPOSITORIO>
   cd IDF_Lab/BT
