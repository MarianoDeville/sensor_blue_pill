/**
*******************************************************************************
 * @file    app_config.c
 * @author  Lcdo. Mariano Ariel Deville
 * @brief   Manejo del almacenado y carga de la configuración del sistema
 ******************************************************************************
 * @attention En modo configuración la red carga la configuración por defecto.
 *			  Si no se pulsa el botón para entrar en modo configuración se
 *			  busca en la memória si existe una configuración guardada, si es
 *			  así se recupera y se utiliza como configuración del módulo.
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "app_config.h"
#include "app_principal.h"
#include "drv_MRF24J40.h"
#include "app_leds.h"
#include "app_debounce.h"
#include "app_delay_unlock.h"
#include "app_eeprom.h"

/* Definiciones de la configuración por defecto -----------------------------*/
#define SETUP_TIME_MS	5000
#define CICLE_TIME_MS	500
#define CICLE_TIME_OUT	24000
#define SIZE_COMMAND	7

/* Variables privadas -------------------------------------------------------*/
static system_config_t system_config_s = {0};

/* Prototipo de funciones privadas ------------------------------------------*/
static sys_state_t SetMRF24Config(mrf24_data_config_t * mrf_info_s);

/* Funciones públicas -------------------------------------------------------*/
/**
 * @brief	Si en el arranque del módulo se presiona y mantiene presionado por
 * 			10 segundos el botón de reset el módulo arranca en modo configuración
 * @param	None.
 * @retval	Estado de la operación (NO_SETUP, SETUP_FAIL y SETUP_SUCIFULL).
 * @note	Arranco con la configuración de fábrica para el módulo MRF24J40
 */
sys_state_t ModoSetup(void) {

	int32_t tiempo = HAL_GetTick();
	bool_t config_enable = false;

	while(HAL_GPIO_ReadPin(BTN_GPIO_Port, BTN_Pin)) {

		if((HAL_GetTick() - tiempo) >= SETUP_TIME_MS) {

			for(uint8_t i = 0; i < 6; i++) {

				ToggleLed(ROJO);
				ToggleLed(VERDE);
				delay_t(500);
			}
			config_enable = true;
			break;
		}
	}

	if(!config_enable)
		return NO_SETUP;

	if(INITIALIZATION_OK != MRF24J40Init())			// inicializo el mrf con configuración por defecto
		return SETUP_FAIL;
	uint8_t ciclos = 0;
	delayNoBloqueanteData_t delay_time;
	DelayInit(&delay_time, CICLE_TIME_MS);
	mrf24_data_in_t * mrf24_data_in;				// puntero para recibir el mensage de entrada
	mrf24_data_config_t data_config_recibida = {0}; // variable de configuración recibida para el mrf
	bool_t actualizar_mrf24 = false;
	bool_t actualizar_system_info = false;

	while(config_enable) {

		if(DelayRead(&delay_time)) {

			ToggleLed(ROJO);
			ciclos ++;
		}

		if(ciclos > CICLE_TIME_OUT) {

			SetLed(ROJO, LED_APAGADO);
			return SETUP_FAIL;
		}
		if(MSG_PRESENT == MRF24IsNewMsg()) {

			if(MSG_READ == MRF24ReciboPaquete()) {

				mrf24_data_in = MRF24GetDataIn();

				if(!memcmp(mrf24_data_in->buffer, "MRFCNF:", SIZE_COMMAND)) {

					memcpy(&data_config_recibida,
							mrf24_data_in->buffer + SIZE_COMMAND,
							sizeof(mrf24_data_config_t));
					actualizar_mrf24 = true;
				} else if(!memcmp(mrf24_data_in->buffer, "SYSCNF:", SIZE_COMMAND)) {

					memcpy(&system_config_s,
							mrf24_data_in->buffer + SIZE_COMMAND,
							sizeof(system_config_t));
					actualizar_system_info = true;
				} else if(!memcmp(mrf24_data_in->buffer, "ENDCNF:", SIZE_COMMAND)) {

					config_enable = false;
				}
			}
		}
	}
	SetLed(ROJO, LED_APAGADO);

	if(actualizar_mrf24) {

		if(CARGA_OK != SetMRF24Config(&data_config_recibida))
			return SETUP_FAIL;

		if(SAVE_OK != EEPROMSaveStruct(EEPROM_MRF24_ADDR,
										&data_config_recibida,
										sizeof(mrf24_data_config_t)))
			return SETUP_FAIL;
	}

	if(actualizar_system_info) {

		if(SAVE_OK != EEPROMSaveStruct(EEPROM_CONFIG_ADDR,
										&system_config_s,
										sizeof(system_config_t)))
			return SETUP_FAIL;
	}
    mrf24_data_out_t data_out_s;
	data_out_s.dest_address = BROADCAST;
	data_out_s.dest_panid = 0x9999;
	data_out_s.origin_address = 0x1111;
    data_out_s.buffer_size = strlen("Configuración guardada");
	strcpy(data_out_s.buffer, "Configuración guardada");
	MRF24TransmitirDato(&data_out_s);

	for(uint8_t i = 0; i < 6; i++) {

		ToggleLed(VERDE);
		delay_t(500);
	}
	return SETUP_SUCIFULL;
}

/**
 * @brief   Devuelvo el puntero al campo de la estructura que contiene el nombre
 *  de la playa.
 * @param   None.
 * @retval  Puntero a char.
 */
char * GetNombrePlaya(void) {

	return system_config_s.playa;
}

/**
 * @brief   Devuelvo el puntero al campo de la estructura que contiene el sector
 *  de la playa donde se encuentra el box.
 * @param   None.
 * @retval  Puntero a char.
 */
char * GetSectorPlaya(void) {

	return system_config_s.sector;
}

/**
 * @brief   Devuelvo el puntero al campo de la estructura que contiene la
 * identificación del box
 * @param   None.
 * @retval  Puntero a char.
 */
char * GetBoxPlaya(void) {

	return system_config_s.box;
}

/**
 * @brief   Devuelvo el puntero al campo de la estructura que contiene la
 * unbicación del box
 * @param   None.
 * @retval  Puntero a char.
 */
char * GetPisoPlaya(void) {

	return system_config_s.playa;
}

/**
 * @brief	Recupero de la memoria no volatil la configuración de red y del
 * 			sistema si estan almacenadas en ROM.
 * @param	None.
 * @retval	Estado de la carga (CARGA_OK, CARGA_ERR).
 * @note
 */
sys_state_t CargarConfig(void) {

	mrf24_data_config_t mrf24_config_s = {0};
	if(LOAD_OK == EEPROMLoadStruct(EEPROM_MRF24_ADDR, &mrf24_config_s, sizeof(mrf24_data_config_t))) {

		if(0xFF != mrf24_config_s.channel) {

			if(CARGA_ERR == SetMRF24Config(&mrf24_config_s))
				return CARGA_ERR;
		}
	}

	if(LOAD_OK != EEPROMLoadStruct(EEPROM_CONFIG_ADDR, &system_config_s, sizeof(system_config_t)))
			return CARGA_ERR;

	if(INITIALIZATION_OK != MRF24J40Init())
		return CARGA_ERR;
	return CARGA_OK;
}

/* Funciones privadas -------------------------------------------------------*/
/**
 * @brief	Actualizo la información de configuración en el driver MRF24J40
 * @param	Puntero a la estructura con la información
 * @retval	Estado de la operación (CARGA_OK, CARGA_ERR).
 * @note	Si alguno de los parámetros no son correctos se informa el error
 * 			para no continuar con el arranque.
 */
sys_state_t SetMRF24Config(mrf24_data_config_t * mrf_info_s) {

	if(OPERATION_OK != MRF24SetChannel(mrf_info_s->channel))
		return CARGA_ERR;

	if(OPERATION_OK != MRF24SetPanId(mrf_info_s->panid))
		return CARGA_ERR;

	if(OPERATION_OK != MRF24SetAdd(mrf_info_s->address))
		return CARGA_ERR;

	if(OPERATION_OK != MRF24SetSec(mrf_info_s->intervalo))
		return CARGA_ERR;

	if(OPERATION_OK != MRF24SetMAC(mrf_info_s->mac))
		return CARGA_ERR;

	if(OPERATION_OK != MRF24SetSecurityKey(mrf_info_s->security_key))
		return CARGA_ERR;
	return CARGA_OK;
}

