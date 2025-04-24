#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define NULLPTR 0
#define PI 3.14159265359

#define stringify(entry) #entry
#define glue(a, b) a##b

#define KiloBytes(value) ((size_t)(value) * 1024L)
#define MegaBytes(value) ((size_t)KiloBytes(value) * 1024L)
#define GigaBytes(value) ((size_t)MegaBytes(value) * 1024L)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define CLAMP(value, min_value, max_value) (MIN(MAX(value, min_value), max_value))

#define local_persist static
#define internal static


// Date: July 12, 2024
// TODO(Jovanni): Test this to make sure its actually works but it makes sense to me
#define OFFSET_OF(type, member) sizeof((size_t)(&(((type*)0)->member)))
#define FIRST_DIGIT(number) ((int)number % 10);
#define GET_BIT(number, bit_to_check) ((number & (1 << bit_to_check)) >> bit_to_check)
#define SET_BIT(number, bit_to_set) number |= (1 << bit_to_set);
#define UNSET_BIT(number, bit_to_unset) number &= (~(1 << bit_to_unset));

#define ArrayCount(array) (sizeof(array) / sizeof(array[0]))

#define PLATFORM_MAX_PATH 256

#if defined(_WIN32)
    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #define PLATFORM_WINDOWS
    #define OS_DELIMITER '\\'
    #define CRASH __debugbreak()
#elif defined(__APPLE__)
    #define PLATFORM_APPLE
    #define OS_DELIMITER '/'
    #define CRASH __builtin_trap()
#elif defined(__linux__) || defined(__unix__) || defined(__POSIX__)
    #define PLATFORM_LINUX
    #define OS_DELIMITER '/'
    #define CRASH __builtin_trap()
#else
    #error "Unknown Platform???"
#endif

#if defined(__clang__)
    #define UNUSED_FUNCTION __attribute__((used))
    #define WRITE_FENCE() __asm__ volatile("" ::: "memory"); __asm__ volatile("sfence" ::: "memory")
    #define READ_FENCE() __asm__ volatile("" ::: "memory");
#elif defined(__GNUC__) || defined(__GNUG__)
    #define UNUSED_FUNCTION __attribute__((used))
    #define WRITE_FENCE() __asm__ volatile("" ::: "memory"); __asm__ volatile("sfence" ::: "memory")
    #define READ_FENCE() __asm__ volatile("" ::: "memory");
#elif defined(_MSC_VER)
    #define UNUSED_FUNCTION
    #define WRITE_FENCE() _WriteBarrier(); _mm_sfence()
    #define READ_FENCE() _ReadBarrier()
#endif