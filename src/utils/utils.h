#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
// #include "printf.h"

#define uint64_t unsigned long long
#define uint32_t unsigned long
#define uint16_t unsigned int
#define uint8_t unsigned char

#define int64_t long long
#define int32_t long
#define int16_t int
#define int8_t signed char

#define NULL ((void*) 0)

#define UINT16_MAX 0xffff

extern unsigned int SYSCON;
extern unsigned int UART;

void print_string(const char* s);
void print_uint32(uint32_t n);
void print_hex(uint32_t x);
void memcpy(void* dst, const void* src, uint16_t cnt);
int strncmp(const char * s1, const char * s2, uint16_t n);
char* toupper(char* string);
void* malloc(uint32_t size);
void* kmalloc(uint32_t size);
void* memset(void *s, uint8_t c, uint32_t len);
void readblock(int blkno, void* dst);
void readdisk(uint32_t start, uint32_t size, void* dst);
bool readsec(void *userData, uint32_t sec, void *dst);
void fatal(const char* s);

#endif