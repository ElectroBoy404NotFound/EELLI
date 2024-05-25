#include "utils.h"

void print_string(const char* s) {
    asm volatile( ".option norvc\ncsrrw x0, 0x138, %0\n" : : "r" (s));
}

void print_uint32(uint32_t n) {
    asm volatile( ".option norvc\ncsrrw x0, 0x136, %0\n" : : "r" (n));
}
void println_uint32(uint32_t n) {
    asm volatile( ".option norvc\ncsrrw x0, 0x136, %0\n" : : "r" (n));
    asm volatile( ".option norvc\ncsrrw x0, 0x135, %0\n" : : "r" ('\n'));
}

void print_hex(uint32_t x) {
    asm volatile( ".option norvc\ncsrrw x0, 0x137, %0\n" : : "r" (x));
}

void memcpy(void* dst, const void* src, uint16_t cnt) {
    char *csrc = (char*) src;  
    char *cdst = (char*) dst;
    for(uint32_t i = 0; i < cnt; i++)
        cdst[i] = csrc[i];
}

void* memset(void *s, uint8_t c, uint32_t len) {
    uint8_t* dst = s;
    while (len > 0) {
        *dst = (uint8_t) c;
        dst++;
        len--;
    }
    return s;
}

int strncmp( const char * s1, const char * s2, uint16_t n ) {
    while (n && *s1 && ( *s1 == *s2 )) {
        ++s1;
        ++s2;
        --n;
    }
    if (n == 0)
        return 0;
    else
        return ( *(unsigned char *)s1 - *(unsigned char *)s2 );
}

char* toupper(char* string) {
    char* sv = string;
    while(*string != '\0')
    {
        if(*sv >= 'a' && *sv <= 'z')  //Only if it's a lower letter
            *sv = *sv - ('a' - 'A');
        sv++;
    }
    return string;
}

uint32_t end_of_heap = 0x80e00000;
uint32_t start_of_heap = 0x80a00000;

void* malloc(uint32_t nbytes) {
    if (start_of_heap + nbytes >= end_of_heap) 
        fatal("Failed to malloc ram (OOM)!");
    uint32_t temp = start_of_heap;
    start_of_heap += nbytes;
    return (void*)temp;
}

uint32_t kstart_of_heap = 0x80001000;
void* kmalloc(uint32_t nbytes) {
    if (kstart_of_heap + nbytes >= 0x80900000) 
        fatal("Failed to kmalloc ram (OOM)!");
    uint32_t temp = kstart_of_heap;
    kstart_of_heap += nbytes;
    return (void*)temp;
}

void readblock(int blkno, void* dst) {
    asm volatile( ".option norvc\ncsrrw x0, 0x400, %0\n" : : "r" (blkno * 512));
    asm volatile( ".option norvc\ncsrrw x0, 0x401, %0\n" : : "r" (512));
    asm volatile( ".option norvc\ncsrrw x0, 0x402, %0\n" : : "r" (dst)); 
}
void readdisk(uint32_t start, uint32_t size, void* dst) {
    asm volatile( ".option norvc\ncsrrw x0, 0x400, %0\n" : : "r" (start));
    asm volatile( ".option norvc\ncsrrw x0, 0x401, %0\n" : : "r" (size));
    asm volatile( ".option norvc\ncsrrw x0, 0x402, %0\n" : : "r" (dst));  
}

bool readsec(void *userData, uint32_t sec, void *dst) {
	uint32_t startSec = *(const uint32_t*)userData;	
    readblock(sec + startSec, dst);
	return true;
}

void fatal(const char* s) {
    print_string(s);
    SYSCON = 0x5555;
}

void _putchar(char character) {
//   char s[2];
//   s[0] = character;
//   s[1] = '\0';
//   print_string(s);
    asm volatile( ".option norvc\ncsrrw x0, 0x135, %0\n" : : "r" (character));
}