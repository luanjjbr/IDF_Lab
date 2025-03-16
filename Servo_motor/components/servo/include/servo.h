#ifndef SERVO_H
#define SERVO_H

#include "driver/ledc.h"

// Definições do pino e características do servo MG946R
#define SERVO_PIN GPIO_NUM_13 // Pino GPIO onde o servo está conectado
#define SERVO_MIN_US 500	  // Pulso mínimo em microssegundos (0°)
#define SERVO_MAX_US 2500	  // Pulso máximo em microssegundos (180°)
#define SERVO_FREQ 50		  // Frequência do PWM (50Hz - padrão para servos)

// Protótipos das funções
void servo_init(void);			 // Inicializa o PWM do servo
void set_servo_angle(int angle); // Define um ângulo para o servo (0° a 180°)

#endif
