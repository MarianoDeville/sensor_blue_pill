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
} delayNoBloqueanteData;

/**
 * Prototipo de funciones públicas.
 */
void DelayInit(delayNoBloqueanteData * delay, tick_t duration);
bool_t DelayRead(delayNoBloqueanteData * delay );
void DelayWrite(delayNoBloqueanteData * delay, tick_t duration);
void DelayReset( delayNoBloqueanteData * delay);

#endif /* API_INC_API_DELAY_H_ */
