#ifndef HQR_H
#define HQR_H

#include <stdint.h>

/* Returns the number of entries in the HQR file, or -1 on error. */
int hqr_get_num_entries(const char *filename);

/* Allocates a buffer, fills it with entry[index] decompressed data,
   stores the pointer in *ptr, and returns the decompressed size.
   Returns 0 on error. Caller must free(*ptr). */
int hqr_get_entry_alloc(uint8_t **ptr, const char *filename, int index);

#endif /* HQR_H */
