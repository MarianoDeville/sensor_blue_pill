/**
******************************************************************************
 * @file    app_leds.c
 * @author  Lcdo. Mariano Ariel Deville
 * @brief   Manejo de leds.
 *******************************************************************************
 * @attention
 *
 *******************************************************************************
 */

#include "app_leds.h"


static info_leds_t led[3] = {{.puerto = LED_VERDE_GPIO_Port, .pin = LED_VERDE_Pin},
							 {.puerto = LED_AMARILLO_GPIO_Port, .pin = LED_AMARILLO_Pin},
							 {.puerto = LED_ROJO_GPIO_Port, .pin = LED_ROJO_Pin}};


void set_led(color_leds color, estado_leds_t estado) {

	HAL_GPIO_WritePin(led[color].puerto, led[color].pin, estado);
}

void toggle_led(color_leds color) {

	HAL_GPIO_TogglePin(led[color].puerto, led[color].pin);
}

estado_leds_t is_led(color_leds color) {

	return HAL_GPIO_ReadPin(led[color].puerto, led[color].pin);
}




