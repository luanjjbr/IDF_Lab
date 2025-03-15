#include "my_bt_component.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_err.h"
#include "esp_gap_bt_api.h"
#include "esp_spp_api.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Nomes para o dispositivo e para o servidor SPP
#define DEVICE_NAME "Meu_Nome_BT"
#define SPP_SERVER_NAME "SPP_SERVER"

// Fila para armazenar mensagens recebidas via BT
static QueueHandle_t bt_rx_queue = NULL;
// Handle da conexão SPP
static uint32_t spp_conn_handle = 0;

// Callback para eventos do SPP
static void spp_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
	switch (event) {
	case ESP_SPP_INIT_EVT:
		// Inicia o servidor SPP quando a inicialização do SPP for concluída
		esp_spp_start_srv(ESP_SPP_SEC_NONE, ESP_SPP_ROLE_SLAVE, 0,
						  SPP_SERVER_NAME);
		printf("Servidor SPP iniciado\n");
		break;
	case ESP_SPP_SRV_OPEN_EVT:
		// Conexão SPP estabelecida; armazena o handle da conexão
		spp_conn_handle = param->open.handle;
		printf("Conexão SPP aberta, handle: %lu\n",
			   (unsigned long)spp_conn_handle);
		break;
	case ESP_SPP_DATA_IND_EVT: {
		// Dados recebidos via SPP
		int len = param->data_ind.len;
		char *buf = malloc(len + 1);
		if (buf) {
			memcpy(buf, param->data_ind.data, len);
			buf[len] = '\0';
			// Envia a mensagem para a fila (se a fila estiver cheia, libera a
			// memória)
			if (xQueueSend(bt_rx_queue, &buf, 0) != pdPASS) {
				free(buf);
			}
		}
		break;
	}
	case ESP_SPP_CLOSE_EVT:
		printf("Conexão SPP encerrada\n");
		spp_conn_handle = 0;
		break;
	default:
		break;
	}
}

void bt_app_start(void) {
	esp_err_t ret;

	// Inicializa o NVS, necessário para a pilha BT
	ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
		ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ret = nvs_flash_erase();
		if (ret != ESP_OK) {
			printf("Erro ao apagar NVS: %s\n", esp_err_to_name(ret));
			return;
		}
		ret = nvs_flash_init();
	}
	if (ret != ESP_OK) {
		printf("Erro ao inicializar NVS: %s\n", esp_err_to_name(ret));
		return;
	}

	// Cria a fila para armazenar ponteiros para mensagens recebidas
	bt_rx_queue = xQueueCreate(10, sizeof(char *));
	if (bt_rx_queue == NULL) {
		printf("Erro ao criar a fila para mensagens BT\n");
		return;
	}

	// Configuração padrão do controlador BT
	esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

	// Inicializa o controlador BT
	ret = esp_bt_controller_init(&bt_cfg);
	if (ret != ESP_OK) {
		printf("Falha na inicialização do controlador BT: %s\n",
			   esp_err_to_name(ret));
		return;
	}

	// Ativa o controlador no modo Bluetooth clássico
	ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT);
	if (ret != ESP_OK) {
		printf("Falha ao habilitar o controlador BT: %s\n",
			   esp_err_to_name(ret));
		return;
	}

	// Inicializa a pilha Bluedroid
	ret = esp_bluedroid_init();
	if (ret != ESP_OK) {
		printf("Falha na inicialização do Bluedroid: %s\n",
			   esp_err_to_name(ret));
		return;
	}

	// Habilita o Bluedroid
	ret = esp_bluedroid_enable();
	if (ret != ESP_OK) {
		printf("Falha ao habilitar o Bluedroid: %s\n", esp_err_to_name(ret));
		return;
	}

	// Define o nome do dispositivo BT
	ret = esp_bt_gap_set_device_name(DEVICE_NAME);
	if (ret != ESP_OK) {
		printf("Falha ao definir o nome do dispositivo BT: %s\n",
			   esp_err_to_name(ret));
		return;
	}

	// Configura o dispositivo para ser conectável e discoverable
	ret = esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE,
								   ESP_BT_GENERAL_DISCOVERABLE);
	if (ret != ESP_OK) {
		printf("Falha ao definir o modo de descoberta BT: %s\n",
			   esp_err_to_name(ret));
		return;
	}

	// Registra o callback para os eventos do SPP
	ret = esp_spp_register_callback(spp_callback);
	if (ret != ESP_OK) {
		printf("Falha ao registrar callback do SPP: %s\n",
			   esp_err_to_name(ret));
		return;
	}
	// Cria uma configuração para o SPP e inicializa o SPP usando a função
	// enhanced
	esp_spp_cfg_t spp_cfg = {
		.mode = ESP_SPP_MODE_CB,
	};
	ret = esp_spp_enhanced_init(&spp_cfg);
	if (ret != ESP_OK) {
		printf("Falha ao inicializar o SPP: %s\n", esp_err_to_name(ret));
		return;
	}

	printf("Bluetooth e servidor SPP inicializados com sucesso\n");
}

void bt_get_message(void) {
	char *msg = NULL;
	// Bloqueia até receber uma mensagem da fila e imprime-a, liberando a
	// memória
	if (xQueueReceive(bt_rx_queue, &msg, portMAX_DELAY) == pdPASS) {
		printf("Recebido via BT: %s\n", msg);
		free(msg);
	}
}

char *bt_get_message_return(void) {
	char *msg = NULL;
	// Bloqueia até receber uma mensagem da fila e retorna o ponteiro para ela
	if (xQueueReceive(bt_rx_queue, &msg, portMAX_DELAY) == pdPASS) {
		return msg; // O chamador é responsável por liberar essa memória
	}
	return NULL;
}

void bt_set_message(const char *message) {
	if (spp_conn_handle != 0) {
		// Envia a mensagem via SPP
		esp_err_t ret =
			esp_spp_write(spp_conn_handle, strlen(message), (uint8_t *)message);
		if (ret == ESP_OK) {
			printf("Mensagem enviada via BT: %s\n", message);
		} else {
			printf("Erro ao enviar mensagem: %s\n", esp_err_to_name(ret));
		}
	} else {
		printf("Nenhuma conexão BT estabelecida para enviar mensagem\n");
	}
}
