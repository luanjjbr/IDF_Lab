#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

void app_main(void) {
	while (true) {
		printf("Hello from app_main4!\n");
		sleep(1);
	}
}
