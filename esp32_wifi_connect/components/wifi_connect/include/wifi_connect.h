#ifndef WIFI_CONNECT_H
#define WIFI_CONNECT_H

#include <stdbool.h>

typedef void (*wifi_connect_callback_t)(void);

void wifi_connect_init(const char *ssid, const char *pass,
					   wifi_connect_callback_t cb);
bool wifi_is_connected(void);

#endif // WIFI_CONNECT_H
