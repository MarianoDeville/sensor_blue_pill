/**
******************************************************************************
 * @file    app_eeprom.c
 * @author  Lcdo. Mariano Ariel Deville
 * @brief   Implementación driver módulo MRF24J40
 *******************************************************************************
 * @attention
 *
 *******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "stm32f1xx_hal.h"
#include "app_eeprom.h"
#include "app_principal.h"

/* Estructura con la información a guardar */
typedef struct {

	mrf24_data_config_t mrf24;
	system_config_t system;
} eeprom_data_t;

/* Funciones públicas --------------------------------------------------------*/
/**
 * @brief	Copio desde memoria ram a la no volatil una estructura.
 * @param	Dirección de memoria eeprom donde se encuentra la estructura.
 * @param	Puntero a la estructura origen.
 * @param	Tamaño de la estructura.
 * @retval	Estado de la operación (SAVE_OK, SAVE_ERR).
 * @note	Como hay varias estructuras en una página recupero de la ROM
 * 			la estructura que no quiero reemplazar ya que para guardar
 * 			primero debo borrar la página completa.
 */
eeprom_state_t EEPROMSaveStruct(eeprom_mem_t mem_addr,
								void *p_info_s,
								size_t info_size) {

	if(FLASH_PAGE_SIZE < sizeof(eeprom_data_t))
		return SAVE_ERR;

	if(NULL == p_info_s)
		return SAVE_ERR;
	eeprom_data_t bloque_actual = {0};

	switch(mem_addr) {

		case EEPROM_MRF24_ADDR:

            if(info_size != sizeof(mrf24_data_config_t))
            	return SAVE_ERR;

            if(LOAD_OK != EEPROMLoadStruct(EEPROM_CONFIG_ADDR, &bloque_actual.system, sizeof(system_config_t)))
            	return SAVE_ERR;
            memcpy(&bloque_actual.mrf24, p_info_s, sizeof(mrf24_data_config_t));
            break;
		case EEPROM_CONFIG_ADDR:

            if(info_size != sizeof(system_config_t))
            	return SAVE_ERR;

            if(LOAD_OK != EEPROMLoadStruct(EEPROM_MRF24_ADDR, &bloque_actual.mrf24, sizeof(mrf24_data_config_t)))
            	return SAVE_ERR;
            memcpy(&bloque_actual.system, p_info_s, sizeof(system_config_t));
            break;
		default:
			return SAVE_ERR;
	}
	HAL_StatusTypeDef status = HAL_OK;

    uint32_t page_start = mem_addr & ~(FLASH_PAGE_SIZE - 1);
    __disable_irq();
    HAL_FLASH_Unlock();
    FLASH_EraseInitTypeDef erase = {0};
    uint32_t pageError = 0;
    erase.TypeErase = FLASH_TYPEERASE_PAGES;
    erase.PageAddress = page_start;
    erase.NbPages = 1;

    if(HAL_OK != HAL_FLASHEx_Erase(&erase, &pageError)) {

        HAL_FLASH_Lock();
        __enable_irq();
        return SAVE_ERR;
    }
    const uint32_t *p_data32 = (const uint32_t*)&bloque_actual;
    uint32_t address = page_start;

    for(uint32_t i = 0; i < (sizeof(eeprom_data_t) + 3u) / 4u; i++) {

        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, p_data32[i]);

        if(HAL_OK != status)
        	break;
        address += 4;
    }
    HAL_FLASH_Lock();
    __enable_irq();
	eeprom_data_t verify = {0};
	memcpy(&verify, (const void*)page_start, sizeof(verify));
	if(HAL_OK != status || !memcmp(&verify, &bloque_actual, sizeof(verify)))
		return SAVE_ERR;
    return SAVE_OK;
}

/**
 * @brief  Copio desde memoria no volatil a memoria ram una estructura.
 * @param  Dirección de memoria eeprom donde se encuentra la estructura.
 * @param  Puntero a la estructura destino.
 * @param  Tamaño de la estructura.
 * @retval Estado de la operación (LOAD_OK, LOAD_ERR).
 * @note
 */
eeprom_state_t EEPROMLoadStruct(eeprom_mem_t mem_addr,
						  	  	void * p_info_s,
								size_t info_size) {

	if(NULL == p_info_s || VACIO == info_size)
		return LOAD_ERR;

	switch(mem_addr) {

		case EEPROM_MRF24_ADDR:
			memcpy(p_info_s, (const void*)mem_addr, info_size);
            break;
		case EEPROM_CONFIG_ADDR:
			size_t mem_offset = mem_addr + sizeof(mrf24_data_config_t);
			memcpy(p_info_s, (const void*) mem_offset, info_size);
            break;
		default:
			return LOAD_ERR;
	}
	return LOAD_OK;
}

