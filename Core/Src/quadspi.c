/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    quadspi.c
  * @brief   This file provides code for the configuration
  *          of the QUADSPI instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#include <stdio.h>
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "quadspi.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

QSPI_HandleTypeDef hqspi;

/* QUADSPI init function */
void MX_QUADSPI_Init(void)
{

  /* USER CODE BEGIN QUADSPI_Init 0 */

  /* USER CODE END QUADSPI_Init 0 */

  /* USER CODE BEGIN QUADSPI_Init 1 */

  /* USER CODE END QUADSPI_Init 1 */
  hqspi.Instance = QUADSPI;
  hqspi.Init.ClockPrescaler = 0;
  hqspi.Init.FifoThreshold = 4;
  hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_NONE;
  hqspi.Init.FlashSize = 22;
  hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_1_CYCLE;
  hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
  hqspi.Init.FlashID = QSPI_FLASH_ID_1;
  hqspi.Init.DualFlash = QSPI_DUALFLASH_DISABLE;
  if (HAL_QSPI_Init(&hqspi) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN QUADSPI_Init 2 */

  /* USER CODE END QUADSPI_Init 2 */

}

void HAL_QSPI_MspInit(QSPI_HandleTypeDef* qspiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(qspiHandle->Instance==QUADSPI)
  {
  /* USER CODE BEGIN QUADSPI_MspInit 0 */

  /* USER CODE END QUADSPI_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_QSPI;
    PeriphClkInitStruct.QspiClockSelection = RCC_QSPICLKSOURCE_D1HCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* QUADSPI clock enable */
    __HAL_RCC_QSPI_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    /**QUADSPI GPIO Configuration
    PB6     ------> QUADSPI_BK1_NCS
    PF6     ------> QUADSPI_BK1_IO3
    PF7     ------> QUADSPI_BK1_IO2
    PF8     ------> QUADSPI_BK1_IO0
    PF9     ------> QUADSPI_BK1_IO1
    PB2     ------> QUADSPI_CLK
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN QUADSPI_MspInit 1 */

  /* USER CODE END QUADSPI_MspInit 1 */
  }
}

void HAL_QSPI_MspDeInit(QSPI_HandleTypeDef* qspiHandle)
{

  if(qspiHandle->Instance==QUADSPI)
  {
  /* USER CODE BEGIN QUADSPI_MspDeInit 0 */

  /* USER CODE END QUADSPI_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_QSPI_CLK_DISABLE();

    /**QUADSPI GPIO Configuration
    PB6     ------> QUADSPI_BK1_NCS
    PF6     ------> QUADSPI_BK1_IO3
    PF7     ------> QUADSPI_BK1_IO2
    PF8     ------> QUADSPI_BK1_IO0
    PF9     ------> QUADSPI_BK1_IO1
    PB2     ------> QUADSPI_CLK
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_2);

    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9);

  /* USER CODE BEGIN QUADSPI_MspDeInit 1 */

  /* USER CODE END QUADSPI_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
#if 0
/* Set to 1 for standalone main.c test, 0 for final .stldr */
#define ENABLE_QSPI_DEBUG 0
#if ENABLE_QSPI_DEBUG
    #define QSPI_LOG(...) printf(__VA_ARGS__)
#else
    #define QSPI_LOG(...)
#endif

/**
 * @brief  Wait for Flash BUSY bit to clear (WIP=0)
 * Uses high-cycle NOP delay to compensate H750's 480MHz execution speed.
 */
static uint8_t QSPI_WaitWIP(void)
{
    QSPI_CommandTypeDef sCommand = {0};
    QSPI_AutoPollingTypeDef sConfig = {0};

    /* CRITICAL: Lead-time delay to let Flash internal logic latch the BUSY flag */
    for (volatile uint32_t i = 0; i < 20000; i++);

    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction       = W25Q_CMD_READ_SR1;
    sCommand.AddressMode       = QSPI_ADDRESS_NONE;
    sCommand.DataMode          = QSPI_DATA_1_LINE;

    sConfig.Match           = 0x00;
    sConfig.Mask            = W25Q_SR1_BUSY;
    sConfig.MatchMode       = QSPI_MATCH_MODE_AND;
    sConfig.StatusBytesSize = 1;
    sConfig.Interval        = 0x10;
    sConfig.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

    if (HAL_QSPI_AutoPolling(&hqspi, &sCommand, &sConfig, 5000) != HAL_OK) return HAL_ERROR;
    return HAL_OK;
}

/**
 * @brief  Ensure Write Enable Latch (WEL) is set
 */
static uint8_t QSPI_WriteEnable(void)
{
    QSPI_CommandTypeDef sCommand = {0};
    QSPI_AutoPollingTypeDef sConfig = {0};

    /* Clear any potential AHB/Mapped mode conflict */
    HAL_QSPI_Abort(&hqspi);

    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction     = W25Q_CMD_WRITE_ENABLE;
    if (HAL_QSPI_Command(&hqspi, &sCommand, 1000) != HAL_OK) return HAL_ERROR;

    /* Verify WEL bit via hardware polling */
    sCommand.Instruction = W25Q_CMD_READ_SR1;
    sCommand.DataMode    = QSPI_DATA_1_LINE;
    sConfig.Match           = W25Q_SR1_WEL;
    sConfig.Mask            = W25Q_SR1_WEL;
    sConfig.MatchMode       = QSPI_MATCH_MODE_AND;
    sConfig.StatusBytesSize = 1;
    sConfig.Interval        = 0x10;
    sConfig.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

    return HAL_QSPI_AutoPolling(&hqspi, &sCommand, &sConfig, 1000);
}

/**
 * @brief Read Status Registers and forcefully clear all write protections
 */
static uint8_t QSPI_CheckAndClearProtection(void)
{
    QSPI_CommandTypeDef sCommand = {0};
    uint8_t sr1 = 0, sr2 = 0, sr3 = 0;

    HAL_QSPI_Abort(&hqspi);

    /* Read SR1, SR2, SR3 */
    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.DataMode        = QSPI_DATA_1_LINE;
    sCommand.NbData          = 1;

    sCommand.Instruction = W25Q_CMD_READ_SR1;
    HAL_QSPI_Command(&hqspi, &sCommand, 1000);
    HAL_QSPI_Receive(&hqspi, &sr1, 1000);

    sCommand.Instruction = W25Q_CMD_READ_SR2;
    HAL_QSPI_Command(&hqspi, &sCommand, 1000);
    HAL_QSPI_Receive(&hqspi, &sr2, 1000);

    sCommand.Instruction = W25Q_CMD_READ_SR3;
    HAL_QSPI_Command(&hqspi, &sCommand, 1000);
    HAL_QSPI_Receive(&hqspi, &sr3, 1000);

    QSPI_LOG("    => [Status] SR1: 0x%02X, SR2: 0x%02X, SR3: 0x%02X\r\n", sr1, sr2, sr3);

    /* Check Block Protect bits in SR1 (mask 0x7C) and CMP bit in SR2 (mask 0x40) */
    if ((sr1 & 0x7C) != 0 || (sr2 & 0x40) != 0)
    {
        QSPI_LOG("    => [Warning] Flash is Write Protected! Unlocking...\r\n");

        if (QSPI_WriteEnable() != HAL_OK) return HAL_ERROR;

        /* Write 0x00 to SR1 and SR2 to clear protection */
        uint8_t clear_sr[2] = {0x00, 0x00};
        sCommand.Instruction = W25Q_CMD_WRITE_SR1_SR2;
        sCommand.NbData      = 2;
        if (HAL_QSPI_Command(&hqspi, &sCommand, 1000) != HAL_OK) return HAL_ERROR;
        if (HAL_QSPI_Transmit(&hqspi, clear_sr, 1000) != HAL_OK) return HAL_ERROR;

        QSPI_WaitWIP();
        QSPI_LOG("    => [Status] Write Protection Cleared.\r\n");
    }

    /* Check WPS bit in SR3 (Individual Block Lock) */
    if (sr3 & 0x04)
    {
        QSPI_LOG("    => [Warning] WPS enabled! Executing Global Block Unlock...\r\n");
        if (QSPI_WriteEnable() != HAL_OK) return HAL_ERROR;

        sCommand.Instruction = W25Q_CMD_GLOBAL_UNLOCK;
        sCommand.DataMode    = QSPI_DATA_NONE;
        sCommand.NbData      = 0;
        if (HAL_QSPI_Command(&hqspi, &sCommand, 1000) != HAL_OK) return HAL_ERROR;
        QSPI_WaitWIP();
        QSPI_LOG("    => [Status] Global Block Unlock Done.\r\n");
    }

    return HAL_OK;
}

/**
 * @brief Flash Initialization
 */
uint8_t CSP_QUADSPI_Init(void)
{
    QSPI_CommandTypeDef sCommand = {0};

    HAL_QSPI_Abort(&hqspi);

    QSPI_LOG("    => [Init] 1-bit Mode Resetting Device...\r\n");
    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction     = W25Q_CMD_RESET_ENABLE;
    HAL_QSPI_Command(&hqspi, &sCommand, 1000);
    sCommand.Instruction     = W25Q_CMD_RESET_DEVICE;
    HAL_QSPI_Command(&hqspi, &sCommand, 1000);

    /* Allow flash to recover from reset */
    for (volatile uint32_t i = 0; i < 500000; i++);

    /* Call the unlock and protection check routine */
    if (QSPI_CheckAndClearProtection() != HAL_OK) {
        QSPI_LOG("    => [Error] Failed to communicate with Status Registers!\r\n");
        return HAL_ERROR;
    }

    return HAL_OK;
}

uint8_t CSP_QSPI_EraseSector(uint32_t StartAddr, uint32_t EndAddr)
{
    QSPI_CommandTypeDef cmd = {0};
    StartAddr &= ~(MEMORY_SECTOR_SIZE - 1);

    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction     = W25Q_CMD_SECTOR_ERASE_4K;
    cmd.AddressMode     = QSPI_ADDRESS_1_LINE;
    cmd.AddressSize     = QSPI_ADDRESS_24_BITS;

    while (StartAddr <= EndAddr) {
        HAL_QSPI_Abort(&hqspi);
        if (QSPI_WriteEnable() != HAL_OK) return HAL_ERROR;

        cmd.Address = StartAddr;
        if (HAL_QSPI_Command(&hqspi, &cmd, 5000) != HAL_OK) return HAL_ERROR;
        if (QSPI_WaitWIP() != HAL_OK) return HAL_ERROR;

        StartAddr += MEMORY_SECTOR_SIZE;
    }
    return HAL_OK;
}

uint8_t CSP_QSPI_Erase_Chip(void)
{
    QSPI_CommandTypeDef cmd = {0};
    HAL_QSPI_Abort(&hqspi);
    if (QSPI_WriteEnable() != HAL_OK) return HAL_ERROR;

    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction     = W25Q_CMD_CHIP_ERASE;
    if (HAL_QSPI_Command(&hqspi, &cmd, 10000) != HAL_OK) return HAL_ERROR;
    return QSPI_WaitWIP();
}

uint8_t CSP_QSPI_WriteMemory(uint8_t* buffer, uint32_t address, uint32_t size)
{
    QSPI_CommandTypeDef cmd = {0};
    uint32_t end_addr = address + size;
    uint32_t current_addr = address;

    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction     = W25Q_CMD_PAGE_PROG; /* 0x02 */
    cmd.AddressMode     = QSPI_ADDRESS_1_LINE;
    cmd.AddressSize     = QSPI_ADDRESS_24_BITS;
    cmd.DataMode        = QSPI_DATA_1_LINE;

    while (current_addr < end_addr) {
        uint32_t page_offset = current_addr % MEMORY_PAGE_SIZE;
        uint32_t current_size = MEMORY_PAGE_SIZE - page_offset;
        if (current_size > (end_addr - current_addr)) current_size = end_addr - current_addr;

        HAL_QSPI_Abort(&hqspi);
        if (QSPI_WriteEnable() != HAL_OK) return HAL_ERROR;

        cmd.Address = current_addr;
        cmd.NbData  = current_size;

        if (HAL_QSPI_Command(&hqspi, &cmd, 5000) != HAL_OK) return HAL_ERROR;
        if (HAL_QSPI_Transmit(&hqspi, buffer, 5000) != HAL_OK) return HAL_ERROR;

        /* Wait strictly for internal write to complete */
        if (QSPI_WaitWIP() != HAL_OK) return HAL_ERROR;

        current_addr += current_size;
        buffer       += current_size;
    }
    return HAL_OK;
}

uint8_t CSP_QSPI_ReadMemory(uint8_t* buffer, uint32_t address, uint32_t size)
{
    QSPI_CommandTypeDef cmd = {0};
    HAL_QSPI_Abort(&hqspi);

    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction     = W25Q_CMD_READ_DATA; /* 0x03 */
    cmd.AddressMode     = QSPI_ADDRESS_1_LINE;
    cmd.AddressSize     = QSPI_ADDRESS_24_BITS;
    cmd.Address         = address;
    cmd.DataMode        = QSPI_DATA_1_LINE;
    cmd.NbData          = size;

    if (HAL_QSPI_Command(&hqspi, &cmd, 5000) != HAL_OK) return HAL_ERROR;
    return HAL_QSPI_Receive(&hqspi, buffer, 5000);
}

uint8_t CSP_QSPI_EnableMemoryMappedMode(void)
{
    QSPI_CommandTypeDef      sCommand   = {0};
    QSPI_MemoryMappedTypeDef sMemMapped = {0};

    HAL_QSPI_Abort(&hqspi);

    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction     = W25Q_CMD_READ_DATA;
    sCommand.AddressMode     = QSPI_ADDRESS_1_LINE;
    sCommand.AddressSize     = QSPI_ADDRESS_24_BITS;
    sCommand.DataMode        = QSPI_DATA_1_LINE;
    sCommand.DummyCycles     = 0;

    sMemMapped.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
    return HAL_QSPI_MemoryMapped(&hqspi, &sCommand, &sMemMapped);
}
#endif
/* USER CODE END 1 */
