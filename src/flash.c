#include "flash.h"
#include "os.h"
#include "interrupt_state.h"

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
    jerry_port_log(JERRY_LOG_LEVEL_TRACE, "user_provided_block_device_read(block: %u, off: %u, buffer: %p, size: %u)", block, off, buffer, size);

    // Ensure interrupts are disabled to avoid flash corruption
    push_interrupt_suspension();

    u_int8_t *flash_buffer = os_get_flash_buffer();
    uint32_t offset = (c->block_size * block) + off;
    memcpy(buffer, flash_buffer + offset, size);

    // Restore interrupts
    pop_interrupt_suspension();

    return LFS_ERR_OK;
}

// Program a region in a block. The block must have previously
// been erased. Negative error codes are propagated to the user.
// May return LFS_ERR_CORRUPT if the block should be considered bad.
int user_provided_block_device_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
    jerry_port_log(JERRY_LOG_LEVEL_TRACE, "user_provided_block_device_prog(block: %u, off: %u, buffer: %p, size: %u)", block, off, buffer, size);

    // Ensure interrupts are disabled to avoid flash corruption
    push_interrupt_suspension();

    // set a page of data
    uint32_t offset = (c->block_size * block) + off;
    os_flash_range_program(FLASH_TARGET_OFFSET + offset, buffer, size);

    // Restore interrupts
    pop_interrupt_suspension();

    return LFS_ERR_OK;
}

// Erase a block. A block must be erased before being programmed.
// The state of an erased block is undefined. Negative error codes
// are propagated to the user.
// May return LFS_ERR_CORRUPT if the block should be considered bad.
int user_provided_block_device_erase(const struct lfs_config *c, lfs_block_t block)
{
    jerry_port_log(JERRY_LOG_LEVEL_TRACE, "user_provided_block_device_erase(block: %u)", block);
    uint32_t offset = c->block_size * block;

    // Ensure interrupts are disabled to avoid flash corruption
    push_interrupt_suspension();

    // Clear entire flash_buffer
    os_flash_range_erase(FLASH_TARGET_OFFSET + offset, c->block_size);

    // Restore interrupts
    pop_interrupt_suspension();

    return LFS_ERR_OK;
}

// Sync the state of the underlying block device. Negative error codes
// are propagated to the user.
int user_provided_block_device_sync(const struct lfs_config *c)
{
    jerry_port_log(JERRY_LOG_LEVEL_TRACE, "user_provided_block_device_sync()");
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
    // mount the filesystem
    int err = lfs_mount(&lfs, &cfg);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        err = lfs_format(&lfs, &cfg);
        err = lfs_mount(&lfs, &cfg);
    }
}

int psj_flash_reformat()
{
    int err = lfs_unmount(&lfs);
    if (err < 0)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "reformat: lfs_unmount returned %d", err);
        err = 0x1000000 | err;
        goto cleanup;
    }

    err = lfs_format(&lfs, &cfg);
    if (err < 0)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "reformat: lfs_format returned %d", err);
        err = 0x2000000 | err;
        goto cleanup;
    }

    err = lfs_mount(&lfs, &cfg);
    if (err < 0)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "reformat: lfs_mount returned %d", err);
        err = 0x3000000 | err;
        goto cleanup;
    }

cleanup:
    return err;
}

void psj_flash_cleanup()
{
    // release any resources we were using
    lfs_unmount(&lfs);
}

int psj_flash_save(const jerry_char_t *path, const jerry_char_t *data, const bool append)
{
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
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error opening '%s' for write: %i", path, err);
        return -1;
    }

    int fileLen = strlen(data);
    err = lfs_file_write(&lfs, &file, data, fileLen);

    lfs_file_close(&lfs, &file);

    if (err < LFS_ERR_OK)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error writing '%s': %i", path, err);
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
        jerry_port_log(JERRY_LOG_LEVEL_TRACE, "Error getting stats on '%s': %i", path, err);
        return -1;
    }
    else if (err < LFS_ERR_OK)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error getting stats on '%s': %i", path, err);
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
        jerry_port_log(JERRY_LOG_LEVEL_TRACE, "Error getting stats on '%s': %i", path, err);
        err = -1;
        goto exit;
    }
    else if (err < LFS_ERR_OK)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error getting stats on '%s': %i", path, err);
        err = -2;
        goto exit;
    }

    err = lfs_file_open(&lfs, &file, path, LFS_O_RDONLY);
    if (err < LFS_ERR_OK)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error opening '%s': %i", path, err);
        err = -3;
        goto exit;
    }
    
    err = lfs_file_read(&lfs, &file, buffer, max_length);
    if (err < LFS_ERR_OK)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error reading '%s': %i", path, err);
        err = -5;
        goto cleanup;
    }

    // Terminate the string with '\0' 
    buffer[err] = 0;

cleanup:

    lfs_file_close(&lfs, &file);

exit:

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
        jerry_port_log(JERRY_LOG_LEVEL_TRACE, "Error getting stats on '%s': %i", path, err);
        err = -1;
        goto exit;
    }
    else if (err < LFS_ERR_OK)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error getting stats on '%s': %i", path, err);
        err = -2;
        goto exit;
    }

    err = lfs_file_open(&lfs, &file, path, LFS_O_RDONLY);
    if (err < LFS_ERR_OK)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error opening '%s': %i", path, err);
        err = -3;
        goto exit;
    }
    
    err = lfs_file_seek(&lfs, &file, segment * SEGMENT_SIZE, LFS_SEEK_SET);
    if (err < LFS_ERR_OK)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error seeking '%s' to segment %i: %i", path, segment, err);
        err = -4;
        goto cleanup;
    }

    err = lfs_file_read(&lfs, &file, buffer, lfs_min(max_length, SEGMENT_SIZE));
    if (err < LFS_ERR_OK)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error reading '%s': %i", path, err);
        err = -5;
        goto cleanup;
    }

    // Terminate the string with '\0' 
    buffer[err] = 0;

cleanup:

    lfs_file_close(&lfs, &file);

exit:

    return err;
}

int psj_flash_list(struct lfs_info *file_info, uint32_t max_file_info)
{
    lfs_dir_t root_dir;
    int err = lfs_dir_open(&lfs, &root_dir, "/");
    if (err < 0)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error opening root directory: %i", err);
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
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error reading root directory: %i", err);
    }

    err = lfs_dir_close(&lfs, &root_dir);
    if (err < 0)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error closing root directory: %i", err);
    }

    return idx;
}

int psj_flash_delete(const jerry_char_t *path)
{
    int err = lfs_remove(&lfs, path);
    if (err == LFS_ERR_NOENT)
    {
        jerry_port_log(JERRY_LOG_LEVEL_TRACE, "File not found deleting '%s': %i", path, err);
        return 0;
    }
    else if (err < 0)
    {
        jerry_port_log(JERRY_LOG_LEVEL_ERROR, "Error deleting '%s': %i", path, err);
        return err;
    }

    return 1;
}