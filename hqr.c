/*
 * HQR (High Quality Resource) reader.
 * Adapted from TwinEngine: twin-e/src/hqr.c
 * Copyright (C) 2002 The TwinEngine team (GPL v2)
 *
 * HQR format:
 *   bytes 0..3        : header size in bytes (num_entries * 4)
 *   bytes 4..headerSz : offset table, one uint32 per entry
 *   at each offset    : uint32 realSize, uint32 compSize, uint16 mode
 *                       followed by compSize bytes of (possibly compressed) data
 *
 * Compression modes:
 *   0 = uncompressed
 *   1 = LBA mode-1 LZ
 *   2 = LBA mode-2 LZ
 */

#include "hqr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* -------------------------------------------------------------------------
 * Internal helpers
 * ---------------------------------------------------------------------- */

static void decompress(uint8_t *dst, uint8_t *src, int32_t decompsize, int mode)
{
    uint8_t b;
    int32_t len, d, i;
    uint16_t offset;
    uint8_t *ptr;

    do {
        b = *src++;
        for (d = 0; d < 8; d++) {
            if (!(b & (1 << d))) {
                offset = *(uint16_t *)src;
                src += 2;
                len = (offset & 0x0F) + (mode + 1);
                ptr = dst - (offset >> 4) - 1;
                for (i = 0; i < len; i++)
                    *dst++ = *ptr++;
            } else {
                len = 1;
                *dst++ = *src++;
            }
            decompsize -= len;
            if (decompsize <= 0)
                return;
        }
    } while (decompsize > 0);
}

static FILE *open_hqr(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (!f)
        fprintf(stderr, "HQR: cannot open '%s'\n", filename);
    return f;
}

/* -------------------------------------------------------------------------
 * Public API
 * ---------------------------------------------------------------------- */

int hqr_get_num_entries(const char *filename)
{
    FILE *f = open_hqr(filename);
    if (!f) return -1;

    uint32_t header_size;
    if (fread(&header_size, 4, 1, f) != 1) {
        fclose(f);
        return -1;
    }
    fclose(f);
    /* The last "entry" is a sentinel (its offset == file size).
     * Twin-e itself does hqr_get_num_entries()-1 when iterating,
     * so we subtract here to give the actual entry count. */
    return (int)(header_size / 4) - 1;
}

int hqr_get_entry_alloc(uint8_t **ptr, const char *filename, int index)
{
    FILE *f = open_hqr(filename);
    if (!f) return 0;

    uint32_t header_size;
    if (fread(&header_size, 4, 1, f) != 1) goto fail;

    if ((uint32_t)index >= header_size / 4) {
        fprintf(stderr, "HQR: index %d out of range (max %u)\n",
                index, (unsigned)(header_size / 4 - 1));
        goto fail;
    }

    /* Read offset for this entry */
    fseek(f, (long)(index * 4), SEEK_SET);
    uint32_t offset;
    if (fread(&offset, 4, 1, f) != 1) goto fail;

    /* Read entry header at that offset */
    fseek(f, (long)offset, SEEK_SET);
    uint32_t real_size, comp_size;
    uint16_t mode;
    if (fread(&real_size,  4, 1, f) != 1) goto fail;
    if (fread(&comp_size,  4, 1, f) != 1) goto fail;
    if (fread(&mode,       2, 1, f) != 1) goto fail;

    *ptr = (uint8_t *)malloc(real_size);
    if (!*ptr) {
        fprintf(stderr, "HQR: out of memory allocating %u bytes\n", real_size);
        goto fail;
    }

    if (mode == 0) {
        /* Uncompressed */
        if (fread(*ptr, 1, real_size, f) != real_size) {
            fprintf(stderr, "HQR: short read for entry %d\n", index);
            free(*ptr); *ptr = NULL;
            goto fail;
        }
    } else if (mode == 1 || mode == 2) {
        uint8_t *comp = (uint8_t *)malloc(comp_size);
        if (!comp) {
            fprintf(stderr, "HQR: out of memory for compressed buffer\n");
            free(*ptr); *ptr = NULL;
            goto fail;
        }
        if (fread(comp, 1, comp_size, f) != comp_size) {
            fprintf(stderr, "HQR: short read (compressed) for entry %d\n", index);
            free(comp); free(*ptr); *ptr = NULL;
            goto fail;
        }
        decompress(*ptr, comp, (int32_t)real_size, (int32_t)mode);
        free(comp);
    } else {
        fprintf(stderr, "HQR: unknown compression mode %u for entry %d\n", mode, index);
        free(*ptr); *ptr = NULL;
        goto fail;
    }

    fclose(f);
    return (int)real_size;

fail:
    fclose(f);
    return 0;
}
