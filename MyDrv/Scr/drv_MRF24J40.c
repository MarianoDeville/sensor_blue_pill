/**
******************************************************************************
 * @file    drv_MRF24J40.c
 * @author  Lcdo. Mariano Ariel Deville
 * @brief   Implementación driver módulo MRF24J40
 *******************************************************************************
 * @attention Driver independiente de la plataforma de uso y del compilardor.
 *
 *******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "drv_MRF24J40.h"
#include "drv_MRF24J40_registers.h"
#include "drv_MRF24J40_port.h"
#include "app_delay_unlock.h"

/* Definiciones de la configuración por defecto ------------------------------*/
#define	DEFAULT_CHANNEL		CH_11
#define DEFAULT_SEC_NUMBER	(0X01)
#define	MY_DEFAULT_PAN_ID	(0x1234)
#define	MY_DEFAULT_ADDRESS	(0x1111)

#define HEAD_LENGTH			(0X0B)
#define WRITE_16_BITS		(0X8010)
#define READ_16_BITS		(0X8000)
#define WRITE_8_BITS		(0x01)
#define READ_8_BITS			(0x7E)
#define SHIFT_LONG_ADDR		(0X05)
#define SHIFT_SHORT_ADDR	(0X01)
#define SHIFT_BYTE			(0X08)
#define FCS_LQI_RSSI		(0x04)

/* Variables privadas --------------------------------------------------------*/
static mrf24_state_t estadoActual = INITIALIZATION_FAIL;
static mrf24_data_config_t data_config_s = {0};
static mrf24_data_out_t data_out_s = {0};
static mrf24_data_in_t data_in_s = {0};

/* MAC address por defecto del dispositivo */
static const uint8_t default_mac_address[] = {0x11,
                                              0x28,
                                              0x35,
                                              0x44,
                                              0x56,
                                              0x66,
                                              0x77,
                                              0x01};

/* Security key por defecto del dispositivo */
static const uint8_t default_security_key[] = {0x00,
                                               0x10,
                                               0x25,
                                               0x37,
                                               0x04,
                                               0x55,
                                               0x06,
                                               0x79,
                                               0x08,
                                               0x09,
                                               0x10,
                                               0x11,
                                               0x12,
                                               0x13,
                                               0x14,
                                               0x15};

/* Prototipo de funciones privadas -------------------------------------------*/
static void InicializoVariables(void);
static mrf24_state_t InicializoMRF24(void);
static void SetShortAddr(uint8_t reg_address, uint8_t valor);
static void SetLongAddr(uint16_t reg_address, uint8_t valor);
static uint8_t GetShortAddr(uint8_t reg_address);
static uint8_t GetLongAddr(uint16_t reg_address);
static void SetDeviceAddress(void);
static void SetChannel(void);
static void SetDeviceMACAddress(void);

/* Funciones privadas --------------------------------------------------------*/
/**
 * @brief  Inicialización de variables de configuración por defecto.
 * @param  None
 * @retval None
 */
static void InicializoVariables(void) {

	if(VACIO == data_config_s.my_address || VACIO == data_config_s.my_panid) {

		strncpy((char *)data_config_s.security_key,
				(char *) default_security_key,
				SEC_KEY_SIZE);
		strncpy((char *)data_config_s.my_mac,
				(char *) default_mac_address,
				LARGE_MAC_SIZE);
		data_config_s.sequence_number = DEFAULT_SEC_NUMBER;
		data_config_s.my_channel = DEFAULT_CHANNEL;
		data_config_s.my_panid = MY_DEFAULT_PAN_ID;
		data_config_s.my_address = MY_DEFAULT_ADDRESS;
	}
	return;
}

/**
 * @brief  Inicialización del módulo MRF24J40MA
 * @param  None
 * @retval Estado de la operación (TIME_OUT_OCURRIDO, INICIALIZACION_OK)
 */
static mrf24_state_t InicializoMRF24(void) {

	uint8_t lectura;
	delayNoBloqueanteData_t delay_time_out;
	DelayInit(&delay_time_out, MRF_TIME_OUT);
	SetShortAddr(SOFTRST, RSTPWR | RSTBB | RSTMAC);
	DelayReset(&delay_time_out);

	do {

		lectura = GetShortAddr(SOFTRST);
		if(DelayRead(&delay_time_out))
			return TIME_OUT_OCURRED;
	}while(VACIO != (lectura & (RSTPWR | RSTBB | RSTMAC)));
	delay_t(WAIT_50_MS);
	SetShortAddr(RXFLUSH, RXFLUSH_RESET);
	SetDeviceAddress();
	SetDeviceMACAddress();
	SetLongAddr(RFCON1, VCOOPT1 | VCOOPT0);
	SetLongAddr(RFCON2, PLLEN);
	SetLongAddr(RFCON3, P20dBm | P0dBm);
	SetLongAddr(RFCON6, TXFIL | _20MRECVR);
	SetLongAddr(RFCON7, SLPCLK100KHZ);
	SetLongAddr(RFCON8, RFVCO);
	SetLongAddr(SLPCON1, CLKOUTDIS | SLPCLKDIV0);
	SetShortAddr(BBREG2, CCA_MODE_1);
	SetShortAddr(BBREG6, RSSIMODE2);
	SetShortAddr(CCAEDTH, CCAEDTH2 | CCAEDTH1);
	SetShortAddr(PACON2, FIFOEN | TXONTS2 | TXONTS1);
	SetShortAddr(TXSTBL, RFSTBL3 | RFSTBL0 | MSIFS2 | MSIFS0);
	DelayReset(&delay_time_out);

	do {

		lectura = GetLongAddr(RFSTATE) & RX;
		if(DelayRead(&delay_time_out))
			return TIME_OUT_OCURRED;
	}while(RX != lectura);
	SetShortAddr(MRFINTCON, SLPIE_DIS | WAKEIE_DIS | HSYMTMRIE_DIS | SECIE_DIS
					| TXG2IE_DIS | TXNIE_DIS);
	SetShortAddr(ACKTMOUT, DRPACK | MAWD5 | MAWD4 | MAWD3 | MAWD0);
	SetChannel();
	SetShortAddr(RXMCR, VACIO);
	(void)GetShortAddr(INTSTAT);
	return INITIALIZATION_OK;
}

/**
 * @brief  Escribo en registro de 1 byte un dato de 1 byte
 * @param  Dirección del registro - 1 byte
 * @param  Dato - 1 byte
 * @retval None
 * @note   Al escribir direcciones cortas (SHORT ADDRESS REGISTER) se comienza
 *         con el MSB en 0 indicando una dirección corta, 6 bits con la
 *         dirección del registro, y 1 bit indicando la lectura o escritura.
 */
static void SetShortAddr(uint8_t reg_address, uint8_t valor) {

	reg_address = (uint8_t) (reg_address << SHIFT_SHORT_ADDR) | WRITE_8_BITS;
	SetCSPin(DISABLE);
	WriteByteSPIPort(reg_address);
	WriteByteSPIPort(valor);
	SetCSPin(ENABLE);
	return;
}

/**
 * @brief  Leo en registro de 1 byte un dato de 1 byte
 * @param  Dirección del registro - 1 byte
 * @param  Dato - 1 byte
 * @retval Valor devuelto por el módulo - 1 byte
 * @note   Al escribir direcciones cortas (SHORT ADDRESS REGISTER) se comienza
 *         con el MSB en 0 indicando una dirección corta, 6 bits con la
 *         dirección del registro, y 1 bit indicando la lectura o escritura.
 */
static uint8_t GetShortAddr(uint8_t reg_address) {

	uint8_t leido_spi = VACIO;
	reg_address = (uint8_t) (reg_address << SHIFT_SHORT_ADDR) & READ_8_BITS;
	SetCSPin(DISABLE);
	WriteByteSPIPort(reg_address);
	leido_spi = ReadByteSPIPort();
	SetCSPin(ENABLE);
	return leido_spi;
}

/**
 * @brief  Escribo de en registro de 2 bytes un dato de 1 byte
 * @param  Dirección del registro - 2 bytes
 * @param  Dato - 1 byte
 * @retval None
 * @note   Al escribir direcciones largas (LONG ADDRESS REGISTER) se comienza
 *         con el MSB en 1 indicando una dirección larga, 10 bits con la
 *         dirección del registro, y 1 bit indicando la lectura o escritura. En
 *         los 4 bits restantes (LSB) no importa el valor.
 */
static void SetLongAddr(uint16_t reg_address, uint8_t valor) {

	reg_address = (reg_address << SHIFT_LONG_ADDR) | WRITE_16_BITS;
	SetCSPin(DISABLE);
	Write2ByteSPIPort(reg_address);
	WriteByteSPIPort(valor);
	SetCSPin(ENABLE);
	return;
}

/**
 * @brief  Leo en registro de 2 bytes un dato de 1 byte
 * @param  Dirección del registro - 2 bytes
 * @param  Dato - 1 byte
 * @retval Valor devuelto por el módulo - 1 byte
 * @note   Al escribir direcciones largas (LONG ADDRESS REGISTER) se comienza
 *         con el MSB en 1 indicando una dirección larga, 10 bits con la
 *         dirección del registro, y 1 bit indicando la lectura o escritura. En
 *         los 4 bits restantes (LSB) no importa el valor.
 */
static uint8_t GetLongAddr(uint16_t reg_address) {

	uint8_t respuesta;
	reg_address = (reg_address << SHIFT_LONG_ADDR) | READ_16_BITS;
	SetCSPin(DISABLE);
	Write2ByteSPIPort(reg_address);
	respuesta = ReadByteSPIPort();
	SetCSPin(ENABLE);
	return respuesta;
}

/**
 * @brief  Seteo en el módulo en canal guardado en mrf24_data_config
 * @param  None
 * @retval None
 */
static void SetChannel(void) {

	SetLongAddr(RFCON0, data_config_s.my_channel);
	SetShortAddr(RFCTL, RFRST_HOLD);
	SetShortAddr(RFCTL, VACIO);
	delay_t(WAIT_1_MS);
	return;
}

/**
 * @brief  Seteo en el módulo la dirección corta guardada en mrf24_data_config
 * @param  None
 * @retval None
 */
static void SetDeviceAddress(void) {

	SetShortAddr(SADRH, (uint8_t) (data_config_s.my_address >> SHIFT_BYTE));
	SetShortAddr(SADRL, (uint8_t) (data_config_s.my_address));
	SetShortAddr(PANIDH, (uint8_t) (data_config_s.my_panid >> SHIFT_BYTE));
	SetShortAddr(PANIDL, (uint8_t) (data_config_s.my_panid));
	return;
}

/**
 * @brief  Seteo en el módulo la dirección mac guardada en mrf24_data_config.
 * @param  None.
 * @retval None.
 */
static void SetDeviceMACAddress(void) {

	for(uint8_t i = 0; i < LARGE_MAC_SIZE; i++) {

		SetShortAddr(EADR0 + i, data_config_s.my_mac[i]);
	}
	return;
}

/* Funciones públicas --------------------------------------------------------*/
/**
 * @brief  Inicialización del módulo MRF24J40MA.
 * @param  None.
 * @retval Estado de la operación (TIME_OUT_OCURRIDO, INICIALIZACION_OK).
 */
mrf24_state_t MRF24J40Init(void) {

	InicializoVariables();
	InicializoPines();
	delay_t(WAIT_1_MS);
	SetResetPin(1);
	delay_t(WAIT_1_MS);
	estadoActual = InicializoMRF24();
	return estadoActual;
}

/**
 * @brief   Devuelvo el puntero a la estructura que contiene la información de
 * 			configuración del módulo MRF24J40.
 * @param   None.
 * @retval  Puntero a la estructura tipo mrf24_data_config_t.
 */
mrf24_data_config_t * MRF24GetConfig(void) {

	return &data_config_s;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief
 * @param
 * @retval
 */
mrf24_state_t MRF24SetChannel(channel_list_t ch) {

	if(VACIO == ch)
		return INVALID_VALUE;
	data_config_s.my_channel = ch;
	return OPERATION_OK;
}
//***********************************************************************************************************************************
/**
 * @brief
 * @param
 * @retval
 */
mrf24_state_t MRF24SetPanId(uint16_t pan_id) {

	if(VACIO == pan_id)
		return INVALID_VALUE;
	data_config_s.my_panid = pan_id;
	return OPERATION_OK;
}
//***********************************************************************************************************************************
/**
 * @brief
 * @param
 * @retval
 */
mrf24_state_t MRF24SetAdd(uint16_t add) {

	if(VACIO == add)
		return INVALID_VALUE;
	data_config_s.my_address = add;
	return OPERATION_OK;
}
//***********************************************************************************************************************************
/**
 * @brief
 * @param
 * @retval
 */
mrf24_state_t MRF24SetInter(uint16_t sec) {




	return INITIALIZATION_OK;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief   Envío la información almacenada en la estructura de salida.
 * @param   None.
 * @retval  Estado de la operación (OPERACION_NO_REALIZADA,
 * 			TRANSMISION_REALIZADA, NO_DIRECCION, MSG_NO_PRESENTE).
 */
mrf24_state_t MRF24TransmitirDato(mrf24_data_out_t * p_info_out_s) {

	if(INITIALIZATION_OK != estadoActual)
		return OPERATION_FAIL;

	if(VACIO == p_info_out_s->dest_address)
		return DIRECTION_EMPTY;

	if(VACIO == p_info_out_s->buffer_size)
		return BUFFER_EMPTY;

	if(BUFFER_SIZE < p_info_out_s->buffer_size)
		return TO_LONG_MSG;
	uint8_t pos_mem = 0;
	SetLongAddr(pos_mem++, HEAD_LENGTH);
	SetLongAddr(pos_mem++, p_info_out_s->buffer_size + HEAD_LENGTH);
	SetLongAddr(pos_mem++, DATA | ACK_REQ | INTRA_PAN);			// LSB.
	SetLongAddr(pos_mem++, SHORT_S_ADD | SHORT_D_ADD);			// MSB.
	SetLongAddr(pos_mem++, data_config_s.sequence_number++);

	if(VACIO == p_info_out_s->dest_panid)
		p_info_out_s->dest_panid = data_config_s.my_panid;
	SetLongAddr(pos_mem++, (uint8_t) p_info_out_s->dest_panid);
	SetLongAddr(pos_mem++, (uint8_t) (p_info_out_s->dest_panid >> SHIFT_BYTE));
	SetLongAddr(pos_mem++, (uint8_t) p_info_out_s->dest_address);
	SetLongAddr(pos_mem++, (uint8_t) (p_info_out_s->dest_address >> SHIFT_BYTE));

	if(VACIO == p_info_out_s->origin_address)
		p_info_out_s->origin_address = data_config_s.my_address;
	SetLongAddr(pos_mem++, (uint8_t) p_info_out_s->origin_address);
	SetLongAddr(pos_mem++, (uint8_t) (p_info_out_s->origin_address >> SHIFT_BYTE));

	for(int8_t i = 0; i < p_info_out_s->buffer_size; i++) {

		SetLongAddr(pos_mem++, p_info_out_s->buffer[i]);
	}
	SetLongAddr(pos_mem++, VACIO);
	SetShortAddr(TXNCON, TXNACKREQ | TXNTRIG);
	return TRANSMISSION_COMPLETED;
}

/**
 * @brief   Se levantó la bandera indicando la llegada de un mensaje.
 * @param   None.
 * @retval  Estado de la operación (ERROR_INESPERADO, MSG_PRESENTE,
 *          MSG_NO_PRESENTE).
 */
volatile mrf24_state_t MRF24IsNewMsg(void) {

	if(INITIALIZATION_OK != estadoActual)
		return UNEXPECTED_ERROR;

	if(!IsMRF24Interrup())
		return MSG_PRESENT;
	return BUFFER_EMPTY;
}

/**
 * @brief   Recibir un paquete y dejarlo en el bufer de entrada en
 *          la estructura data_in_s.
 * @param   None.
 * @retval  Estado de la operación (OPERACION_NO_REALIZADA, MSG_LEIDO).
 */
mrf24_state_t MRF24ReciboPaquete(void) {
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	 * agregar la lectura del rssi y del lq, vienen en el paquete del mensaje.
	 */


	if(INITIALIZATION_OK != estadoActual)
		return OPERATION_FAIL;
	SetLongAddr(BBREG1, RXDECINV);
	SetShortAddr(RXFLUSH, DATAONLY);
	data_in_s.buffer_size = GetLongAddr(RX_FIFO);
	uint16_t add = GetLongAddr(RX_FIFO + 9);
	add = (add << SHIFT_BYTE) | GetLongAddr(RX_FIFO + 8);
	data_in_s.source_address = add;

	for(uint8_t i = 0; i < data_in_s.buffer_size - FCS_LQI_RSSI; i++) {

		data_in_s.buffer[i] = GetLongAddr(RX_FIFO + HEAD_LENGTH + i - 1);
	}
	SetLongAddr(BBREG1, VACIO);
	(void)GetShortAddr(INTSTAT);
	return MSG_READ;
}

/**
 * @brief   Devuelvo el puntero a la estructura que contiene la información del
 * 			mensaje de entrada.
 * @param   None.
 * @retval  Puntero a la estructura tipo mrf24_data_in_t.
 */
mrf24_data_in_t * MRF24GetDataIn(void) {

	return &data_in_s;
}






mrf24_state_t MRF24BuscarDispositivos(void) {

//   static MRF24_discover_nearby_t algo[10];

	if(INITIALIZATION_OK != estadoActual)
		return OPERATION_FAIL;


    data_out_s.dest_address = 1;


	SetShortAddr(TXNCON, TXNACKREQ | TXNTRIG);



	return MSG_READ;
}

