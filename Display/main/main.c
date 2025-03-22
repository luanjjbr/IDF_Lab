#include "Test_display.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ssd1306.h"
#include <stdio.h>

extern SSD1306_t dev; // Objeto do display vindo do componente

// === Função principal ===
void app_main(void) {
	init_display(); // Inicializa o display

	// Uso normal do display
	ssd1306_clear_screen(&dev, false);
	ssd1306_display_text(&dev, 0, "Ola Mundo", 9, false);
	vTaskDelay(pdMS_TO_TICKS(2000));
	ssd1306_display_text_x3(&dev, 2, "ESP32", 5, false);
	ssd1306_hardware_scroll(&dev, SCROLL_RIGHT);
	vTaskDelay(pdMS_TO_TICKS(10000));
	ssd1306_hardware_scroll(&dev, SCROLL_STOP);

	xTaskCreate(test_display_task, "TestDisplayTask", 4096, NULL, 5, NULL);
}
