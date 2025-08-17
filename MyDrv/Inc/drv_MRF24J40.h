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
#define BUFFER_SIZE		128

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

	INITIALIZATION_FAIL,
	INITIALIZATION_OK,
	TRANSMISSION_COMPLETED,
	DIRECTION_EMPTY,
	MSG_PRESENT,
	MSG_READ,
	TO_LONG_MSG,
	BUFFER_EMPTY,
	TIME_OUT_OCURRED,
	OPERATION_FAIL,
	OPERATION_OK,
	UNEXPECTED_ERROR,
	INVALID_VALUE,
} mrf24_state_t;

/* Estructura con la información del dispositivo */
typedef struct {

	channel_list_t channel;
	uint8_t sequence_number;
    uint16_t panid;
	uint16_t address;
	uint16_t intervalo;
    uint8_t mac[8];
	uint8_t security_key[16];
}mrf24_data_config_t;

/* Estructura con la información de transmisión */
typedef struct {

	uint16_t dest_panid;
	uint16_t dest_address;
	uint16_t origin_address;
	char buffer[BUFFER_SIZE];
	uint8_t buffer_size;
}mrf24_data_out_t;

/* Estructura con la información de recepción */
typedef struct {

	uint16_t panid;
	uint16_t address;
	uint8_t rssi;
	uint8_t buffer[BUFFER_SIZE];
	uint8_t buffer_size;
}mrf24_data_in_t;

/* Estructura con la lista de dispositivos cercanos */
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
mrf24_state_t MRF24SetChannel(channel_list_t ch);
mrf24_state_t MRF24SetPanId(uint16_t pan_id);
mrf24_state_t MRF24SetAdd(uint16_t add);
mrf24_state_t MRF24SetInter(uint16_t sec);
mrf24_state_t MRF24SetMAC(uint8_t mac[8]);
mrf24_state_t MRF24SetSecurityKey(uint8_t security_key[16]);

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
