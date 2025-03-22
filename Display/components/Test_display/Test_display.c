#include "Test_display.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "font8x8_basic.h"
#include "ssd1306.h"

#define TAG "SSD1306_DEMO"
#define DELAY_SHORT 1000 / portTICK_PERIOD_MS
#define DELAY_MEDIUM 3000 / portTICK_PERIOD_MS
#define DELAY_LONG 5000 / portTICK_PERIOD_MS

SSD1306_t dev;
int top, center, bottom;
char lineChar[20];

// === Inicialização do display ===
void init_display() {
#if CONFIG_I2C_INTERFACE
	ESP_LOGI(TAG, "Usando interface I2C");
	i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
#elif CONFIG_SPI_INTERFACE
	ESP_LOGI(TAG, "Usando interface SPI");
	spi_master_init(&dev, CONFIG_MOSI_GPIO, CONFIG_SCLK_GPIO, CONFIG_CS_GPIO,
					CONFIG_DC_GPIO, CONFIG_RESET_GPIO);
#endif

#if CONFIG_FLIP
	dev._flip = true;
	ESP_LOGW(TAG, "Display invertido");
#endif

#if CONFIG_SSD1306_128x64
	ssd1306_init(&dev, 128, 64);
	top = 2;
	center = 3;
	bottom = 8;
#elif CONFIG_SSD1306_128x32
	ssd1306_init(&dev, 128, 32);
	top = 1;
	center = 1;
	bottom = 4;
#endif

	ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0xFF);
}

// === Mensagem de boas-vindas ===
void show_welcome() {
	ssd1306_display_text_x3(&dev, 0, "Hello", 5, false);
	vTaskDelay(DELAY_MEDIUM);
}

// === Mostrar texto de teste ===
void show_test_text() {
#if CONFIG_SSD1306_128x64
	ssd1306_display_text(&dev, 0, "SSD1306 128x64", 14, false);
	ssd1306_display_text(&dev, 1, "ABCDEFGHIJKLMNOP", 16, false);
	ssd1306_display_text(&dev, 2, "abcdefghijklmnop", 16, false);
	ssd1306_display_text(&dev, 3, "Hello World!!", 13, false);
	ssd1306_display_text(&dev, 4, "SSD1306 128x64", 14, true);
	ssd1306_display_text(&dev, 5, "ABCDEFGHIJKLMNOP", 16, true);
	ssd1306_display_text(&dev, 6, "abcdefghijklmnop", 16, true);
	ssd1306_display_text(&dev, 7, "Hello World!!", 13, true);
#elif CONFIG_SSD1306_128x32
	ssd1306_display_text(&dev, 0, "SSD1306 128x32", 14, false);
	ssd1306_display_text(&dev, 1, "Hello World!!", 13, false);
	ssd1306_display_text(&dev, 2, "SSD1306 128x32", 14, true);
	ssd1306_display_text(&dev, 3, "Hello World!!", 13, true);
#endif
	vTaskDelay(DELAY_MEDIUM);
}

// === Contagem regressiva com fonte 8x8 ===
void show_countdown() {
	uint8_t image[24] = {0};
	for (int i = 0; i < 3; i++)
		ssd1306_display_image(&dev, top + i, (6 * 8 - 1), image, sizeof(image));

	for (int font = '9'; font > '0'; font--) {
		memset(image, 0, sizeof(image));
		memcpy(image, font8x8_basic_tr[font], 8);
		if (dev._flip)
			ssd1306_flip(image, 8);
		ssd1306_display_image(&dev, top + 1, (7 * 8 - 1), image, 8);
		vTaskDelay(DELAY_SHORT);
	}
}

// === Scroll vertical (para cima ou para baixo) ===
void scroll_vertical(bool down) {
	const char *direction = down ? "DOWN" : "UP";
	ssd1306_clear_screen(&dev, false);
	ssd1306_display_text(&dev, 0, down ? "--Scroll DOWN--" : "---Scroll  UP---",
						 16, true);
	ssd1306_software_scroll(&dev, down ? 1 : (dev._pages - 1),
							down ? (dev._pages - 1) : 1);

	for (int i = 0; i < bottom + 10; i++) {
		lineChar[0] = down ? 0x02 : 0x01;
		sprintf(&lineChar[1], " Line %02d", i);
		ssd1306_scroll_text(&dev, lineChar, strlen(lineChar), false);
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}

	vTaskDelay(DELAY_MEDIUM);
}

// === Paginação ===
void scroll_pagination() {
	ssd1306_clear_screen(&dev, false);
	ssd1306_display_text(&dev, 0, "---Page	DOWN---", 16, true);
	ssd1306_software_scroll(&dev, 1, (dev._pages - 1));

	for (int i = 0; i < bottom + 10; i++) {
		if ((i % (dev._pages - 1)) == 0)
			ssd1306_scroll_clear(&dev);

		lineChar[0] = 0x02;
		sprintf(&lineChar[1], " Line %02d", i);
		ssd1306_scroll_text(&dev, lineChar, strlen(lineChar), false);
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}

	vTaskDelay(DELAY_MEDIUM);
}

// === Scroll horizontal ===
void scroll_horizontal() {
	ssd1306_clear_screen(&dev, false);
	ssd1306_display_text(&dev, center, "Horizontal", 10, false);
	ssd1306_hardware_scroll(&dev, SCROLL_RIGHT);
	vTaskDelay(DELAY_LONG);
	ssd1306_hardware_scroll(&dev, SCROLL_LEFT);
	vTaskDelay(DELAY_LONG);
	ssd1306_hardware_scroll(&dev, SCROLL_STOP);
}

// === Scroll vertical com hardware ===
void scroll_vertical_hw() {
	ssd1306_clear_screen(&dev, false);
	ssd1306_display_text(&dev, center, "Vertical", 8, false);
	ssd1306_hardware_scroll(&dev, SCROLL_DOWN);
	vTaskDelay(DELAY_LONG);
	ssd1306_hardware_scroll(&dev, SCROLL_UP);
	vTaskDelay(DELAY_LONG);
	ssd1306_hardware_scroll(&dev, SCROLL_STOP);
}

// === Mensagem final ===
void show_goodbye() {
	ssd1306_clear_screen(&dev, true);
	ssd1306_display_text(&dev, center, "  Good Bye!!", 12, true);
	vTaskDelay(DELAY_LONG);
	ssd1306_fadeout(&dev);
}

// === Task principal do display ===
void test_display_task(void *pvParameters) {
	show_welcome();
	show_test_text();
	show_countdown();
	scroll_vertical(false);
	scroll_vertical(true);
	scroll_pagination();
	scroll_horizontal();
	scroll_vertical_hw();
	show_goodbye();
	esp_restart();
}
