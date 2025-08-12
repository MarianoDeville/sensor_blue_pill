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
#include <app_config.h>
#include <string.h>
#include "app_principal.h"
#include "app_config.h"
#include "drv_MRF24J40.h"
#include "app_leds.h"
#include "app_debounce.h"
#include "app_delay_unlock.h"

/* Private define ------------------------------------------------------------*/
#define LOW_END_ADDR	(0x1112)

/* Variables privadas --------------------------------------------------------*/
static debounce_data_t boton1;

/* Prototipo de funciones privadas -------------------------------------------*/
static void CheckBoton(void);

void bucle(void) {
/*****************************************************************************************************************************
 * ACÁ PRUEBO COPIAR TODA LA INFO DE UNA ESTRUCTURA EN UNA CADENA
 * ****************************************************************************************************************************
 * ****************************************************************************************************************************
 * ****************************************************************************************************************************
 */
/*
mrf24_data_config_t data_config_s = {0};
data_config_s.my_channel = CH_12;
data_config_s.my_address = LOW_END_ADDR;
data_config_s.my_panid = 0x1234;
data_config_s.intervalo = 0xf7;
// LO ANTERIOR ES PARA TENER ALGO QUE COPIAR.
char buuf[sizeof(data_config_s) + 10] = {"MRFCNF:"};					// DECLARO UNA NUEVA CADENA DEL LARGO DE LA ESTRUCTURA
memcpy(buuf + strlen(buuf), &data_config_s, sizeof(data_config_s));	// AGREGO LA ESTRUCTURA EN LA CADENA
*/
/*****************************************************************************************************************************
 * ****************************************************************************************************************************
 * ****************************************************************************************************************************
 * ****************************************************************************************************************************
 * ****************************************************************************************************************************
 */



	SetLed(VERDE, LED_APAGADO);
	SetLed(AMARILLO, LED_APAGADO);
	SetLed(ROJO, LED_APAGADO);

	if(CargarConfig() != CARGA_OK)
		Error_Handler();

	if(ModoSetup() != SETUP_SUCIFULL)
		Error_Handler();
	DebounceFSMInit(&boton1);

	mrf24_data_in_t * mrf24_data_in;

	while(1) {

		CheckBoton();

		if(MRF24IsNewMsg() == MSG_PRESENT) {

			if(MRF24ReciboPaquete() == MSG_READ) {

				mrf24_data_in = MRF24GetDataIn();

				if(!strcmp(mrf24_data_in->buffer, "CMD:PLV"))
					SetLed(VERDE, LED_PRENDIDO);
				if(!strcmp(mrf24_data_in->buffer, "CMD:ALV"))
					SetLed(VERDE, LED_APAGADO);
			} else {

				ToggleLed(AMARILLO);
			}
		}
	}
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
			ToggleLed(ROJO);
			strcpy(data_out_s.buffer, "CMD:ALA"); // acá iría memcpy
			data_out_s.buffer_size = strlen("CMD:ALA");
			MRF24TransmitirDato(&data_out_s);
			break;
		case SUELTO_BOTON:
			ToggleLed(ROJO);
			strcpy(data_out_s.buffer, "CMD:PLA");
			data_out_s.buffer_size = strlen("CMD:PLA");
			MRF24TransmitirDato(&data_out_s);
			break;
		default:
	}
}

