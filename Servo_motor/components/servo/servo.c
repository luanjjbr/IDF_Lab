#include "servo.h"

// Função para converter ângulo (0°-180°) para duty cycle do PWM
static uint32_t angle_to_duty(int angle) {
	// Garante que o ângulo esteja no intervalo correto
	if (angle < 0)
		angle = 0;
	if (angle > 180)
		angle = 180;

	// Converte o ângulo para tempo de pulso (dentro da faixa de 500-2500us)
	int pulse_width =
		SERVO_MIN_US + ((SERVO_MAX_US - SERVO_MIN_US) * angle) / 180;

	// Converte microssegundos para duty cycle (13 bits)
	return (pulse_width * 8192) / 20000;
}

// Inicializa o PWM para controlar o servo
void servo_init(void) {
	// Configuração do timer PWM
	ledc_timer_config_t pwm_timer = {
		.speed_mode = LEDC_LOW_SPEED_MODE,	  // Modo de baixa velocidade
		.timer_num = LEDC_TIMER_0,			  // Usa o timer 0
		.duty_resolution = LEDC_TIMER_13_BIT, // Resolução de 13 bits (0-8191)
		.freq_hz = SERVO_FREQ,				  // Frequência do PWM: 50Hz
		.clk_cfg = LEDC_AUTO_CLK // Configuração automática do clock
	};
	ledc_timer_config(&pwm_timer);

	// Configuração do canal PWM para o servo
	ledc_channel_config_t pwm_ch = {
		.speed_mode = LEDC_LOW_SPEED_MODE, // Modo de baixa velocidade
		.channel = LEDC_CHANNEL_0,		   // Usa o canal 0
		.timer_sel = LEDC_TIMER_0,		   // Associa ao timer 0
		.intr_type = LEDC_INTR_DISABLE,	   // Interrupção desativada
		.gpio_num = SERVO_PIN,			   // Pino onde o servo está conectado
		.duty = angle_to_duty(90),		   // Define ângulo inicial como 90°
		.hpoint = 0						   // Sem atraso na geração do PWM
	};
	ledc_channel_config(&pwm_ch);
}

// Move o servo para um ângulo específico (0° - 180°)
void set_servo_angle(int angle) {
	// Configura o novo duty cycle baseado no ângulo desejado
	ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, angle_to_duty(angle));
	ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}
