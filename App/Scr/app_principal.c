/**
******************************************************************************
 * @file    app_principal.c
 * @author  Lcdo. Mariano Ariel Deville
 * @brief
 *******************************************************************************
 * @attention
 *
 *******************************************************************************
 */
#include <string.h>
#include <stdio.h>
#include "app_principal.h"
#include "drv_MRF24J40.h"
#include "app_leds.h"
#include "app_debounce.h"
#include "app_delay_unlock.h"
#include "app_eeprom.h"

/* Private define ------------------------------------------------------------*/
#define LOW_END_ADDR	(0x1112)
#define SETUP_TIME_SEG	120

/* Variables privadas --------------------------------------------------------*/
static debounce_data_t boton1;
static 	system_config_t system_config_s = {0};

/* Prototipo de funciones privadas -------------------------------------------*/
static void CheckBoton(void);
static void GetEEPROMtoMRF24Config(void);
static void ModoSetup(void);

void bucle(void) {

	set_led(VERDE, LED_APAGADO);
	set_led(AMARILLO, LED_APAGADO);
	set_led(ROJO, LED_APAGADO);

	// Cargo de la memoria no volatil la configuración del sistema.
	EEPROMLoadStruct(EEPROM_CONFIG_ADDR, &system_config_s, sizeof(system_config_t));

	// Cargo de la memoria no volatil la configuración de red e inicializo el módulo.
	GetEEPROMtoMRF24Config();
	if(MRF24J40Init() != INICIALIZACION_OK)
		Error_Handler();

	// Entro al modo configuración del sistema?
	ModoSetup();

	DebounceFSMInit(&boton1);

	mrf24_data_in_t * mrf24_data_in;

	while(1) {

		CheckBoton();

		if(MRF24IsNewMsg() == MSG_PRESENTE) {

			if(MRF24ReciboPaquete() == MSG_LEIDO) {

				mrf24_data_in = MRF24GetDataIn();

				if(!strcmp(mrf24_data_in->buffer, "CMD:PLV"))
					set_led(VERDE, LED_PRENDIDO);
				if(!strcmp(mrf24_data_in->buffer, "CMD:ALV"))
					set_led(VERDE, LED_APAGADO);
			} else {

				toggle_led(AMARILLO);
			}
		}
	}
}

/**
 * @brief	Recupero de la memória no volatil la configuración de red.
 * @param	None.
 * @retval	None.
 * @note
 */
static void GetEEPROMtoMRF24Config(void) {

	mrf24_data_config_t mrf24_config_s;
	if(EEPROMLoadStruct(EEPROM_MRF24_ADDR, &mrf24_config_s, sizeof(mrf24_config_s)) == LOAD_OK) {

		if(mrf24_config_s.my_channel != 0xFF)
			memcpy(MRF24GetConfig(), &mrf24_config_s, sizeof(mrf24_config_s));
	}
	return;
}

/**
 * @brief
 * @param
 * @param
 * @retval
 * @note
 */
static void ModoSetup(void) {

	int32_t tiempo = HAL_GetTick();
	bool_t config_enable = false;

	while(HAL_GPIO_ReadPin(BTN_GPIO_Port, BTN_Pin)) {

		if((HAL_GetTick() - tiempo) >= SETUP_TIME_SEG) {

			config_enable = true;
			break;
		}
	}
	delayNoBloqueanteData_t delay_time_out;
	DelayInit(&delay_time_out, MRF_TIME_OUT);
	mrf24_data_in_t * mrf24_data_in;

	while(config_enable) {

		if(DelayRead(&delay_time_out)) {

			toggle_led(ROJO);
			toggle_led(VERDE);
		}

		// Acá recibo por IEEE 802.15.2 la configuración del sistema y de la red
		//
		if(MRF24IsNewMsg() == MSG_PRESENTE) {

			if(MRF24ReciboPaquete() == MSG_LEIDO) {

				mrf24_data_in = MRF24GetDataIn();
			}
		}
		//
		// Después de de terminar de recibirla bajo la bandera




	}
	// Inicializo la red con la nueva configuración y el sistema
	// Cargo de la memoria no volatil la configuración del sistema.
	EEPROMLoadStruct(EEPROM_CONFIG_ADDR, &system_config_s, sizeof(system_config_t));

	// Cargo de la memoria no volatil la configuración de red e inicializo el módulo.
	GetEEPROMtoMRF24Config();
	if(MRF24J40Init() != INICIALIZACION_OK)
		Error_Handler();

	return;
}

/**
  * @brief  Manejo de las pulsaciones del botón
  * @retval
  */
static void CheckBoton(void) {

	mrf24_data_out_t data_out_s;
	data_out_s.dest_address = LOW_END_ADDR;
	data_out_s.dest_panid = 0x1234;
	data_out_s.origin_address = 0x1111;

	switch(DebounceFSMUpdate(&boton1, HAL_GPIO_ReadPin(BTN_GPIO_Port, BTN_Pin))) {

		case PRESIONO_BOTON:
			toggle_led(ROJO);
			strcpy(data_out_s.buffer, "CMD:ALA");
			MRF24TransmitirDato(&data_out_s);
			break;
		case SUELTO_BOTON:
			toggle_led(ROJO);
			strcpy(data_out_s.buffer, "CMD:PLA");
			MRF24TransmitirDato(&data_out_s);
			break;
		default:
	}
}

