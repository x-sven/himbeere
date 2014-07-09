#ifndef CRC16_H
#define CRC16_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Derived from CRC algorithm for JTAG ICE mkII, published in Atmel
 * Appnote AVR067.  Converted from C++ to C.
 */

unsigned short crcFast(unsigned char const message[], int nBytes);

#ifdef __cplusplus
}
#endif

#endif
