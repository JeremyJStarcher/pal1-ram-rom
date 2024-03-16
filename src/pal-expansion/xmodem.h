#pragma once

#include <stdint.h>  // For uint16_t

#define XMODEL_UPLOAD_TYPE_PROGRAM 1
#define XMODEL_UPLOAD_TYPE_MEMORY 2

typedef struct {
  char upload_type;  // Common attribute
  union {
    uint16_t base_address;  // For the first type
    char* base_ptr;         // For the second type
  } location;
} UploadConfig;

int xmodemReceive(UploadConfig* dest, int destsz);
