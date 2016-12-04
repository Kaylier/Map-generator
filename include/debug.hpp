#ifndef DEBUG_HPP
#define DEBUG_HPP

#ifdef DEBUG
#define DEBUG_TEST 1
#include <cstdio> /* stderr, stdout, fprintf */
#else
#define DEBUG_TEST 0
#define NDEBUG
#endif

#include <cassert> /* assert */

#define     COLOR_BLACK     "\033[0;30m"
#define     COLOR_RED       "\033[0;31m"
#define     COLOR_GREEN     "\033[0;32m"
#define     COLOR_BROWN     "\033[0;33m"
#define     COLOR_BLUE      "\033[0;34m"
#define     COLOR_PURPLE    "\033[0;35m"
#define     COLOR_CYAN      "\033[0;36m"
#define     COLOR_LGRAY     "\033[0;37m"
#define     COLOR_DGRAY     "\033[1;30m"
#define     COLOR_BRED      "\033[1;31m"
#define     COLOR_BGREEN    "\033[1;32m"
#define     COLOR_BYELLOW   "\033[1;33m"
#define     COLOR_BBLUE     "\033[1;34m"
#define     COLOR_BPURPLE   "\033[1;35m"
#define     COLOR_BCYAN     "\033[1;36m"
#define     COLOR_WHITE     "\033[1;37m"
#define     COLOR_RESET     "\033[0m"

#define critical_error(str, ...) \
    do { \
       if (DEBUG_TEST) { \
           fprintf(stderr, \
                   COLOR_WHITE "%s:%d:%s():" \
                   COLOR_BRED " critical error : " \
                   COLOR_RED str COLOR_RESET "\n", \
                   __FILE__, \
                   __LINE__, \
                   __func__, \
                   __VA_ARGS__); \
           exit(EXIT_FAILURE); \
       } \
    } while (0)

#define error(str, ...) \
    do { \
       if (DEBUG_TEST) { \
           fprintf(stderr, \
                   COLOR_WHITE "%s:%d:%s():" \
                   COLOR_BRED " error : " \
                   COLOR_RED str COLOR_RESET "\n", \
                   __FILE__, \
                   __LINE__, \
                   __func__, \
                   __VA_ARGS__); \
       } \
    } while (0)

#define warning(str, ...) \
    do { \
       if (DEBUG_TEST) { \
           fprintf(stderr, \
                   COLOR_WHITE "%s:%d:%s():" \
                   COLOR_BPURPLE " warning : " \
                   COLOR_PURPLE str COLOR_RESET "\n", \
                   __FILE__, \
                   __LINE__, \
                   __func__, \
                   __VA_ARGS__); \
       } \
    } while (0)

#define note(str, ...) \
    do { \
       if (DEBUG_TEST) { \
           fprintf(stderr, \
                   COLOR_WHITE "%s:%d:%s():" \
                   COLOR_BCYAN " note : " \
                   COLOR_CYAN str COLOR_RESET "\n", \
                   __FILE__, \
                   __LINE__, \
                   __func__, \
                   __VA_ARGS__); \
       } \
    } while (0)

#define not_implemented() \
    do { \
       if (DEBUG_TEST) { \
           fprintf(stderr, \
                   COLOR_WHITE "%s:%d:%s():" \
                   COLOR_BYELLOW " Not implemented yet" \
                   COLOR_RESET "\n", \
                   __FILE__, \
                   __LINE__, \
                   __func__); \
       } \
    } while (0)

/* Check if GL types match with the one used in code to replace them */
static_assert(sizeof(GLboolean ) == sizeof(bool    ), "GLboolean type don't match");
static_assert(sizeof(GLbyte    ) == sizeof(int8_t  ), "GLbyte type don't match");
static_assert(sizeof(GLubyte   ) == sizeof(uint8_t ), "GLubyte type don't match");
static_assert(sizeof(GLshort   ) == sizeof(int16_t ), "GLshort type don't match");
static_assert(sizeof(GLushort  ) == sizeof(uint16_t), "GLushort type don't match");
static_assert(sizeof(GLint     ) == sizeof(int32_t ), "GLint type don't match");
static_assert(sizeof(GLuint    ) == sizeof(uint32_t), "GLuint type don't match");
static_assert(sizeof(GLfixed   ) == sizeof(int32_t ), "GLfixed type don't match");
static_assert(sizeof(GLint64   ) == sizeof(int64_t ), "GLint64 type don't match");
static_assert(sizeof(GLuint64  ) == sizeof(uint64_t), "GLuint64 type don't match");
static_assert(sizeof(GLsizei   ) == sizeof(uint32_t), "GLsizei type don't match");
static_assert(sizeof(GLenum    ) == sizeof(uint32_t), "GLenum type don't match");
static_assert(sizeof(GLintptr  ) == sizeof(size_t  ), "GLintptr type don't match");
static_assert(sizeof(GLsizeiptr) == sizeof(size_t  ), "GLsizeiptr type don't match");
static_assert(sizeof(GLsync    ) == sizeof(size_t  ), "GLsync type don't match");
static_assert(sizeof(GLbitfield) == sizeof(uint32_t), "GLbitfield type don't match");
/*static_assert(sizeof(GLhalf    ) == sizeof(float   ), "GLhalf type don't match");*/
static_assert(sizeof(GLfloat   ) == sizeof(float   ), "GLfloat type don't match");
static_assert(sizeof(GLclampf  ) == sizeof(float   ), "GLclampf type don't match");
static_assert(sizeof(GLdouble  ) == sizeof(double  ), "GLdouble type don't match");
static_assert(sizeof(GLclampd  ) == sizeof(double  ), "GLclampd type don't match");

#endif // DEBUG_HPP
