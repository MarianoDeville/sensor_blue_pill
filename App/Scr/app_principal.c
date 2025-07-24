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
	MRF24SetDireccionDestino(LOW_END_ADDR);
	MRF24SetPANIDDestino(MRF24GetMiPANID());
	//MRF24SetDireccionDestino(BROADCAST);
	//MRF24SetPANIDDestino(0x1234);
	MRF24SetDireccionOrigen(MRF24GetMyAddr());

	while(1) {

		CheckBoton();

		if(MRF24IsNewMsg() == MSG_PRESENTE) {

			MRF24ReciboPaquete();

			unsigned char msg[50] = {0};
			strcpy((char*)msg, (const char*)MRF24GetMensajeEntrada());

			set_led(VERDE, LED_PRENDIDO);
		} else {

			set_led(VERDE, LED_APAGADO);
		}




	}
}



/**
  * @brief  Manejo de las pulsaciones del botón
  * @retval
  */
static void CheckBoton(void){

	switch(DebounceFSMUpdate(&boton1, HAL_GPIO_ReadPin(PULSADOR_GPIO_Port, PULSADOR_Pin))) {

		case PRESIONO_BOTON:

			toggle_led(AMARILLO);
			MRF24SetMensajeSalida("CMD:ALA");
			MRF24TransmitirDato();
			break;

		case SUELTO_BOTON:

			toggle_led(ROJO);
			MRF24SetMensajeSalida("CMD:PLA");
			MRF24TransmitirDato();
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
