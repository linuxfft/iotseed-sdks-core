//
// Created by gubin on 17-11-20.
//

#ifndef IOTSEED_SDKS_CORE_GLBTYPES_H
#define IOTSEED_SDKS_CORE_GLBTYPES_H

#define SD_TRUE		1
#define SD_FALSE	0
#define SD_SUCCESS 	0
#define SD_FAILURE 	1
#define SD_BIG_ENDIAN		0 //大端
#define SD_LITTLE_ENDIAN	1 //小端



#if defined(_WIN32)

#define SD_BYTE_ORDER	SD_LITTLE_ENDIAN

#define ST_CHAR    char
#define ST_INT     signed int
#define ST_LONG    signed long int
#define ST_UCHAR   unsigned char
#define ST_UINT    unsigned int
#define ST_ULONG   unsigned long
#define ST_VOID    void
#define ST_VOID_PTR ST_VOID*
#define ST_DOUBLE  double
#define ST_FLOAT   float

#define ST_RET signed int

#define ST_INT8     signed char
#define ST_INT16    signed short
#define ST_INT32    signed long
#define ST_INT64    __int64
#define ST_UINT8    unsigned char
#define ST_UINT16   unsigned short
#define ST_UINT32   unsigned long
#define ST_UINT64   unsigned __int64
#define ST_BOOLEAN  unsigned char

#define INT64_SUPPORT


#endif /* windows	*/


#if defined(__linux__) || defined(__LYNX)
#include <endian.h>
#if (__BYTE_ORDER ==__LITTLE_ENDIAN)
#define SD_BYTE_ORDER	SD_LITTLE_ENDIAN
#elif (__BYTE_ORDER ==__BIG_ENDIAN)
#define SD_BYTE_ORDER	SD_BIG_ENDIAN
#else
  #error unsupported byte order
#endif

#define SD_END_STRUCT

#define ST_CHAR    char
#define ST_INT     signed int
#define ST_LONG    signed long int
#define ST_UCHAR   unsigned char
#define ST_UINT    unsigned int
#define ST_ULONG   unsigned long
#define ST_VOID    void
#define ST_VOID_PTR ST_VOID*
#define ST_DOUBLE  double
#define ST_FLOAT   float

#define ST_RET signed int

#define ST_INT8   signed char
#define ST_INT16  signed short
#define ST_INT32  signed int			/* was signed long	*/
#define ST_INT64  signed long long
#define ST_UINT8  unsigned char
#define ST_UINT16 unsigned short
#define ST_UINT32 unsigned int			/* was unsigned long	*/
#define ST_UINT64 unsigned long long
#define ST_BOOLEAN  unsigned char


/* This define shows that we really have support for 64 bit integers	*/
#define INT64_SUPPORT


#endif	/* linux	*/


typedef enum _enum_value_type_t {
    R_VAL_INT8_T  = 1,
    R_VAL_INT16_T,
    R_VAL_INT32_T,
    R_VAL_INT64_T,
    R_VAL_UINT8_T,
    R_VAL_UINT16_T,
    R_VAL_UINT32_T,
    R_VAL_UINT64_T,
    R_VAL_FLOAT_T,
    R_VAL_DOUBLE_T,
    R_VAL_BOOLEAN_T,
    R_VAL_STRING_T
}IOTSEED_VAL_TYPE;


#endif //IOTSEED_SDKS_CORE_GLBTYPES_H
