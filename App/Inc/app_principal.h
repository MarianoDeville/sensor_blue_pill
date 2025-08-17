/**
******************************************************************************
 * @file    app_principal.h
 * @author  Lcdo. Mariano Ariel Deville
 * @brief   Implementación de delay no bloqueante.
 *******************************************************************************
 * @attention
 *
 *******************************************************************************
 */

#ifndef INC_APP_PRINCIPAL_H_
#define INC_APP_PRINCIPAL_H_

#include "main.h"
#include "compatibility.h"

typedef struct {

	char playa[20];
	char sector[20];
	char box[10];
	char piso[10];
}system_config_t;

void bucle(void);

#endif /* INC_APP_PRINCIPAL_H_ */
