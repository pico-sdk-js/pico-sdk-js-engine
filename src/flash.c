#include "flash.h"
#include "os.h"

#include <string.h>

#include "jerryscript-port.h"
#include "lfs.h"

#ifndef FLASH_PAGE_SIZE
    #define FLASH_PAGE_SIZE (1u << 8)
#endif

#ifndef FLASH_SECTOR_SIZE
    #define FLASH_SECTOR_SIZE (1u << 12)
#endif

#ifndef FLASH_BLOCK_SIZE
    #define FLASH_BLOCK_SIZE (1u << 16)
#endif

const char entry_file[] = "main.js";

// Read a region in a block. Negative error codes are propagated
// to the user.
int user_provided_block_device_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    jerry_port_log(JERRY_LOG_LEVEL_TRACE, "user_provided_block_device_read(block: %u, off: %u, buffer: %p, size: %u)\n", block, off, buffer, size);

    u_int8_t *flash_buffer = os_get_flash_buffer();
    u_int32_t offset = (c->block_size * block) + off;
    memcpy(buffer, flash_buffer + offset, size);

    return LFS_ERR_OK;
}

// Program a region in a block. The block must have previously
// been erased. Negative error codes are propagated to the user.
// May return LFS_ERR_CORRUPT if the block should be considered bad.
int user_provided_block_device_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
    jerry_port_log(JERRY_LOG_LEVEL_TRACE, "user_provided_block_device_prog(block: %u, off: %u, buffer: %p, size: %u)\n", block, off, buffer, size);

    // set a page of data
    u_int32_t offset = (c->block_size * block) + off;
    os_flash_range_program(FLASH_TARGET_OFFSET + offset, buffer, size);

    return LFS_ERR_OK;
}

// Erase a block. A block must be erased before being programmed.
// The state of an erased block is undefined. Negative error codes
// are propagated to the user.
// May return LFS_ERR_CORRUPT if the block should be considered bad.
int user_provided_block_device_erase(const struct lfs_config *c, lfs_block_t block)
{
    jerry_port_log(JERRY_LOG_LEVEL_TRACE, "user_provided_block_device_erase(block: %u)\n", block);
    u_int32_t offset = c->block_size * block;

    // Clear entire flash_buffer
    os_flash_range_erase(FLASH_TARGET_OFFSET + offset, c->block_size);

    return LFS_ERR_OK;
}

// Sync the state of the underlying block device. Negative error codes
// are propagated to the user.
int user_provided_block_device_sync(const struct lfs_config *c)
{
    jerry_port_log(JERRY_LOG_LEVEL_TRACE, "user_provided_block_device_sync()\n");
    return LFS_ERR_OK;
}

lfs_t lfs;

// configuration of the filesystem is provided by this struct
const struct lfs_config cfg = {
    // block device operations
    .read  = user_provided_block_device_read,
    .prog  = user_provided_block_device_prog,
    .erase = user_provided_block_device_erase,
    .sync  = user_provided_block_device_sync,

    // block device configuration
    .read_size = FLASH_PAGE_SIZE,
    .prog_size = FLASH_PAGE_SIZE,
    .block_size = FLASH_SECTOR_SIZE,
    .block_count = (FLASH_TARGET_SIZE / FLASH_SECTOR_SIZE),
    .cache_size = FLASH_PAGE_SIZE,
    .lookahead_size = FLASH_PAGE_SIZE,
    .block_cycles = 500,
};

void psj_flash_init()
{
    // Ensure interrupts are disabled to avoid flash corruption
    uint8_t status = os_save_and_disable_interrupts();

    // mount the filesystem
    int err = lfs_mount(&lfs, &cfg);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        err = lfs_format(&lfs, &cfg);
        err = lfs_mount(&lfs, &cfg);
    }

    // Restore interrupts
    os_restore_interrupts(status);
}

void psj_flash_cleanup()
{
    // release any resources we were using
    lfs_unmount(&lfs);
}

void psj_flash_save(const jerry_char_t *jsFile)
{
    // Ensure interrupts are disabled to avoid flash corruption
    uint8_t status = os_save_and_disable_interrupts();

    lfs_file_t file;
    int err;
    
    err = lfs_file_open(&lfs, &file, entry_file, LFS_O_WRONLY | LFS_O_CREAT);
    if (err != LFS_ERR_OK)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error opening '%s' for write: %i\n", entry_file, err);
        return;
    }

    int fileLen = strlen(jsFile);
    err = lfs_file_write(&lfs, &file, jsFile, fileLen);
    if (err <= LFS_ERR_OK)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error writing '%s': %i\n", entry_file, err);
    }

    lfs_file_close(&lfs, &file);

    // Restore interrupts
    os_restore_interrupts(status);
}

int psj_flash_read(jerry_char_t *buffer, u_int32_t max_length)
{
    lfs_file_t file;
    int err;
    struct lfs_info info;
    err = lfs_stat(&lfs, entry_file, &info);
    if (err == LFS_ERR_NOENT)
    {
        // file does not exist
        jerry_port_log(JERRY_LOG_LEVEL_TRACE, "Error getting stats on '%s': %i\n", entry_file, err);
        return 0;
    }
    else if (err != LFS_ERR_OK)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error getting stats on '%s': %i\n", entry_file, err);
        return 0;
    }

    err = lfs_file_open(&lfs, &file, entry_file, LFS_O_RDONLY);
    if (err != LFS_ERR_OK)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error opening '%s': %i\n", entry_file, err);
        return 0;
    }
    
    err = lfs_file_read(&lfs, &file, buffer, max_length);
    if (err <= 0)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error reading '%s': %i\n", entry_file, err);
    }

    // Terminate the string with '\0' 
    buffer[err] = 0;

    lfs_file_close(&lfs, &file);

    return err;
}
