/**
******************************************************************************
 * @file    app_config.c
 * @author  Lcdo. Mariano Ariel Deville
 * @brief   Manejo del almacenado y carga de la configuración del sistema
 *******************************************************************************
 * @attention
 *
 *******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <app_config.h>
#include <string.h>
#include <stdio.h>
#include "app_principal.h"
#include "drv_MRF24J40.h"
#include "app_leds.h"
#include "app_debounce.h"
#include "app_delay_unlock.h"
#include "app_eeprom.h"

/* Definiciones de la configuración por defecto ------------------------------*/
#define SETUP_TIME_MS	5000
#define CICLE_TIME_MS	500
#define CICLE_TIME_OUT	24000
#define CANT_PAR_MRF	5

/* Variables privadas --------------------------------------------------------*/
static 	system_config_t system_config_s = {0};

/* Prototipo de funciones privadas -------------------------------------------*/
static sys_state_t GetEEPROMtoMRF24Config(void);

/* Funciones públicas --------------------------------------------------------*/
/**
 * @brief  Recupero de la memoria no volatil la configuración de red y del sistema
 * @param  None.
 * @retval Estado de la carga (CARGA_OK, CARGA_ERR).
 * @note
 */
sys_state_t CargarConfig(void) {

	if(EEPROMLoadStruct(EEPROM_CONFIG_ADDR,
						&system_config_s,
						sizeof(system_config_t)) != LOAD_OK)
		return CARGA_ERR;

	if(GetEEPROMtoMRF24Config() != CARGA_OK)
		return CARGA_ERR;

	if(MRF24J40Init() != INITIALIZATION_OK)
		return CARGA_ERR;
	return CARGA_OK;
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief
 * @param
 * @param
 * @retval
 * @note
 */
sys_state_t ModoSetup(void) {

	int32_t tiempo = HAL_GetTick();
	bool_t config_enable = false;

	while(!HAL_GPIO_ReadPin(BTN_GPIO_Port, BTN_Pin)) {

		if((HAL_GetTick() - tiempo) >= SETUP_TIME_MS) {

			config_enable = true;

			for(uint8_t i = 0; i < 6; i++) {

				ToggleLed(ROJO);
				ToggleLed(VERDE);
				delay_t(500);
			}
			break;
		}
	}
	uint8_t ciclos = 0;
	delayNoBloqueanteData_t delay_time;
	DelayInit(&delay_time, CICLE_TIME_MS);
	mrf24_data_in_t * mrf24_data_in;

	while(config_enable) {

		if(DelayRead(&delay_time)) {

			ToggleLed(ROJO);
			ciclos ++;
		}

		if(ciclos > CICLE_TIME_OUT)
			return SETUP_FAIL;

		// Acá recibo por IEEE 802.15.2 la configuración del sistema y de la red
		//
		if(MRF24IsNewMsg() == MSG_PRESENT) {

			if(MRF24ReciboPaquete() == MSG_READ) {

				mrf24_data_in = MRF24GetDataIn();
				char *palabra = strtok((char *)mrf24_data_in->buffer, ":");

/**
 * mejor recibo una estructura, la primer parte del mensaje me dice que estructura recibo
 * Comandos a recibir:
 * 						MRFCNF:CH(1 BYTE),PANID(2 BYTE),ADDRESS(2 BYTE),INTERVALO(2 BYTE)
 * 						MACCNF:LONG MAC ADDRESS (8 BYTES)
 * 						SECCNF:SECURITY KEY (16 BYTES)
 * 						DESIG:PLAYA,ZONA,BOX,PISO
 * Puedo mandar datos binarios en cambio de ascii, por ejemplo MRFCNF puede ser 0xFF0100
 * 	FF: comando
 * 	01: número de comando
 *	0: cantidad de variables a enviar (máximo 16)
 *	0: (bit menos significativo) tamaño de las variables en bytes (máximo 4 bytes)
 *
 *	0xFF0281: sería como enviar MACCNF con 8 datos de un byte y así sucesivamente.
 *
 * Para envío de datos sería 0xF00001
 *
 */
				if(!strcmp(palabra, "MRFCNF")) {

// Acá leo cada parámetro y lo guardo
// descarto la primer parte del mensaje porque es el comando y nesecito solo la parte de la estructura

					mrf24_data_config_t data_config_recibida = {0};

					uint8_t tamaño = strlen("MRFCNF:");

					memcpy(&data_config_recibida, mrf24_data_in->buffer + tamaño, mrf24_data_in->buffer_size - tamaño);

					data_config_recibida.intervalo = 4;

				}


			}


		}
		//
		// Después de de terminar de recibirla bajo la bandera




	}


	return SETUP_SUCIFULL;



	// Inicializo la red con la nueva configuración y el sistema
	// Cargo de la memoria no volatil la configuración del sistema.
	if(EEPROMLoadStruct(EEPROM_CONFIG_ADDR,
						&system_config_s,
						sizeof(system_config_t)) != LOAD_OK)
		return SETUP_FAIL;

	// Cargo de la memoria no volatil la configuración de red e inicializo el módulo.
	if(GetEEPROMtoMRF24Config() != CARGA_OK)
		return SETUP_FAIL;

	if(MRF24J40Init() != INITIALIZATION_OK)
		return SETUP_FAIL;


	for(uint8_t i = 0; i < 6; i++) {

		ToggleLed(VERDE);
		delay_t(500);
	}



	return SETUP_SUCIFULL;
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////














/* Funciones privadas --------------------------------------------------------*/
/**
 * @brief	Recupero de la memoria no volatil la configuración de red.
 * @param	None.
 * @retval	None.
 * @note
 */
static sys_state_t GetEEPROMtoMRF24Config(void) {

	mrf24_data_config_t mrf24_config_s;
	if(EEPROMLoadStruct(EEPROM_MRF24_ADDR, &mrf24_config_s, sizeof(mrf24_config_s)) == LOAD_OK) {

		if(mrf24_config_s.my_channel != 0xFF)
			memcpy(MRF24GetConfig(), &mrf24_config_s, sizeof(mrf24_config_s));
	} else {

		return CARGA_ERR;
	}
	return CARGA_OK;
}
