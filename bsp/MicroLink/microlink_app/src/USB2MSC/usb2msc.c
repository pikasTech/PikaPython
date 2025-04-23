#ifdef CONFIG_CHERRYDAP_USE_MSC
#include "usb2msc.h"
#include "usbd_core.h"
#include "usbd_msc.h"
#include "vfs_manager.h"
#include "target_board.h"
#include "board.h"
#include "hpm_romapi.h"

extern xpi_nor_config_t s_xpi_nor_config;
extern void led_usb_in_activity(void);
extern void led_usb_out_activity(void);
enum {
    DISK_BLOCK_NUM  = FAT_ON_CHIP_FLASH_BLOCK_NUM, 
    DISK_BLOCK_SIZE = 0X1000
};

#define FAT_DIR_ENTRY_SIZE 32
#define ATTR_VOLUME_ID 0x08   // 卷标目录项的属性
uint32_t rootDirSectors = 0;
uint32_t rootDirStartSector = 0;
// **FAT16 Boot Sector 结构**
typedef struct {
    uint8_t  jmpBoot[3];        // 跳转指令
    char     OEMName[8];        // OEM 名称
    uint16_t bytesPerSector;    // 每扇区字节数
    uint8_t  sectorsPerCluster; // 每簇扇区数
    uint16_t reservedSectors;   // 保留扇区数
    uint8_t  numFATs;           // FAT 表数量
    uint16_t rootEntryCount;    // 根目录最大条目数
    uint16_t totalSectors16;    // 总扇区数（16位）
    uint8_t  media;             // 介质描述符
    uint16_t fatSize16;         // 每个 FAT 表的扇区数
    uint16_t sectorsPerTrack;   // 每磁道扇区数（BIOS 参数）
    uint16_t numHeads;          // 磁头数（BIOS 参数）
    uint32_t hiddenSectors;     // 隐藏扇区数
    uint32_t totalSectors32;    // 总扇区数（32位）
    uint8_t  driveNumber;       // 物理驱动器号（BIOS 参数）
    uint8_t  reserved1;         // 保留字段
    uint8_t  bootSignature;     // 引导标志（必须是 0x29）
    uint32_t volumeID;          // 卷序列号
    char     volumeLabel[11];   // **偏移 0x2B：卷标（U 盘名称）**
    char     fileSystemType[8]; // 文件系统类型 "FAT16  "
} __attribute__((packed)) FAT16BootSector;


// 假设 FAT16 目录项结构体
typedef struct {
    char name[11];      // 文件名（8 + 3 字节）
    uint8_t attr;       // 文件属性
    uint8_t reserved;
    uint8_t createTimeMs;
    uint16_t createTime;
    uint16_t createDate;
    uint16_t lastAccessDate;
    uint16_t firstClusterHigh;
    uint16_t writeTime;
    uint16_t writeDate;
    uint16_t firstClusterLow;
    uint32_t fileSize;
} FAT16DirEntry;

// **修改 FAT16 的卷标**
void set_volume_label(uint8_t *root_dir, const char *label) {
    if (!root_dir || !label) return;
    //// 确保 label 最多 11 个字符，并用空格填充
    char volume_label[12] = {0};
    memset(volume_label, ' ', 11);
    strncpy(volume_label, label, strlen(label) > 11 ? 11 : strlen(label));

    for (int i = 0; i < DISK_BLOCK_SIZE / FAT_DIR_ENTRY_SIZE; i++) {
        FAT16DirEntry *entry = (FAT16DirEntry *)(root_dir + i * FAT_DIR_ENTRY_SIZE);
        // 找到已有的卷标项，直接修改
        if (entry->attr == ATTR_VOLUME_ID) {
            if(strncmp(entry->name, volume_label,11) != 0 ){
                memcpy(entry->name, volume_label, 11);  // 更新卷标
                if (rom_xpi_nor_erase(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config,(FAT_ON_CHIP_FLASH_OFFSET + DISK_BLOCK_SIZE*rootDirStartSector), DISK_BLOCK_SIZE) != status_success) {
                    return;
                }
                if (rom_xpi_nor_program(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, (uint32_t *)root_dir, (FAT_ON_CHIP_FLASH_OFFSET + DISK_BLOCK_SIZE*rootDirStartSector), DISK_BLOCK_SIZE) != status_success) {
                    return;
                }
                printf("Updated volume label to: %.11s\n", entry->name);
            }
            return;
        }
    }

    // 如果没有找到卷标项，创建新的
    for (int i = 0; i < DISK_BLOCK_SIZE / FAT_DIR_ENTRY_SIZE; i++) {
        FAT16DirEntry *entry = (FAT16DirEntry *)(root_dir + i * FAT_DIR_ENTRY_SIZE);
        if (entry->name[0] == 0x00 || entry->name[0] == 0xE5) {
            memset(entry, 0, sizeof(FAT16DirEntry));  // 清空旧数据
            memcpy(entry->name, volume_label, 11);   // 设置新卷标
            entry->attr = ATTR_VOLUME_ID;            // 设置卷标属性
            if (rom_xpi_nor_erase(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config,(FAT_ON_CHIP_FLASH_OFFSET + DISK_BLOCK_SIZE*rootDirStartSector), DISK_BLOCK_SIZE) != status_success) {
                return;
            }
            if (rom_xpi_nor_program(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, (uint32_t *)root_dir, (FAT_ON_CHIP_FLASH_OFFSET + DISK_BLOCK_SIZE*rootDirStartSector), DISK_BLOCK_SIZE) != status_success) {
                return;
            }
            printf("Created new volume label: %.11s\n", entry->name);
            return;
        }
    }
    printf("Error: No space available for volume label!\n");
}
 

void usbd_msc_get_cap(uint8_t busid, uint8_t lun, uint32_t *block_num, uint32_t *block_size)
{
    (void)busid;
    (void)lun;

    *block_num = DISK_BLOCK_NUM;
    *block_size = DISK_BLOCK_SIZE;
}
uint8_t rootDirBuffer[DISK_BLOCK_SIZE];  
int usbd_msc_sector_read(uint8_t busid, uint8_t lun, uint32_t sector, uint8_t *buffer, uint32_t length)
{
    (void)busid;
    (void)lun;
    static uint8_t read_root = 0;
    if(sector <= FAT_ON_CHIP_FLASH_BLOCK_NUM){
      uint32_t level = disable_global_irq(CSR_MSTATUS_MIE_MASK);
      if (rom_xpi_nor_read(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, (uint32_t *)buffer, (FAT_ON_CHIP_FLASH_OFFSET + DISK_BLOCK_SIZE*sector), length) != status_success) {
          restore_global_irq(level);
          return 0;
      }
      restore_global_irq(level);
      if (sector == 0 && read_root == 0) {
          read_root = 1;
        
          FAT16BootSector *boot = (FAT16BootSector *)buffer;
          uint32_t totalSectors = boot->totalSectors16 ? boot->totalSectors16 : boot->totalSectors32;
          if(boot->bytesPerSector == DISK_BLOCK_SIZE && totalSectors == DISK_BLOCK_NUM){
              rootDirSectors = ((boot->rootEntryCount * 32) + (boot->bytesPerSector - 1)) / boot->bytesPerSector;
              rootDirStartSector = boot->reservedSectors + (boot->numFATs * boot->fatSize16);
              if (rom_xpi_nor_read(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, (uint32_t *)rootDirBuffer, (FAT_ON_CHIP_FLASH_OFFSET + DISK_BLOCK_SIZE*rootDirStartSector), sizeof(rootDirBuffer)) != status_success) {
                  return 0;
              }
              set_volume_label(rootDirBuffer,g_board_info.board_vendor);                                      
          } 
          uint32_t dataStartSector = rootDirStartSector + rootDirSectors;
          printf("\n===== Boot Sector Info =====\n");
          printf("Bytes Per Sector: %u\n", boot->bytesPerSector);
          printf("Sectors Per Cluster: %u\n", boot->sectorsPerCluster);
          printf("Reserved Sectors: %u\n", boot->reservedSectors);
          printf("Number of FATs: %u\n", boot->numFATs);
          printf("Root Entry Count: %u\n", boot->rootEntryCount);
          printf("FAT Size (sectors): %u\n", boot->fatSize16);
          printf("Total Sectors: %u\n", totalSectors);
          printf("Root Dir Start Sector: %u\n", rootDirStartSector);
          printf("Root Dir Size (sectors): %u\n", rootDirSectors);
          printf("Data Start Sector: %u\n", dataStartSector);                      
          printf("fileSystemType: %.8s\n", boot->fileSystemType);
          printf("===================================\n");

      }
    }
    return 0;
}

int usbd_msc_sector_write(uint8_t busid, uint8_t lun, uint32_t sector, uint8_t *buffer, uint32_t length)
{
    (void)busid;
    (void)lun;
    static uint8_t flash_program = 1;
    if(sector <= FAT_ON_CHIP_FLASH_BLOCK_NUM){
      uint32_t level = disable_global_irq(CSR_MSTATUS_MIE_MASK);
      if (rom_xpi_nor_erase(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config,(FAT_ON_CHIP_FLASH_OFFSET + DISK_BLOCK_SIZE*sector), length) != status_success) {
          restore_global_irq(level);
          flash_program = 0;
          return 0;
      }
      if (rom_xpi_nor_program(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, (uint32_t *)buffer, (FAT_ON_CHIP_FLASH_OFFSET + DISK_BLOCK_SIZE*sector), length) != status_success) {
          restore_global_irq(level);
          flash_program = 0;
          return 0;
      }
         
      if (flash_program == 1 && sector >= rootDirStartSector && sector < rootDirStartSector + rootDirSectors) {  
          transfer_finsh_state(ERROR_SUCCESS_DONE);                                
      }else if(flash_program == 1 && sector >= rootDirStartSector + rootDirSectors){
          file_data_handler(sector,buffer,length/DISK_BLOCK_SIZE);
      }
      restore_global_irq(level);
    }
    return 0;
}
#endif


