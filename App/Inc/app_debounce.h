/**
******************************************************************************
 * @file    app_debounce.h
 * @author  Lcdo. Mariano Ariel Deville
 * @brief   Archivo cabecera para la implementación driver antirrebote
 *******************************************************************************
 * @attention
 *
 *******************************************************************************
 */

#ifndef API_INC_API_DEBOUNCE_H_
#define API_INC_API_DEBOUNCE_H_

#include "app_delay_unlock.h"

/**
 * Estados posibles de la máquina de estados.
 */
typedef enum {

	BUTTON_UP,
	BUTTON_FALLING,
	BUTTON_DOWN,
	BUTTON_RAISING,
} debounce_state_t;

/**
 * Respuestas de la máquina de estados.
 */
typedef enum {

	ERROR_ANTI_REBOTE,
	PRESIONO_BOTON,
	SUELTO_BOTON,
	BOTON_SIN_CAMBIOS,
	RUIDO,
} estado_pulsador_t;

/**
 * Estructura definida para el manejo del antirrebote.
 */
typedef struct {

	debounce_state_t estadoActual;
	delayNoBloqueanteData_t delay_anti_rebote;
} debounce_data_t;

/**
 * Prototipo de funciones públicas.
 */
void DebounceFSMInit(debounce_data_t * antirrebote_boton_n);
estado_pulsador_t DebounceFSMUpdate(debounce_data_t * antirrebote_boton_n, bool_t estado_pin);

#endif /* API_INC_API_DEBOUNCE_H_ */
