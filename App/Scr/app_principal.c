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
#include "app_principal.h"
#include "drv_MRF24J40.h"
#include "app_leds.h"
#include "app_debounce.h"
#include "app_delay_unlock.h"

/* Private define ------------------------------------------------------------*/
#define LOW_END_ADDR	(0x1112)

/* Variables privadas --------------------------------------------------------*/
static debounceData_t boton1;

/* Prototipo de funciones privadas -------------------------------------------*/
static void CheckBoton(void);

void bucle(void) {

	DebounceFSMInit(&boton1);
	set_led(VERDE, LED_APAGADO);
	set_led(AMARILLO, LED_APAGADO);
	set_led(ROJO, LED_APAGADO);

	if(MRF24J40Init() != INICIALIZACION_OK)
		Error_Handler();
	mrf24_data_in_t * mrf24_data_in;

	while(1) {

		CheckBoton();

		if(MRF24IsNewMsg() == MSG_PRESENTE) {

			if(MRF24ReciboPaquete() == MSG_LEIDO) {

				mrf24_data_in = MRF24DataIn();
				char msg[50] = {0};
				strcpy((char*)msg, (const char*)mrf24_data_in->buffer);

				if(!strcmp(msg, "CMD:PLV"))
					set_led(VERDE, LED_PRENDIDO);
				if(!strcmp(msg, "CMD:ALV"))
					set_led(VERDE, LED_APAGADO);
			} else {

				toggle_led(AMARILLO);
			}
		}
	}
}

/**
  * @brief  Manejo de las pulsaciones del botón
  * @retval
  */
static void CheckBoton(void){

	mrf24_data_out_t data_out_s;
	data_out_s.dest_address = LOW_END_ADDR;
	data_out_s.dest_panid = MRF24GetMiPANID();
	data_out_s.origin_address = MRF24GetMyAddr();

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

		case RUIDO:
			break;

		case ERROR_ANTI_REBOTE:
			break;

		case BOTON_SIN_CAMBIOS:
			break;

		default:
	}
}
