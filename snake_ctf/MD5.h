#ifndef _MD5_H
#define _MD5_H

#include <stdint.h>

void md5(const uint8_t *initial_msg, size_t initial_len, uint8_t *digest);

#endif
