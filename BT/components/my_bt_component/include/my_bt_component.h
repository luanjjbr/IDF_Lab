#ifndef MY_BT_COMPONENT_H
#define MY_BT_COMPONENT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Inicializa o Bluetooth com NVS, define o nome do dispositivo,
 * inicia o servidor SPP e configura o ambiente para envio e recepção de
 * mensagens.
 */
void bt_app_start(void);

/**
 * @brief Bloqueia até receber uma mensagem via SPP, imprime a mensagem e libera
 * a memória.
 */
void bt_get_message(void);

/**
 * @brief Bloqueia até receber uma mensagem via SPP e retorna o ponteiro para a
 * mensagem.
 *
 * @return char* Ponteiro para a mensagem recebida. O chamador é responsável por
 * liberar essa memória. Retorna NULL se nenhum dado for recebido.
 */
char *bt_get_message_return(void);

/**
 * @brief Envia uma mensagem via SPP.
 *
 * @param message String com a mensagem a ser enviada.
 */
void bt_set_message(const char *message);

#ifdef __cplusplus
}
#endif

#endif // MY_BT_COMPONENT_H
