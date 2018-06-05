#include <stdint-gcc.h>

#ifndef ARM_TYPE_TYPEDEFS
#define ARM_TYPE_TYPEDEFS
typedef uint8_t byte;
typedef uint32_t word;
typedef int32_t wordS;
typedef uint16_t address;
#endif

#ifndef MEM_SIZE
#define MEM_SIZE 65536
#endif

#ifndef REG_SIZE
#define REG_SIZE 17
#endif

#ifndef PC
#define PC 15
#endif

#ifndef CPSR
#define CPSR 16
#endif

#ifndef PC_OFFSET
#define PC_OFFSET 8
#endif

#ifndef flagV
#define flagV 28
#endif

#ifndef flagC
#define flagC 29
#endif

#ifndef flagZ
#define flagZ 30
#endif

#ifndef flagN
#define flagN 31
#endif

#ifndef SBit
#define SBit 20
#endif

#ifndef ABit
#define ABit 21
#endif

#ifndef upBit
#define upBit 23
#endif

#ifndef ppIndexBit
#define ppIndexBit 24
#endif

#ifndef branchOffset
#define branchOffset 24
#endif

#ifndef immediateOffset
#define immediateOffset 25
#endif

#ifndef unsignedImmOffset
#define unsignedImmOffset 12
#endif

