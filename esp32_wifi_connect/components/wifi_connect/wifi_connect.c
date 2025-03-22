#include "wifi_connect.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include <string.h>

static const char *TAG = "wifi_connect";
static wifi_connect_callback_t on_wifi_connected = NULL;

#define WIFI_CONNECTED_BIT BIT0

static EventGroupHandle_t wifi_event_group;

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
							   int32_t event_id, void *event_data) {
	if (event_base == WIFI_EVENT) {
		switch (event_id) {
		case WIFI_EVENT_STA_START:
			esp_wifi_connect();
			break;
		case WIFI_EVENT_STA_DISCONNECTED:
			ESP_LOGW(TAG, "Wi-Fi desconectado! Tentando reconectar...");
			esp_wifi_connect(); // Reconecta sem parar
			break;
		}
	} else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
		xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
	}
}

static void wifi_task(void *param) {
	const char *ssid = ((const char **)param)[0];
	const char *pass = ((const char **)param)[1];

	wifi_event_group = xEventGroupCreate();

	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_create_default_wifi_sta();

	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
											   &wifi_event_handler, NULL));
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
											   &wifi_event_handler, NULL));

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	wifi_config_t wifi_config = {0};
	strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
	strncpy((char *)wifi_config.sta.password, pass,
			sizeof(wifi_config.sta.password));
	wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
	wifi_config.sta.pmf_cfg.capable = true;
	wifi_config.sta.pmf_cfg.required = false;

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());

	ESP_LOGI(TAG, "Wi-Fi inicializado. Conectando...");

	EventBits_t bits = xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT,
										   pdFALSE, pdFALSE, portMAX_DELAY);

	if (bits & WIFI_CONNECTED_BIT) {
		ESP_LOGI(TAG, "Conectado com sucesso ao SSID: %s", ssid);
		if (on_wifi_connected) {
			on_wifi_connected();
		}
	}

	// Loop de monitoramento para reconexão contínua
	while (1) {
		if (!wifi_is_connected()) {
			ESP_LOGW(TAG, "Wi-Fi desconectado (loop)! Tentando reconectar...");
			esp_wifi_connect();
			xEventGroupClearBits(
				wifi_event_group,
				WIFI_CONNECTED_BIT); // Limpa flag para nova conexão
			xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE,
								pdFALSE, portMAX_DELAY);
			ESP_LOGI(TAG, "Reconectado ao Wi-Fi!");
			// Se quiser, pode chamar o callback de novo aqui
		}
		vTaskDelay(pdMS_TO_TICKS(5000));
	}
}

void wifi_connect_init(const char *ssid, const char *pass,
					   wifi_connect_callback_t cb) {
	static const char *params[2];
	params[0] = ssid;
	params[1] = pass;
	on_wifi_connected = cb;
	xTaskCreate(wifi_task, "wifi_task", 4096, (void *)params, 5, NULL);
}

bool wifi_is_connected(void) {
	wifi_ap_record_t ap_info;
	return (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK);
}
