
#ifndef	__Ctypedef_H
#define	__Ctypedef_H

typedef enum _BOOL { enFALSE , enTRUE } BOOL;	// Undefined size
typedef enum _bool { FALSE , TRUE } bool;	// Undefined size

#ifndef NULL
#define NULL    0//((void *)0)
#endif

#define	PUBLIC                                  // Function attributes
#define PROTECTED
#define PRIVATE   static

typedef unsigned char		BYTE;				// 8-bit unsigned
typedef unsigned short int	WORD;				// 16-bit unsigned
typedef unsigned long		DWORD;				// 32-bit unsigned

#ifndef WIN32
typedef unsigned long long	QWORD;				// 64-bit unsigned
#endif

typedef signed char			CHAR;				// 8-bit signed
typedef signed short int	SHORT;				// 16-bit signed
typedef signed long			LONG;				// 32-bit signed

#ifndef WIN32
typedef signed long long	LONGLONG;			// 64-bit signed
#endif

/* Alternate definitions */
typedef void                VOID;

typedef char                CHAR8;
typedef unsigned char       UCHAR8;

/* Processor & Compiler independent, size specific definitions */
// To Do:  We need to verify the sizes on each compiler.  These
//         may be compiler specific, we should either move them
//         to "compiler.h" or #ifdef them for compiler type.
typedef signed int          SINT;
typedef signed char         SINT8;
typedef signed short int    SINT16;
typedef signed long int     SINT32;
typedef signed long long    SINT64;

typedef unsigned int        UINT;
typedef unsigned char       UINT8;
typedef unsigned short int  UINT16;
typedef unsigned long int   UINT32;  // other name for 32-bit integer
typedef unsigned long long  UINT64;

#define OFF		0
#define ON		1

#endif
