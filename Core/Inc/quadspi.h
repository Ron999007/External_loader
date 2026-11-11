/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    quadspi.h
  * @brief   QSPI Header for 1-bit SPI Mode Stability Testing
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __QUADSPI_H__
#define __QUADSPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern QSPI_HandleTypeDef hqspi;

/* USER CODE BEGIN Private defines */

/* W25Q64JV Memory Parameters */
#define MEMORY_FLASH_SIZE           0x00800000U /* 8 MBytes */
#define MEMORY_BLOCK_SIZE           0x00010000U /* 64 KBytes */
#define MEMORY_SECTOR_SIZE          0x00001000U /* 4 KBytes */
#define MEMORY_PAGE_SIZE            0x00000100U /* 256 Bytes */

/* 1-bit Standard SPI Commands (For Debugging) */
#define W25Q_CMD_WRITE_ENABLE       0x06
#define W25Q_CMD_READ_SR1           0x05
#define W25Q_CMD_READ_SR2           0x35
#define W25Q_CMD_READ_SR3           0x15  /* Read Status Register-3 */
#define W25Q_CMD_WRITE_SR1_SR2      0x01
#define W25Q_CMD_PAGE_PROG          0x02
#define W25Q_CMD_READ_DATA          0x03
#define W25Q_CMD_SECTOR_ERASE_4K    0x20
#define W25Q_CMD_CHIP_ERASE         0xC7
#define W25Q_CMD_RESET_ENABLE       0x66
#define W25Q_CMD_RESET_DEVICE       0x99
#define W25Q_CMD_GLOBAL_UNLOCK      0x98  /* Global Block Unlock */

/* Status Register Bits */
#define W25Q_SR1_BUSY               0x01
#define W25Q_SR1_WEL                0x02

/* USER CODE END Private defines */

void MX_QUADSPI_Init(void);

/* USER CODE BEGIN Prototypes */

/* External Loader Public APIs */
uint8_t CSP_QUADSPI_Init(void);
uint8_t CSP_QSPI_EraseSector(uint32_t EraseStartAddress, uint32_t EraseEndAddress);
uint8_t CSP_QSPI_Erase_Chip(void);
uint8_t CSP_QSPI_WriteMemory(uint8_t* buffer, uint32_t address, uint32_t buffer_size);
uint8_t CSP_QSPI_ReadMemory(uint8_t* buffer, uint32_t address, uint32_t buffer_size);
uint8_t CSP_QSPI_EnableMemoryMappedMode(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __QUADSPI_H__ */

