/**
******************************************************************************
 * @file    app_eeprom.h
 * @author  Lcdo. Mariano Ariel Deville
 * @brief   Implementación driver módulo MRF24J40
 *******************************************************************************
 * @attention
 *
 *******************************************************************************
 */

#ifndef SCR_EEPROM_H_
#define SCR_EEPROM_H_

#include "drv_MRF24J40.h"

/* Respuesta de las funciones ------------------------------------------------*/
typedef enum {

	SAVE_OK,
	SAVE_ERR,
	LOAD_OK,
	LOAD_ERR,
	PUNTERO_NO_VALIDO,
	MEMORIA_VACIA,
} eeprom_state_t;

typedef enum {

	EEPROM_MRF24_ADDR = 0x0800FC00,		// Última página de la flash (64 KB - 1 KB)
	EEPROM_CONFIG_ADDR = 0x0800FC40,
}eeprom_mem_t;

eeprom_state_t EEPROMLoadStruct(eeprom_mem_t mem_addr,
								void * p_info_s,
								size_t struct_size);
eeprom_state_t EEPROMSaveStruct(eeprom_mem_t mem_addr,
								void *p_info_s,
								size_t struct_size);

#endif /* SCR_EEPROM_H_ */
