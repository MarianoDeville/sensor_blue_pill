/**
******************************************************************************
 * @file    app_principal.c
 * @author  Lcdo. Mariano Ariel Deville
 * @brief   Implementación de delay no bloqueante.
 *******************************************************************************
 * @attention
 *
 *******************************************************************************
 */
#include "app_principal.h"
#include "app_delay_unlock.h"
#include "app_debounce.h"
#include "drv_MRF24J40.h"

/* Private define ------------------------------------------------------------*/
#define LOW_END_ADDR	(0x1112)

/* Variables privadas --------------------------------------------------------*/
static debounceData_t boton1;


/* Prototipo de funciones privadas -------------------------------------------*/
static void CheckBoton(void);

void bucle(void) {

	DebounceFSMInit(&boton1);

	if(MRF24J40Init() != INICIALIZACION_OK)
		Error_Handler();

	//MRF24SetDireccionDestino(LOW_END_ADDR);
	MRF24SetDireccionDestino(BROADCAST);
	//MRF24SetPANIDDestino(MRF24GetMiPANID());
	MRF24SetPANIDDestino(0xFFFF);

	while(1) {



		CheckBoton();

		HAL_GPIO_WritePin(LED_VERDE_GPIO_Port, LED_VERDE_Pin, MRF24IsNewMsg());
	}
}



/**
  * @brief  Manejo de las pulsaciones del botón
  * @retval
  */
static void CheckBoton(void){

	switch(DebounceFSMUpdate(&boton1, HAL_GPIO_ReadPin(PULSADOR_GPIO_Port, PULSADOR_Pin))) {

		case PRESIONO_BOTON:

			HAL_GPIO_TogglePin(LED_AMARILLO_GPIO_Port, LED_AMARILLO_Pin);
			MRF24SetMensajeSalida("CMD:PLA");
			MRF24TransmitirDato();
			break;

		case SUELTO_BOTON:

			HAL_GPIO_TogglePin(LED_ROJO_GPIO_Port, LED_ROJO_Pin);
//			MRF24SetMensajeSalida("CMD:ALA");
//			MRF24TransmitirDato();
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
