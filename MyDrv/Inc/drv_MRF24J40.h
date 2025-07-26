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

#include "compatibility.h"

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
	NO_DIRECCION,
} mrf24_state_t;

/* Estructura con la información del dispositivo */
typedef struct {

	uint8_t sequence_number;
	uint8_t my_channel;
	uint8_t security_key[16];
	uint8_t my_mac[8];
	uint16_t my_panid;
	uint16_t my_address;
	uint16_t intervalo;
}mrf24_data_config_t;

/* Estructura con la información de transmisión ------------------------------*/
typedef struct {

	uint16_t dest_panid;
	uint16_t dest_address;
	uint16_t origin_address;
	char buffer[50];
}mrf24_data_out_t;

/* Estructura con la información de recepción --------------------------------*/
typedef struct {

	uint16_t source_panid;
	uint16_t source_address;
	uint8_t tamano_mensaje;
	uint8_t rssi;
	char buffer[50];
}mrf24_data_in_t;

/* Estructura con la lista de dispositivos cercanos --------------------------*/
typedef struct {

	uint8_t channel;
	uint16_t panid;
	uint8_t long_address[8];
	uint16_t short_address;
	uint8_t rssi;
} MRF24_discover_nearby_t;


/* Prototipo de funciones públicas -------------------------------------------*/
// Configuración del módulo
mrf24_state_t MRF24J40Init(void);
mrf24_data_config_t * MRF24GetConfig(void);

// Mensajes salientes
mrf24_state_t MRF24TransmitirDato(mrf24_data_out_t * p_info_out_s);

// Mensajes entrantes
volatile mrf24_state_t MRF24IsNewMsg(void);
mrf24_state_t MRF24ReciboPaquete(void);
mrf24_data_in_t * MRF24GetDataIn(void);

//
mrf24_state_t MRF24BuscarDispositivos(void);
mrf24_state_t MRF24TransmitirDatoEncriptado(void);

#endif /* INC_DRV_MRF24J40_H_ */
