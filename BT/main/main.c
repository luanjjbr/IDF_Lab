#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "my_bt_component.h"
#include <stdio.h>

// Tarefa que aguarda e exibe mensagens recebidas via BT (imprime e libera)
void bt_get_task(void *param) {
	while (1) {
		bt_get_message();
	}
}

// Tarefa que envia periodicamente uma mensagem via BT
void bt_set_task(void *param) {
	const char *msg = "Olá, esta é uma mensagem via BT!\r\n";
	while (1) {
		bt_set_message(msg);
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
}

// Tarefa que aguarda e retorna uma mensagem via BT, e então imprime-a (exemplo
// de uso do retorno)
void bt_get_task_return(void *param) {
	while (1) {
		char *msg = bt_get_message_return();
		if (msg != NULL) {
			printf("Recebido via BT (retorno): %s\n", msg);
			free(msg);
		}
	}
}

void app_main(void) {
	// Inicializa o componente de BT
	bt_app_start();

	// Cria as tarefas para get (imprime e libera) e set
	xTaskCreate(bt_get_task, "BT_Get_Task", 4096, NULL, 5, NULL);
	xTaskCreate(bt_set_task, "BT_Set_Task", 4096, NULL, 5, NULL);

	// Opcional: Crie uma tarefa para usar a versão que retorna a mensagem
	xTaskCreate(bt_get_task_return, "BT_Get_Task_Return", 4096, NULL, 5, NULL);

	// Loop principal (as tarefas já estão em execução)
	while (1) {
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
