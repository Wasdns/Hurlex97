#include "string.h"
// #include <assert.h>

inline void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len) {
    for (; len != 0; len--) {
        *dest++ = *src++;
    }
}

inline void memset(void *dest, uint8_t val, uint32_t len) {
    uint8_t *dst = (uint8_t *)dest;
    for (; len != 0; len--) {
        *dst++ = val;
    }
}

inline void bzero(void *dest, uint32_t len) {
    memset(dest, 0, len);
}

inline int strlen(const char *src) {
	if (src == NULL) return 0;
    int i = 0, len = 0;
    while (*(src+i) != '\0') {
        len++; i++;
    } return len;
}

inline int strcmp(const char *str1, const char *str2) {
	if (strlen(str1) != strlen(str2))
		return 1;
	int len = strlen(str1);
	for (int i = 0; i < len; i++) {
		if (*(str1+i) != *(str2+i)) {
			return 1;
		}
	} return 0;
}

inline char *strcpy(char *dest, const char *src) {
    // assert(dest != NULL && src != NULL);
    // assert(strlen(dest) > strlen(src));
    int srclen = strlen(src);
    for (int i = 0; i < srclen; i++) {
        *(dest+i) = *(src+i);
    } return dest;
}

inline char *strcat(char *dest, const char *src) {
    // assert(sizeof(dest) > strlen(src)+strlen(dest)); // enough place
    int dstlen = strlen(dest), srclen = strlen(src);
    for (int i = dstlen; i < dstlen+srclen; i++) {
        *(dest+i) = *(src+i-dstlen);
    } return dest;
}
