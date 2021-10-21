#ifndef BL_FLASH_OTA_EXTENDED_H_
#define BL_FLASH_OTA_EXTENDED_H_

#define APPLICATION_PERFORM_BSL    (0xABCD55AA)
#define APP_SW_START_ADDRESS       (0x00008000)
#define SHARED_MEMORY_LOCATION     (0x20002000)
#define OTA_BUFFER_SIZE            (256)
#define OTA_FIRMWARE_NAME          "hostmcuimg.bin"
#define OTA_FIRMWARE_PATH          "/sys/hostmcuimg.bin"

#endif /* BL_FLASH_OTA_EXTENDED_H_ */
