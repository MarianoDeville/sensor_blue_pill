/**
******************************************************************************
 * @file    app_config.h
 * @author  Lcdo. Mariano Ariel Deville
 * @brief   Manejo del almacenado y carga de la configuración del sistema
 *******************************************************************************
 * @attention
 *
 *******************************************************************************
 */
#ifndef INC_SETUP_H_
#define INC_SETUP_H_

/* Includes ------------------------------------------------------------------*/
#include <string.h>

/* Respuesta de las funciones ------------------------------------------------*/
typedef enum {

	CARGA_OK,
	CARGA_ERR,
	SETUP_FAIL,
	SETUP_SUCIFULL,
}sys_state_t;

/* Prototipo de funciones públicas -------------------------------------------*/
sys_state_t CargarConfig(void);
sys_state_t ModoSetup(void);

#endif /* INC_SETUP_H_ */
