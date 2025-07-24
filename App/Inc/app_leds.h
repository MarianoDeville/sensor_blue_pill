/**
******************************************************************************
 * @file    app_leds.h
 * @author  Lcdo. Mariano Ariel Deville
 * @brief   Manejo de leds.
 *******************************************************************************
 * @attention
 *
 *******************************************************************************
 */

#ifndef INC_APP_LEDS_H_
#define INC_APP_LEDS_H_

#include "compatibility.h"

typedef enum {

	LED_PRENDIDO,
	LED_APAGADO,
} estado_leds_t;

typedef struct {

	GPIO_TypeDef *puerto;
	uint16_t pin;
} info_leds_t;

typedef enum {
	VERDE,
	AMARILLO,
	ROJO
} color_leds;

void set_led(color_leds color, estado_leds_t estado);
void toggle_led(color_leds color);
estado_leds_t is_led(color_leds color);

#endif /* INC_APP_LEDS_H_ */
