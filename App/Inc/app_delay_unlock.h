/**
******************************************************************************
 * @file    app_delay_unlock.h
 * @author  Lcdo. Mariano Ariel Deville
 * @brief   Implementación de delay no bloqueante.
 *******************************************************************************
 * @attention
 *
 *******************************************************************************
 */

#ifndef API_INC_API_DELAY_H_
#define API_INC_API_DELAY_H_

#include "compatibility.h"

typedef uint32_t tick_t;

/**
 * Estructura defina para el manejo del delay no bloqueante.
 * Está definida en el header para poder utilizarla en cualquier parte del proyecto.
 */
typedef struct{

	tick_t startTime;
	tick_t duration;
	bool_t running;
} delayNoBloqueanteData_t;

/**
 * Prototipo de funciones públicas.
 */
void DelayInit(delayNoBloqueanteData_t * delay, tick_t duration);
bool_t DelayRead(delayNoBloqueanteData_t * delay );
void DelayWrite(delayNoBloqueanteData_t * delay, tick_t duration);
void DelayReset( delayNoBloqueanteData_t * delay);
//uint16_t

#endif /* API_INC_API_DELAY_H_ */
