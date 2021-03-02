#ifndef _TESTING_H
#define _TESTING_H

#include <stdio.h>
#include <stdint.h>

void equalchr(const char given, const char expected);
void equalstr(const char *given, const char *expected);
void equalnstr(const char *given, const char *expected, uint32_t len);
void equalbin(const unsigned char *given, const unsigned char *, uint32_t len);
void equalint(int given, int expected);


#define RED     "\e[0;31m"
#define GREEN   "\e[0;32m"
#define YELLOW  "\e[0;33m"
#define BLUE    "\e[0;34m"
#define MAGENTA "\e[0;35m"
#define CYAN    "\e[0;36m"
#define WHITE   "\e[0;37m"

#define CLR     "\33[0m"


#define pcolor(c_, f_, ...) \
    printf(c_); \
    printf((f_), ##__VA_ARGS__); \
    printf(CLR)

#define pok(f_, ...) pcolor(GREEN, f_, ##__VA_ARGS__)
#define perr(f_, ...) pcolor(RED, f_, ##__VA_ARGS__)

#define pokln(f_, ...) pok(f_, ##__VA_ARGS__); printf("\r\n")
#define perrln(f_, ...) perr(f_, ##__VA_ARGS__); printf("\r\n")
#define pcolorln(c_, f_, ...) pcolor(c_, f_, ##__VA_ARGS__); printf("\r\n")

#define pdataln(f_, ...) pcolorln(WHITE, f_, ##__VA_ARGS__);


/* Asserts */

#define assert(f, ...) \
    pcolor(CYAN, "%s:%d", __FILE__, __LINE__); \
    pcolor(MAGENTA, " [%s] ", __func__); \
    f(__VA_ARGS__)

#define eqchr(g, e) assert(equalchr, g, e)
#define eqstr(g, e) assert(equalstr, g, e)
#define eqnstr(g, e, n) assert(equalnstr, g, e, n)
#define eqint(g, e) assert(equalint, g, e)
#define eqbin(g, e, l) assert(equalbin, (unsigned char*)g, (unsigned char*)e, l)

#endif
