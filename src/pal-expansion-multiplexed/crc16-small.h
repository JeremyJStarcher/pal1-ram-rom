unsigned short crc16_ccitt(const void *buf, int len) {
  unsigned short crc = 0;
  while (len--) {
    int i;
    crc ^= *(char *)buf++ << 8;
    for (i = 0; i < 8; ++i) {
      if (crc & 0x8000)
        crc = (crc << 1) ^ 0x1021;
      else
        crc = crc << 1;
    }
  }
  return crc;
}