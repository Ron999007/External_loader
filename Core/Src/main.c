/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "quadspi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#if 0
#include <stdio.h>
#include <string.h>

/* 引入 QSPI 實例，CubeMX 生成的通常叫 hqspi */
extern QSPI_HandleTypeDef hqspi;

/* W25Q32 常用指令定義 */
#define W25Q_CMD_JEDEC_ID         0x9F
#define W25Q_CMD_WRITE_ENABLE     0x06
#define W25Q_CMD_READ_STATUS1     0x05
#define W25Q_CMD_SECTOR_ERASE     0x20
#define W25Q_CMD_PAGE_PROGRAM     0x02
#define W25Q_CMD_READ_DATA        0x03

/* --- 內部輔助函數：等待 Flash 就緒 --- */
static void W25Q_WaitReady(void)
{
    QSPI_CommandTypeDef sCommand = {0};
    QSPI_AutoPollingTypeDef sConfig = {0};

    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction       = W25Q_CMD_READ_STATUS1;
    sCommand.AddressMode       = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode          = QSPI_DATA_1_LINE;
    sCommand.DummyCycles       = 0;
    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    sConfig.Match           = 0x00;
    sConfig.Mask            = 0x01; /* WIP bit */
    sConfig.MatchMode       = QSPI_MATCH_MODE_AND;
    sConfig.StatusBytesSize = 1;
    sConfig.Interval        = 0x10;
    sConfig.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

    HAL_QSPI_AutoPolling(&hqspi, &sCommand, &sConfig, HAL_MAX_DELAY);
}

/* --- 內部輔助函數：啟用寫入 --- */
static void W25Q_WriteEnable(void)
{
    QSPI_CommandTypeDef sCommand = {0};

    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction       = W25Q_CMD_WRITE_ENABLE;
    sCommand.AddressMode       = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode          = QSPI_DATA_NONE;
    sCommand.DummyCycles       = 0;
    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    HAL_QSPI_Command(&hqspi, &sCommand, HAL_MAX_DELAY);
    W25Q_WaitReady(); /* 確保指令已被接受 */
}

/* --- 測試主程式 --- */
void W25Q_Test(void)
{
    QSPI_CommandTypeDef sCommand = {0};
    uint8_t id[3] = {0};
    uint8_t writeBuffer[256];
    uint8_t readBuffer[256];
    uint32_t testAddress = 0x00000000; /* 測試第 0 個 Sector */
    int i;

    printf("\r\n=== W25Q32 QSPI Test Start ===\r\n");

    /* 1. 讀取 JEDEC ID (驗證 SPI 通訊與硬體接線) */
    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction       = W25Q_CMD_JEDEC_ID;
    sCommand.AddressMode       = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode          = QSPI_DATA_1_LINE;
    sCommand.DummyCycles       = 0;
    sCommand.NbData            = 3;
    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_MAX_DELAY) == HAL_OK) {
        HAL_QSPI_Receive(&hqspi, id, HAL_MAX_DELAY);
        printf("1. Read ID: 0x%02X 0x%02X 0x%02X\r\n", id[0], id[1], id[2]);
        if (id[0] != 0xEF || id[1] != 0x40 || id[2] != 0x17) {
            printf("[ERROR] ID Mismatch! Check wiring or CubeMX QSPI settings.\r\n");
            return;
        }
    } else {
        printf("[ERROR] QSPI Command Failed!\r\n");
        return;
    }

    /* 2. 擦除測試 Sector (4KB) */
    printf("2. Erasing Sector 0...\r\n");
    W25Q_WriteEnable();
    sCommand.Instruction       = W25Q_CMD_SECTOR_ERASE;
    sCommand.AddressMode       = QSPI_ADDRESS_1_LINE;
    sCommand.AddressSize       = QSPI_ADDRESS_24_BITS;
    sCommand.Address           = testAddress;
    sCommand.DataMode          = QSPI_DATA_NONE;
    sCommand.NbData            = 0;
    HAL_QSPI_Command(&hqspi, &sCommand, HAL_MAX_DELAY);
    W25Q_WaitReady(); /* 擦除需要一點時間，這裡會卡住等它完成 */

    /* 3. 準備測試資料並寫入一頁 (256 Bytes) */
    printf("3. Writing 256 bytes to Page 0...\r\n");
    for (i = 0; i < 256; i++) {
        writeBuffer[i] = i; /* 寫入 0x00 ~ 0xFF */
    }

    W25Q_WriteEnable();
    sCommand.Instruction       = W25Q_CMD_PAGE_PROGRAM;
    sCommand.AddressMode       = QSPI_ADDRESS_1_LINE;
    sCommand.Address           = testAddress;
    sCommand.DataMode          = QSPI_DATA_1_LINE;
    sCommand.NbData            = 256;
    HAL_QSPI_Command(&hqspi, &sCommand, HAL_MAX_DELAY);
    HAL_QSPI_Transmit(&hqspi, writeBuffer, HAL_MAX_DELAY);
    W25Q_WaitReady();

    /* 4. 讀出資料並比對 */
    printf("4. Reading back data and verifying...\r\n");
    memset(readBuffer, 0, sizeof(readBuffer)); /* 清空讀取陣列 */

    sCommand.Instruction       = W25Q_CMD_READ_DATA;
    sCommand.AddressMode       = QSPI_ADDRESS_1_LINE;
    sCommand.Address           = testAddress;
    sCommand.DataMode          = QSPI_DATA_1_LINE;
    sCommand.NbData            = 256;
    HAL_QSPI_Command(&hqspi, &sCommand, HAL_MAX_DELAY);
    HAL_QSPI_Receive(&hqspi, readBuffer, HAL_MAX_DELAY);

    /* 比對陣列內容 */
    int errorCount = 0;
    for (i = 0; i < 256; i++) {
        if (writeBuffer[i] != readBuffer[i]) {
            errorCount++;
            if (errorCount <= 5) { /* 只印前幾個錯誤以免洗頻 */
                printf("   [FAIL] Byte %d: Wrote 0x%02X, Read 0x%02X\r\n", i, writeBuffer[i], readBuffer[i]);
            }
        }
    }

    if (errorCount == 0) {
        printf("=> [SUCCESS] All 256 bytes matched perfectly!\r\n");
        printf("=> Your hardware and basic QSPI driver are READY for the Loader.\r\n");
    } else {
        printf("=> [FAILED] Total %d bytes mismatch.\r\n", errorCount);
    }

    printf("=== Test End ===\r\n\r\n");
}
#endif

#if 0
int Safe_EnableMemoryMappedMode(void) {
    QSPI_CommandTypeDef sCommand = {0};
    QSPI_MemoryMappedTypeDef sMemMappedCfg = {0};

    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction = 0x6B; /* W25Q64 Fast Read Quad Output */
    sCommand.AddressMode = QSPI_ADDRESS_1_LINE;
    sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode = QSPI_DATA_4_LINES;
    sCommand.DummyCycles = 8; /* 絕對必須是 8 */
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    sMemMappedCfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;

    if (HAL_QSPI_MemoryMapped(&hqspi, &sCommand, &sMemMappedCfg) != HAL_OK) {
        return HAL_ERROR;
    }
    return HAL_OK;
}

uint8_t write_buffer[256];

/* 3. 測試主程式 */
void Run_QSPI_App_Test(void) {
    uint32_t test_address = 0x00000000;
    uint32_t sector_end   = 0x00000FFF;
    uint32_t error_count  = 0;

    printf("\r\n========================================\r\n");
    printf("[QSPI TEST] Polaris SNOR Flash Test Start!\r\n");
    printf("========================================\r\n");

    printf("[1/5] Initializing QSPI and W25Q64... ");
    if (CSP_QUADSPI_Init() != HAL_OK) {
        printf("[FAIL]\r\n");
        return;
    }
    printf("[OK]\r\n");

    printf("[2/5] Erasing Sector 0... ");
    if (CSP_QSPI_EraseSector(test_address, sector_end) != HAL_OK) {
        printf("[FAIL]\r\n");
        return;
    }
    printf("[OK]\r\n");

    printf("[3/5] Writing 256 bytes to Page 0... ");
    for (int i = 0; i < 256; i++) {
        write_buffer[i] = (uint8_t)(i & 0xFF);
    }

    if (CSP_QSPI_WriteMemory(write_buffer, test_address, 256) != HAL_OK) {
        printf("[FAIL]\r\n");
        return;
    }
    printf("[OK]\r\n");

    /* 🌟 解鎖 MPU */
    //MPU_Config_QSPI();

    printf("[4/5] Enabling Memory Mapped Mode... ");
    if (Safe_EnableMemoryMappedMode() != HAL_OK) {
        printf("[FAIL]\r\n");
        return;
    }
    printf("[OK]\r\n");

    /* 加入一點延遲，確保 printf 送出 UART 緩衝區 */
    HAL_Delay(10);

    printf("[5/5] Verifying data via memory map (0x90000000)... \r\n");
    uint8_t *mapped_flash_ptr = (uint8_t *)(0x90000000 + test_address);

    for (int i = 0; i < 256; i++) {
        if (mapped_flash_ptr[i] != write_buffer[i]) {
            if (error_count < 5) {
                printf("      [FAIL] Mismatch at offset %d: Wrote 0x%02X, Read 0x%02X\r\n",
                       i, write_buffer[i], mapped_flash_ptr[i]);
            }
            error_count++;
        }
    }

    if (error_count > 0) {
        printf("[FAIL] VERIFY FAILED! Total errors: %lu / 256\r\n", error_count);
    } else {
        printf("[DONE] ALL PASSED! The QSPI driver is 100%% PERFECT!\r\n");
    }

    printf("========================================\r\n\r\n");
}
#endif

#if 0
extern QSPI_HandleTypeDef hqspi;
uint8_t test_write_buffer[256];

/* * 模擬 Loader 行為的完整驗證測試
 */
void Polaris_Loader_Standalone_Test(void)
{
    uint32_t test_address = 0x00000000;
    uint32_t sector_end   = 0x00000FFF;
    uint32_t error_count  = 0;
    int i;

    printf("\r\n==============================================\r\n");
    printf("[POLARIS QSPI TEST] Standalone Verification\r\n");
    printf("==============================================\r\n");

    /* 1. 測試初始化 (Init) */
    printf("[1/5] CSP_QUADSPI_Init()... ");
    if (CSP_QUADSPI_Init() != HAL_OK) {
        printf("[FAIL] Hardware or SPI communication error.\r\n");
        return;
    }
    printf("[OK]\r\n");

    /* 2. 測試擦除 (SectorErase) */
    printf("[2/5] CSP_QSPI_EraseSector()... ");
    /* 這裡如果卡死或瞬間失敗，就是硬體供電或 WIP Polling 的問題 */
    if (CSP_QSPI_EraseSector(test_address, sector_end) != HAL_OK) {
        printf("[FAIL] Erase command rejected or timeout.\r\n");
        return;
    }
    printf("[OK]\r\n");

    /* 3. 準備測資並測試寫入 (Write) */
    printf("[3/5] CSP_QSPI_WriteMemory()... ");
    for (i = 0; i < 256; i++) {
        test_write_buffer[i] = (uint8_t)(i & 0xFF); // 0x00 ~ 0xFF
    }
    if (CSP_QSPI_WriteMemory(test_write_buffer, test_address, 256) != HAL_OK) {
        printf("[FAIL] Write command rejected.\r\n");
        return;
    }
    printf("[OK]\r\n");

    /* 4. 測試切換至記憶體映射模式 (EnableMemoryMappedMode) */
    printf("[4/5] CSP_QSPI_EnableMemoryMappedMode()... ");
    if (CSP_QSPI_EnableMemoryMappedMode() != HAL_OK) {
        printf("[FAIL] Failed to enter QSPI 4-line mapped mode.\r\n");
        return;
    }
    printf("[OK]\r\n");

    /* 5. 透過 AHB 匯流排直接讀取驗證 (Verify) */
    printf("[5/5] Verifying Memory via AHB (0x90000000)... \r\n");

    /* 使用 volatile 確保每次都從真實物理位址讀取，不受編譯器最佳化影響 */
    volatile uint8_t *mapped_flash_ptr = (volatile uint8_t *)(0x90000000 + test_address);

    /* 為了避免 UART 卡死，我們先延遲一下讓前面的 printf 送完 */
    HAL_Delay(10);

    for (i = 0; i < 256; i++) {
        uint8_t read_val = mapped_flash_ptr[i];
        if (read_val != test_write_buffer[i]) {
            if (error_count < 5) {
                printf("      [FAIL] Offset %d: Wrote 0x%02X, Read 0x%02X\r\n",
                       i, test_write_buffer[i], read_val);
            }
            error_count++;
        }
    }

    if (error_count > 0) {
        printf("=> [FAIL] Data verification failed! Total errors: %lu / 256\r\n", error_count);
        printf("=> Possible cause: Dummy cycles mismatch, IO2/IO3 wiring, or Cache issue.\r\n");
    } else {
        printf("=> [SUCCESS] All 256 bytes verified perfectly!\r\n");
        printf("=> The quadspi.c driver is 100%% ready for the Loader.\r\n");
    }

    printf("==============================================\r\n\r\n");
}
#endif

#if 0
/* External Loader and QSPI API declarations */
extern int Init(void);
extern int SectorErase(uint32_t EraseStartAddress, uint32_t EraseEndAddress);
extern int Write(uint32_t Address, uint32_t Size, uint8_t *buffer);
extern uint8_t CSP_QSPI_EnableMemoryMappedMode(void);

#define FLASH_SIZE_8MB 0x00800000  /* 8MB = 8 * 1024 * 1024 */
#define CHUNK_SIZE     4096        /* 4KB chunk size */

uint8_t chunk_buffer[CHUNK_SIZE];

void Test_Loader_API_8MB_Full(void)
{
    uint32_t offset;
    int i;
    uint32_t error_count = 0;

    printf("\r\n=== 8MB Full QSPI Flash Stress Test ===\r\n");

    /* 1. Init phase */
    printf("[1/4] Init()... ");
    fflush(stdout);
    if (Init() != 1) {
        printf("FAIL\r\n");
        return;
    }
    printf("OK\r\n");

    /* 2. Erase the entire 8MB */
    printf("[2/4] Erasing full 8MB...\r\n");
    printf("      (This will take ~20-40 seconds, please wait)\r\n");
    fflush(stdout);

    if (SectorErase(0x00000000, FLASH_SIZE_8MB - 1) != 1) {
        printf("=> [FAIL] Erase Error!\r\n");
        return;
    }
    printf("      => Erase 8MB OK!\r\n");

    /* 3. Write 8MB */
    printf("[3/4] Writing 8MB Data...\r\n");
    fflush(stdout);

    for (offset = 0; offset < FLASH_SIZE_8MB; offset += CHUNK_SIZE) {
        /* Generate address-dependent test data to detect address mirroring issues */
        for (i = 0; i < CHUNK_SIZE; i++) {
            chunk_buffer[i] = (uint8_t)((offset + i) & 0xFF);
        }

        if (Write(offset, CHUNK_SIZE, chunk_buffer) != 1) {
            printf("\r\n=> [FAIL] Write Error at offset 0x%08lX\r\n", offset);
            return;
        }

        /* Print progress every 1MB */
        if (offset > 0 && (offset % (1024 * 1024)) == 0) {
            printf("      ... %lu MB written\r\n", offset / (1024 * 1024));
            fflush(stdout);
        }
    }
    printf("      => Write 8MB OK!\r\n");

    /* 4. Verify 8MB using Memory Mapped Mode */
    printf("[4/4] Verifying 8MB via AHB (0x90000000)...\r\n");
    fflush(stdout);

    /* Enable Memory Mapped Mode */
    if (CSP_QSPI_EnableMemoryMappedMode() != HAL_OK) {
        printf("=> [FAIL] Enable Memory Mapped Mode Error!\r\n");
        return;
    }

    HAL_Delay(10);

    volatile uint8_t *mapped_ptr = (volatile uint8_t *)0x90000000;

    for (offset = 0; offset < FLASH_SIZE_8MB; offset++) {
        uint8_t expected = (uint8_t)(offset & 0xFF);
        uint8_t read_val = mapped_ptr[offset];

        if (read_val != expected) {
            if (error_count < 10) { /* Print only the first 10 errors to avoid console flood */
                printf("      [FAIL] at 0x%08lX: expected 0x%02X, got 0x%02X\r\n",
                       offset, expected, read_val);
            }
            error_count++;
        }

        /* Print progress every 1MB */
        if (offset > 0 && (offset % (1024 * 1024)) == 0) {
            printf("      ... %lu MB verified\r\n", offset / (1024 * 1024));
            fflush(stdout);
        }
    }

    /* Summary */
    if (error_count > 0) {
        printf("\r\n=> [FAIL] Verify failed! Total errors: %lu bytes\r\n", error_count);
    } else {
        printf("\r\n=> [SUCCESS] 8MB perfectly completely verified!\r\n");
        printf("=> Congratulations! Your Polaris External Loader is 100%% READY!\r\n");
    }
    printf("=======================================================\r\n\r\n");
}
#endif

#if 0
#define TEST_FLASH_SIZE 0x00800000 /* 8MB */
#define CHUNK_SIZE      4096       /* 4KB */

uint8_t write_buf[CHUNK_SIZE];
uint8_t read_buf[CHUNK_SIZE];

void Test_Direct_ReadWrite_8MB(void)
{
    uint32_t offset;
    uint32_t error_count = 0;

    printf("\r\n=== Direct SPI Command Read/Write Test (8MB) ===\r\n");

    /* 1. 直接呼叫 QSPI 初始化 (繞過 Loader_Src.c 的 Init) */
    printf("[1/3] Resetting Chip... ");
    if (CSP_QUADSPI_Init() != HAL_OK) { printf("FAIL\r\n"); return; }
    printf("OK\r\n");

    /* 2. 擦除測試 (先測前 1MB 即可，省時間) */
    uint32_t test_limit = 1024 * 1024; /* 1MB test */
    printf("[2/3] Erasing & Writing %lu MB...\r\n", test_limit / (1024*1024));

    for (offset = 0; offset < test_limit; offset += CHUNK_SIZE) {
        /* Erase Sector */
        if (CSP_QSPI_EraseSector(offset, offset + CHUNK_SIZE - 1) != HAL_OK) {
            printf("      Erase FAIL at 0x%08lX\r\n", offset);
            return;
        }

        /* Prepare Data */
        for (int i = 0; i < CHUNK_SIZE; i++) {
            write_buf[i] = (uint8_t)((offset + i) & 0xFF);
        }

        /* Write Data */
        if (CSP_QSPI_WriteMemory(write_buf, offset, CHUNK_SIZE) != HAL_OK) {
            printf("      Write FAIL at 0x%08lX\r\n", offset);
            return;
        }

        if (offset > 0 && offset % (256 * 1024) == 0) printf("      ... %lu KB done\r\n", offset / 1024);
    }
    printf("      => Write OK!\r\n");

    /* 3. 直接指令讀取比對 */
    printf("[3/3] Direct Read (Instruction 0x03) & Verifying...\r\n");

    for (offset = 0; offset < test_limit; offset += CHUNK_SIZE) {
        if (CSP_QSPI_ReadMemory(read_buf, offset, CHUNK_SIZE) != HAL_OK) {
            printf("      Read FAIL at 0x%08lX\r\n", offset);
            return;
        }

        for (int i = 0; i < CHUNK_SIZE; i++) {
            uint8_t expected = (uint8_t)((offset + i) & 0xFF);
            if (read_buf[i] != expected) {
                if (error_count < 5) {
                    printf("      [Data Error] at 0x%08lX: Expected 0x%02X, Got 0x%02X\r\n",
                           offset + i, expected, read_buf[i]);
                }
                error_count++;
            }
        }
    }

    if (error_count == 0) {
        printf("=> [SUCCESS] Direct Command Read/Write PASSED!\r\n");
    } else {
        printf("=> [FAIL] Total Errors: %lu bytes\r\n", error_count);
    }
}

/* Define test parameters */
#define TEST_ADDR        0x00000000  /* Test starting at the beginning of Flash */
#define TEST_SIZE        4096        /* 4KB (Standard Sector size) */
#define TEST_PATTERN_XOR 0x5A        /* Pattern to make data unique */

/* Buffers for read/write operations */
uint8_t write_buffer[TEST_SIZE];
uint8_t read_buffer[TEST_SIZE];

/**
 * @brief  Rigorous 4KB Read/Write/Verify Test
 * @retval 1 if success, 0 if failure
 */
int Test_Flash_4KB_Sector(void)
{
    uint32_t i;
    uint32_t error_count = 0;

    printf("\r\n--- Starting 4KB Sector Stress Test ---\r\n");

    /* 1. Initialization */
    printf("[1/5] Initializing QSPI... ");
    if (CSP_QUADSPI_Init() != HAL_OK) {
        printf("FAILED\r\n");
        return 0;
    }
    printf("OK\r\n");

    /* 2. Erase Sector */
    printf("[2/5] Erasing 4KB Sector at 0x%08X... ", (unsigned int)TEST_ADDR);
    if (CSP_QSPI_EraseSector(TEST_ADDR, TEST_ADDR + TEST_SIZE - 1) != HAL_OK) {
        printf("FAILED\r\n");
        return 0;
    }
    printf("OK\r\n");

    /* 3. Prepare Buffer with Dynamic Pattern */
    /* We use (index ^ pattern) to ensure data is not just a simple sequence */
    for (i = 0; i < TEST_SIZE; i++) {
        write_buffer[i] = (uint8_t)((i & 0xFF) ^ TEST_PATTERN_XOR);
    }

    /* 4. Write Data (Involves 16 Pages for W25Q64) */
    printf("[3/5] Writing 4KB Data... ");
    if (CSP_QSPI_WriteMemory(write_buffer, TEST_ADDR, TEST_SIZE) != HAL_OK) {
        printf("FAILED\r\n");
        return 0;
    }
    printf("OK\r\n");

    /* 5. Read Back and Verify */
    printf("[4/5] Reading back for verification... ");
    /* Using Indirect Read to bypass any Cache/AHB issues for now */
    if (CSP_QSPI_ReadMemory(read_buffer, TEST_ADDR, TEST_SIZE) != HAL_OK) {
        printf("FAILED (Read error)\r\n");
        return 0;
    }
    printf("OK\r\n");

    /* 6. Byte-by-Byte Comparison */
    printf("[5/5] Comparing data... \r\n");
    for (i = 0; i < TEST_SIZE; i++) {
        if (read_buffer[i] != write_buffer[i]) {
            if (error_count < 10) { /* Show first 10 errors */
                printf("      Mismatch at 0x%08X: Exp 0x%02X, Got 0x%02X\r\n",
                       (unsigned int)(TEST_ADDR + i), write_buffer[i], read_buffer[i]);
            }
            error_count++;
        }
    }

    /* Final Result */
    if (error_count == 0) {
        printf(">>> [SUCCESS] 4KB Sector Verified Perfectly! <<<\r\n");
        return 1;
    } else {
        printf(">>> [FAILURE] Total Errors: %lu bytes <<<\r\n", error_count);
        uint8_t dump[16];
		printf("\r\n--- Diagnostic Dump ---\r\n");

		CSP_QSPI_ReadMemory(dump, 0x00, 16);
		printf("Page 0 (Start): ");
		for(int i=0; i<16; i++) printf("%02X ", dump[i]);

		printf("\r\nPage 1 (Start): ");
		CSP_QSPI_ReadMemory(dump, 0x100, 16);
		for(int i=0; i<16; i++) printf("%02X ", dump[i]);
		printf("\r\n-----------------------\r\n");
        return 0;
    }
}
#endif
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_QUADSPI_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  //printf("Start! \r\n");
  //Run_QSPI_App_Test();
  //Polaris_Loader_Standalone_Test();
  //Test_Loader_API_8MB_Full();
  //Test_Direct_ReadWrite_8MB();
  //Test_Flash_4KB_Sector();


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
#if 0
/* Retarget standard output (printf) to UART */
int _write(int file, char *ptr, int len)
{
    /* * Send the string via USART1.
     * Note: Change 'huart1' to your specific UART handle if different.
     */
    HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, HAL_MAX_DELAY);

    return len;
}
#endif
/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
