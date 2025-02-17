#include <string.h>

//many copied code bcs lazy to write

void* memcpy(void* restrict dstptr, const void* restrict srcptr, size_t size) {
	unsigned char* dst = (unsigned char*) dstptr;
	const unsigned char* src = (const unsigned char*) srcptr;
	for (size_t i = 0; i < size; i++)
		dst[i] = src[i];
	return dstptr;
}

int memcmp(const void* aptr, const void* bptr, size_t size) {
	const unsigned char* a = (const unsigned char*) aptr;
	const unsigned char* b = (const unsigned char*) bptr;
	for (size_t i = 0; i < size; i++) {
		if (a[i] < b[i])
			return -1;
		else if (b[i] < a[i])
			return 1;
	}
	return 0;
}

size_t strnlen(const char* str, size_t maxlen) {
	size_t len = 0;
	while (str[len] && len <= maxlen)
		len++;
	return len;
}

void *memchr(const void *buf, int c, size_t n)
{
    unsigned char* p = (unsigned char*)buf;
    unsigned char* end = p + n;
    while (p != end)
    {
        if (*p == c)
        {
            return p;
        }
        ++p;
    }
    return 0;
}

char *strrchr(const char *s, int c)
{
    unsigned char* p = (unsigned char*)s;
    unsigned char* last = 0;
    while (*p != 0)
    {
        if (*p == c)
        {
            last = p;
        }
        ++p;
    }
    return last;
}

void* memmove(void* dstptr, const void* srcptr, size_t size) {
	unsigned char* dst = (unsigned char*) dstptr;
	const unsigned char* src = (const unsigned char*) srcptr;
	if (dst < src) {
		for (size_t i = 0; i < size; i++)
			dst[i] = src[i];
	} else {
		for (size_t i = size; i != 0; i--)
			dst[i-1] = src[i-1];
	}
	return dstptr;
}
