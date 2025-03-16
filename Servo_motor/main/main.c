#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "servo.h"
#include <stdio.h>

// Definição do pino do LED embutido no ESP32
#define LED_PIN GPIO_NUM_2

// Função para configurar o LED como saída
void led_init(void) { gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT); }

// Task que pisca o LED a cada 500ms
void blink_led(void *pvParameters) {
	while (true) {
		printf("Ligando LED!\n");
		gpio_set_level(LED_PIN, 1);		// Liga o LED
		vTaskDelay(pdMS_TO_TICKS(500)); // Espera 500ms

		printf("Desligando LED!\n");
		gpio_set_level(LED_PIN, 0);		// Desliga o LED
		vTaskDelay(pdMS_TO_TICKS(500)); // Espera mais 500ms
	}
}

// Task que movimenta o servo para diferentes ângulos
void servo_task(void *pvParameters) {
	while (true) {
		printf("Movendo servo para 0°\n");
		set_servo_angle(0);				 // Define o servo em 0°
		vTaskDelay(pdMS_TO_TICKS(1000)); // Espera 1 segundo

		printf("Movendo servo para 90°\n");
		set_servo_angle(90);			 // Define o servo em 90°
		vTaskDelay(pdMS_TO_TICKS(1000)); // Espera 1 segundo

		printf("Movendo servo para 180°\n");
		set_servo_angle(180);			 // Define o servo em 180°
		vTaskDelay(pdMS_TO_TICKS(1000)); // Espera 1 segundo

		printf("Movendo servo para 90°\n");
		set_servo_angle(90);			 // Define o servo em 90°
		vTaskDelay(pdMS_TO_TICKS(1000)); // Espera 1 segundo
	}
}

// Função principal
void app_main(void) {
	// Inicializa o servo e o LED
	servo_init();
	led_init();

	// Cria a tarefa para controlar o servo (prioridade 2)
	xTaskCreate(servo_task, "servo_task", 2048, NULL, 2, NULL);

	// Cria a tarefa para piscar o LED (prioridade 1)
	xTaskCreate(blink_led, "blink_led", 1024, NULL, 1, NULL);
}
