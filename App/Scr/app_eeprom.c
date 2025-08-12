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
								size_t struct_size) {

	if(p_info_s == NULL || struct_size == 0)
		return LOAD_ERR;
	memcpy(p_info_s, (const void*)mem_addr, struct_size);
	return LOAD_OK;
}

/**
 * @brief  Copio desde memoria ram a no volatil una estructura.
 * @param  Dirección de memoria eeprom donde se encuentra la estructura.
 * @param  Puntero a la estructura origen.
 * @param  Tamaño de la estructura.
 * @retval Estado de la operación (SAVE_OK, SAVE_ERR).
 * @note
 */
eeprom_state_t EEPROMSaveStruct(eeprom_mem_t mem_addr,
								void *p_info_s,
								size_t struct_size) {

    HAL_StatusTypeDef status = HAL_OK;
    uint32_t page_start = mem_addr & ~(FLASH_PAGE_SIZE - 1);
    __disable_irq();
    HAL_FLASH_Unlock();
    // Borrar la página
    FLASH_EraseInitTypeDef erase;
    uint32_t pageError;
    erase.TypeErase = FLASH_TYPEERASE_PAGES;
    erase.PageAddress = page_start;
    erase.NbPages = 1;

    if(HAL_FLASHEx_Erase(&erase, &pageError) != HAL_OK) {

        HAL_FLASH_Lock();
        __enable_irq();
        return SAVE_ERR;
    }
    // Escribir de a 4 bytes
    const uint32_t *p_data = (const uint32_t *)p_info_s;
    uint32_t address = mem_addr;

    for(uint32_t i = 0; i < (struct_size + 3) / 4; i++) {

        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, p_data[i]);
        if(status != HAL_OK)
            break;
        address += 4;
    }
    HAL_FLASH_Lock();
    __enable_irq();
    return (status == HAL_OK) ? SAVE_OK : SAVE_ERR;
}
