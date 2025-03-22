#include "esp_log.h"
#include "esp_sntp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "wifi_connect.h"
#include <sys/time.h>
#include <time.h>

static const char *TAG = "main";

void time_sync_cb(struct timeval *tv) {
	ESP_LOGI(TAG, "Tempo sincronizado via SNTP");
}

void initialize_sntp() {
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, "pool.ntp.org");
	sntp_set_time_sync_notification_cb(time_sync_cb);
	sntp_init();
}

void wait_for_time_sync() {
	time_t now = 0;
	struct tm timeinfo = {0};
	int retry = 0;
	const int retry_count = 10;

	while (timeinfo.tm_year < (2023 - 1900) && ++retry < retry_count) {
		ESP_LOGI(TAG, "Aguardando sincronização do tempo... (%d/%d)", retry,
				 retry_count);
		vTaskDelay(pdMS_TO_TICKS(2000));
		time(&now);
		localtime_r(&now, &timeinfo);
	}

	if (retry == retry_count) {
		ESP_LOGW(TAG, "Tempo não sincronizado");
	} else {
		ESP_LOGI(TAG, "Tempo sincronizado");
	}
}

void print_time_task(void *pvParameter) {
	while (1) {
		// Verifica se está conectado ao Wi-Fi
		if (wifi_is_connected()) {
			time_t now;
			struct tm timeinfo;
			time(&now);
			localtime_r(&now, &timeinfo);

			char strftime_buf[64];
			strftime(strftime_buf, sizeof(strftime_buf), "%d/%m/%Y %H:%M:%S",
					 &timeinfo);
			ESP_LOGI(TAG, "Hora atual: %s", strftime_buf);
		} else {
			ESP_LOGW(TAG, "Wi-Fi desconectado, não é possível mostrar hora!");
			vTaskDelay(pdMS_TO_TICKS(5000));
		}

		vTaskDelay(
			pdMS_TO_TICKS(5000)); // Delay fora do IF, para manter o intervalo
	}
}

void on_wifi_connected() {
	ESP_LOGI(TAG, "Wi-Fi conectado, iniciando SNTP...");
	initialize_sntp();
	wait_for_time_sync();
	xTaskCreate(&print_time_task, "print_time_task", 4096, NULL, 5, NULL);
}

void app_main(void) {
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
		ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	wifi_connect_init("Luan_Not", "12345678", on_wifi_connected);
}
