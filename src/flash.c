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

// Read a region in a block. Negative error codes are propagated
// to the user.
int user_provided_block_device_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    jerry_port_log(JERRY_LOG_LEVEL_TRACE, "user_provided_block_device_read(block: %u, off: %u, buffer: %p, size: %u)\n", block, off, buffer, size);

    u_int8_t *flash_buffer = os_get_flash_buffer();
    uint32_t offset = (c->block_size * block) + off;
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
    uint32_t offset = (c->block_size * block) + off;
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
    uint32_t offset = c->block_size * block;

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

int psj_flash_save(const jerry_char_t *path, const jerry_char_t *data, const bool append)
{
    // Ensure interrupts are disabled to avoid flash corruption
    uint8_t status = os_save_and_disable_interrupts();

    lfs_file_t file;
    int err;
    int open_flags = LFS_O_WRONLY | LFS_O_CREAT;
    if (append)
    {
        open_flags |= LFS_O_APPEND;
    }
    else
    {
        open_flags |= LFS_O_TRUNC;
    }
    
    err = lfs_file_open(&lfs, &file, path, open_flags);
    if (err != LFS_ERR_OK)
    {
        // Restore interrupts to reenable logging
        os_restore_interrupts(status);
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error opening '%s' for write: %i\n", path, err);
        return -1;
    }

    int fileLen = strlen(data);
    err = lfs_file_write(&lfs, &file, data, fileLen);

    // Restore interrupts after writing
    os_restore_interrupts(status);
    lfs_file_close(&lfs, &file);

    if (err < LFS_ERR_OK)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error writing '%s': %i\n", path, err);
        return -2;
    }

    return err;
}

int psj_flash_file_size(const jerry_char_t *path, uint32_t *size)
{
    lfs_file_t file;
    struct lfs_info info;
    int err = lfs_stat(&lfs, path, &info);
    if (err == LFS_ERR_NOENT)
    {
        // file does not exist
        jerry_port_log(JERRY_LOG_LEVEL_TRACE, "Error getting stats on '%s': %i\n", path, err);
        return -1;
    }
    else if (err < LFS_ERR_OK)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error getting stats on '%s': %i\n", path, err);
        return -2;
    }

    *size = info.size;
    return 0;
}

int psj_flash_read_all(const jerry_char_t *path, jerry_char_t *buffer, uint32_t max_length)
{
    lfs_file_t file;
    int err;
    struct lfs_info info;
    err = lfs_stat(&lfs, path, &info);
    if (err == LFS_ERR_NOENT)
    {
        // file does not exist
        jerry_port_log(JERRY_LOG_LEVEL_TRACE, "Error getting stats on '%s': %i\n", path, err);
        return -1;
    }
    else if (err < LFS_ERR_OK)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error getting stats on '%s': %i\n", path, err);
        return -2;
    }

    err = lfs_file_open(&lfs, &file, path, LFS_O_RDONLY);
    if (err < LFS_ERR_OK)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error opening '%s': %i\n", path, err);
        return -3;
    }
    
    err = lfs_file_read(&lfs, &file, buffer, max_length);
    if (err < LFS_ERR_OK)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error reading '%s': %i\n", path, err);
        err = -5;
        goto cleanup;
    }

    // Terminate the string with '\0' 
    buffer[err] = 0;

cleanup:

    lfs_file_close(&lfs, &file);

    return err;
}

int psj_flash_read(const jerry_char_t *path, jerry_char_t *buffer, uint32_t max_length, uint32_t segment)
{
    lfs_file_t file;
    int err;
    struct lfs_info info;
    err = lfs_stat(&lfs, path, &info);
    if (err == LFS_ERR_NOENT)
    {
        // file does not exist
        jerry_port_log(JERRY_LOG_LEVEL_TRACE, "Error getting stats on '%s': %i\n", path, err);
        return -1;
    }
    else if (err < LFS_ERR_OK)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error getting stats on '%s': %i\n", path, err);
        return -2;
    }

    err = lfs_file_open(&lfs, &file, path, LFS_O_RDONLY);
    if (err < LFS_ERR_OK)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error opening '%s': %i\n", path, err);
        return -3;
    }
    
    err = lfs_file_seek(&lfs, &file, segment * SEGMENT_SIZE, LFS_SEEK_SET);
    if (err < LFS_ERR_OK)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error seeking '%s' to segment %i: %i\n", path, segment, err);
        err = -4;
        goto cleanup;
    }

    err = lfs_file_read(&lfs, &file, buffer, lfs_min(max_length, SEGMENT_SIZE));
    if (err < LFS_ERR_OK)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error reading '%s': %i\n", path, err);
        err = -5;
        goto cleanup;
    }

    // Terminate the string with '\0' 
    buffer[err] = 0;

cleanup:

    lfs_file_close(&lfs, &file);

    return err;
}

int psj_flash_list(struct lfs_info *file_info, uint32_t max_file_info)
{
    lfs_dir_t root_dir;
    int err = lfs_dir_open(&lfs, &root_dir, "/");
    if (err < 0)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error opening root directory: %i\n", err);
        return 0;
    }

    int idx = 0;
    while (idx < max_file_info && (err = lfs_dir_read(&lfs, &root_dir, file_info + idx)) > 0) 
    {
        if (file_info[idx].name[0] != '.')
        {
            idx++;
        }
    }

    if (err < 0)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error reading root directory: %i\n", err);
    }

    err = lfs_dir_close(&lfs, &root_dir);
    if (err < 0)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error closing root directory: %i\n", err);
    }

    return idx;
}