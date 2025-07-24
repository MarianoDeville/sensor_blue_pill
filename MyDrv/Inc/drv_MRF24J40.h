/**
 *******************************************************************************
 * @file    drv_MRF24J40.h
 * @author  Lcdo. Mariano Ariel Deville
 * @brief	Archivo cabecera para el archivo drv_MRF24J40.c
 *******************************************************************************
 * @attention Driver independiente de la plataforma de uso y del compilardor.
 *
 *******************************************************************************
 */
#ifndef INC_DRV_MRF24J40_H_
#define INC_DRV_MRF24J40_H_

#include "../../App/Inc/compatibility.h"

/* Macros --------------------------------------------------------------------*/
#define	BROADCAST		(0xFFFF)
#define MRF_TIME_OUT	200
#define ENABLE			true
#define	DISABLE			false
#define LARGE_MAC_SIZE	8
#define SEC_KEY_SIZE	16
#define WAIT_1_MS		1
#define WAIT_50_MS		50

/* Canales disponibles para el IEEE 802.15.4 ---------------------------------*/
typedef enum {

    CH_11 = 0x03,
    CH_12 = 0x13,
    CH_13 = 0x23,
	CH_14 = 0x33,
	CH_15 = 0x43,
	CH_16 = 0x53,
	CH_17 = 0x63,
	CH_18 = 0x73,
	CH_19 = 0x83,
	CH_20 = 0x93,
	CH_21 = 0xA3,
	CH_22 = 0xB3,
	CH_23 = 0xC3,
	CH_24 = 0xD3,
	CH_25 = 0xE3,
	CH_26 = 0xF3
} channel_list_t;

/* Respuesta de las funciones ------------------------------------------------*/
typedef enum {

	INICIALIZACION_OK,
	TRANSMISION_REALIZADA,
	MSG_PRESENTE,
	MSG_NO_PRESENTE,
	MSG_LEIDO,
	TIME_OUT_OCURRIDO,
	OPERACION_NO_REALIZADA,
	OPERACION_REALIZADA,
	ERROR_INESPERADO,
} MRF24_State_t;

/* Estructura con la lista de dispositivos cercanos --------------------------*/
typedef struct { uint8_t channel;
                 uint16_t panid;
                 uint8_t long_address[8];
                 uint16_t short_address;
                 uint8_t rssi;
} MRF24_discover_nearby_t;

/* Prototipo de funciones públicas -------------------------------------------*/
MRF24_State_t MRF24J40Init(void);
MRF24_State_t MRF24SetMensajeSalida(const char * mensaje);
MRF24_State_t MRF24SetDireccionDestino(uint16_t direccion);
MRF24_State_t MRF24SetPANIDDestino(uint16_t panid);
MRF24_State_t MRF24SetDireccionOrigen(uint16_t direccion);
MRF24_State_t MRF24TransmitirDato(void);
volatile MRF24_State_t MRF24IsNewMsg(void);
MRF24_State_t MRF24ReciboPaquete(void);
unsigned char * MRF24GetMensajeEntrada(void);
uint16_t MRF24GetMiPANID(void);
uint16_t MRF24GetMyAddr(void);


MRF24_State_t MRF24BuscarDispositivos(void);
MRF24_State_t MRF24TransmitirDatoEncriptado(void);
#endif /* INC_DRV_MRF24J40_H_ */
