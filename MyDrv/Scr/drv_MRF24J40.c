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

#define HEAD_LENGTH			(0X08)
#define WRITE_16_BITS		(0X8010)
#define READ_16_BITS		(0X8000)
#define WRITE_8_BITS		(0x01)
#define READ_8_BITS			(0x7E)
#define SHIFT_LONG_ADDR		(0X05)
#define SHIFT_SHORT_ADDR	(0X01)
#define SHIFT_BYTE			(0X08)
#define FCS_LQI_RSSI		(0x04)

/* Variables privadas --------------------------------------------------------*/
static MRF24_State_t estadoActual;
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

/* Estructura con la información del dispositivo */
static struct {	bool_t get_new_msg;
				bool_t put_new_msg;
				uint8_t sequence_number;
				uint8_t my_channel;
				uint8_t security_key[16];
				uint8_t my_mac[8];
				uint16_t my_panid;
				uint16_t my_address;
				uint16_t intervalo;
}mrf24_data_config;

/* Estructura con la información de transmisión */
static struct {	uint16_t destinity_panid;
				uint16_t destinity_address;
				uint8_t largo_mensaje;
				const char * buffer_salida;
}mrf24_data_out;

/* Estructura con la información de recepción */
static struct {	uint16_t source_panid;
				uint16_t source_address;
				uint8_t tamano_mensaje;
				uint8_t rssi;
				uint8_t buffer_entrada[50];
}mrf24_data_in;

/* Prototipo de funciones privadas -------------------------------------------*/
static void InicializoVariables(void);
static MRF24_State_t InicializoMRF24(void);
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

	for(int i = 0; i < SEC_KEY_SIZE; i++){

		if(i < LARGE_MAC_SIZE)
			mrf24_data_config.my_mac[i] = default_mac_address[i];
		mrf24_data_config.security_key[i] = default_security_key[i];
	}
	mrf24_data_config.sequence_number = DEFAULT_SEC_NUMBER;
	mrf24_data_config.my_channel = DEFAULT_CHANNEL;
	mrf24_data_config.get_new_msg = false;
	mrf24_data_config.put_new_msg = false;
	mrf24_data_config.my_panid = MY_DEFAULT_PAN_ID;
	mrf24_data_config.my_address = MY_DEFAULT_ADDRESS;
	mrf24_data_in.source_address = VACIO;
	mrf24_data_in.source_panid = VACIO;
	mrf24_data_in.tamano_mensaje = VACIO;
	mrf24_data_in.buffer_entrada[0] = VACIO;
	mrf24_data_out.destinity_panid = VACIO;
	mrf24_data_out.destinity_address = VACIO;
	mrf24_data_out.largo_mensaje = VACIO;
	mrf24_data_out.buffer_salida = NULL;
	return;
}

/**
 * @brief  Inicialización del módulo MRF24J40MA
 * @param  None
 * @retval Estado de la operación (TIME_OUT_OCURRIDO, INICIALIZACION_OK)
 */
static MRF24_State_t InicializoMRF24(void) {

	uint8_t lectura;
	delayNoBloqueanteData delay_time_out;
	DelayInit(&delay_time_out, MRF_TIME_OUT);
	SetShortAddr(SOFTRST, RSTPWR | RSTBB | RSTMAC);
	DelayReset(&delay_time_out);

	do {

		lectura = GetShortAddr(SOFTRST);
		if(DelayRead(&delay_time_out))
			return TIME_OUT_OCURRIDO;
	}while((lectura & (RSTPWR | RSTBB | RSTMAC)) != VACIO);
	delay_t(WAIT_1_MS);
	SetShortAddr(RXFLUSH, RXFLUSH_RESET);
	SetDeviceAddress();
	SetDeviceMACAddress();
	SetLongAddr(RFCON2, PLLEN);
	SetLongAddr(RFCON3, P30dBm | P6_3dBm);
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

		lectura = GetLongAddr(RFSTATE);
		if(DelayRead(&delay_time_out))
			return TIME_OUT_OCURRIDO;
	}while(lectura != RX);
	SetShortAddr(MRFINTCON, SLPIE_DIS | WAKEIE_DIS | HSYMTMRIE_DIS | SECIE_DIS | TXG2IE_DIS | TXNIE_DIS);
	SetShortAddr(ACKTMOUT, DRPACK | MAWD5 | MAWD4 | MAWD3 | MAWD0);
	SetLongAddr(RFCON1, VCOOPT1 | VCOOPT0);
	SetChannel();
	SetShortAddr(RXMCR, VACIO);
	(void)GetShortAddr(INTSTAT);
	return INICIALIZACION_OK;
}

/**
 * @brief  Escribo en registro de 1 byte un dato de 1 byte
 * @param  Dirección del registro - 1 byte
 * @param  Dato - 1 byte
 * @retval None
 */
static void SetShortAddr(uint8_t reg_address, uint8_t valor) {

	// Al escribir direcciones cortas (SHORT ADDRESS REGISTER) se comienza con el MSB
	// en 0 indicando una dirección corta, 6 bits con la dirección del registro, y 1 bit
	// indicando la lectura o escritura.
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
 */
static uint8_t GetShortAddr(uint8_t reg_address) {

	// Al escribir direcciones cortas (SHORT ADDRESS REGISTER) se comienza con el MSB
	// en 0 indicando una dirección corta, 6 bits con la dirección del registro, y 1 bit
	// indicando la lectura o escritura.
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
 */
static void SetLongAddr(uint16_t reg_address, uint8_t valor) {

	// Al escribir direcciones largas (LONG ADDRESS REGISTER) se comienza con el MSB
	// en 1 indicando una dirección larga, 10 bits con la dirección del registro, y 1 bit
	// indicando la lectura o escritura. En los 4 bits restantes (LSB) no importa el valor.
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
 */
static uint8_t GetLongAddr(uint16_t reg_address) {

	// Al escribir direcciones largas (LONG ADDRESS REGISTER) se comienza con el MSB
	// en 1 indicando una dirección larga, 10 bits con la dirección del registro, y 1 bit
	// indicando la lectura o escritura. En los 4 bits restantes (LSB) no importa el valor.
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

	SetLongAddr(RFCON0, mrf24_data_config.my_channel);
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

	SetShortAddr(SADRH, (uint8_t) (mrf24_data_config.my_address >> SHIFT_BYTE));
	SetShortAddr(SADRL, (uint8_t) (mrf24_data_config.my_address));
	SetShortAddr(PANIDH, (uint8_t) (mrf24_data_config.my_panid >> SHIFT_BYTE));
	SetShortAddr(PANIDL, (uint8_t) (mrf24_data_config.my_panid));
	return;
}

/**
 * @brief  Seteo en el módulo la dirección mac guardada en mrf24_data_config.
 * @param  None.
 * @retval None.
 */
static void SetDeviceMACAddress(void) {

	long_mac_address_e = EADR0;

	for(int i = 0; i < 8; i++) {

		SetShortAddr(long_mac_address_e++, mrf24_data_config.my_mac[i]);
	}
	return;
}

/* Funciones públicas --------------------------------------------------------*/
/**
 * @brief  Inicialización del módulo MRF24J40MA.
 * @param  None.
 * @retval Estado de la operación (TIME_OUT_OCURRIDO, INICIALIZACION_OK).
 */
MRF24_State_t MRF24J40Init(void) {

	InicializoVariables();
	InicializoPines();
	delay_t(WAIT_1_MS);
	SetResetPin(1);
	delay_t(WAIT_1_MS);
	estadoActual = InicializoMRF24();
	return estadoActual;
}

/**
 * @brief   Paso por referencia la dirección del mensaje a enviar.
 * @param   Puntero al mensaje.
 * @retval  Estado de la operación (ERROR_INESPERADO, OPERACION_REALIZADA).
 */
MRF24_State_t MRF24SetMensajeSalida(const char * mensaje) {

	if(strlen(mensaje) == VACIO || estadoActual != INICIALIZACION_OK)
		return ERROR_INESPERADO;
	mrf24_data_out.buffer_salida = mensaje;
	mrf24_data_out.largo_mensaje = (uint8_t) strlen(mensaje);
	return OPERACION_REALIZADA;
}

/**
 * @brief   Configuro la dirección corta del dispositivo con el que me comunicaré.
 * @param   Dirección corta del dispositivo - 2 bytes.
 * @retval  Estado de la operación (OPERACION_NO_REALIZADA, OPERACION_REALIZADA).
 */
MRF24_State_t MRF24SetDireccionDestino(uint16_t direccion) {

	if(estadoActual != INICIALIZACION_OK)
		return OPERACION_NO_REALIZADA;
	mrf24_data_out.destinity_address = direccion;
	return OPERACION_REALIZADA;
}

/**
 * @brief   Configuro la PANID del dispositivo con el que me comunicaré.
 * @param   Dirección PANID de dos bytes.
 * @retval  Estado de la operación (OPERACION_NO_REALIZADA, OPERACION_REALIZADA).
 */
MRF24_State_t MRF24SetPANIDDestino(uint16_t panid) {

	if(estadoActual != INICIALIZACION_OK)
		return OPERACION_NO_REALIZADA;
	mrf24_data_out.destinity_panid = panid;
	return OPERACION_REALIZADA;
}

/**
 * @brief   Envío la información almacenada en la estructura de salida.
 * @param   None.
 * @retval  Estado de la operación (OPERACION_NO_REALIZADA, TRANSMISION_REALIZADA).
 */
MRF24_State_t MRF24TransmitirDato(void) {

	if(estadoActual != INICIALIZACION_OK)
		return OPERACION_NO_REALIZADA;
	uint8_t pos_memoria = 0;
	uint8_t largo_cabecera = HEAD_LENGTH;
	SetLongAddr(pos_memoria++, largo_cabecera);
	SetLongAddr(pos_memoria++, mrf24_data_out.largo_mensaje + largo_cabecera);
	SetLongAddr(pos_memoria++, DATA|ACK_REQ|INTRA_PAN);         // LSB.
	SetLongAddr(pos_memoria++, LONG_S_ADD|SHORT_D_ADD);         // MSB.
	SetLongAddr(pos_memoria++, mrf24_data_config.sequence_number++);
	SetLongAddr(pos_memoria++, (uint8_t) mrf24_data_out.destinity_panid);
	SetLongAddr(pos_memoria++, (uint8_t) (mrf24_data_out.destinity_panid >> SHIFT_BYTE));
	SetLongAddr(pos_memoria++, (uint8_t) mrf24_data_out.destinity_address);
	SetLongAddr(pos_memoria++, (uint8_t) (mrf24_data_out.destinity_address >> SHIFT_BYTE));

	for(int8_t i = 0; i < mrf24_data_out.largo_mensaje; i++) {

		SetLongAddr(pos_memoria++, mrf24_data_out.buffer_salida[i]);
	}
	SetLongAddr(pos_memoria++, VACIO);
	SetShortAddr(TXNCON, TXNACKREQ | TXNTRIG);
	return TRANSMISION_REALIZADA;
}

/**
 * @brief   Consulto si se levantó la bandera indicando la llegada de un mensaje.
 * @param   None.
 * @retval  Estado de la operación (ERROR_INESPERADO, MSG_PRESENTE, MSG_NO_PRESENTE).
 */
volatile MRF24_State_t MRF24IsNewMsg(void) {

	if(estadoActual != INICIALIZACION_OK)
		return ERROR_INESPERADO;

	if(!IsMRF24Interrup())
		return MSG_PRESENTE;
	return MSG_NO_PRESENTE;
}

/**
 * @brief   Recibir un paquete y dejarlo en el bufer de entrada de mrf24_data_config.
 * @param   None.
 * @retval  Estado de la operación (OPERACION_NO_REALIZADA, MSG_LEIDO).
 */
MRF24_State_t MRF24ReciboPaquete(void) {

	if(estadoActual != INICIALIZACION_OK)
		return OPERACION_NO_REALIZADA;
	uint8_t index;
	uint8_t largo_mensaje;
	SetLongAddr(BBREG1, RXDECINV);
	SetShortAddr(RXFLUSH, DATAONLY);
	largo_mensaje = GetLongAddr(RX_FIFO);

	for(index = 0; index < largo_mensaje - FCS_LQI_RSSI; index++) {

		mrf24_data_in.buffer_entrada[index] = GetLongAddr(RX_FIFO + HEAD_LENGTH + index);
	}
	SetLongAddr(BBREG1, VACIO);
	(void)GetShortAddr(INTSTAT);
	return MSG_LEIDO;
}

/**
 * @brief   Devuelvo un puntero al mensaje recibido por RF.
 * @param   None.
 * @retval  Puntero a la cadena recibida.
 */
uint8_t * MRF24GetMensajeEntrada(void) {

	return mrf24_data_in.buffer_entrada;
}

/**
 * @brief   Obtengo el PANID en el que estoy.
 * @param   None.
 * @retval  La dirección de 2 bytes de mi PANID.
 */
uint16_t MRF24GetMiPANID(void) {

	return mrf24_data_config.my_panid;
}
