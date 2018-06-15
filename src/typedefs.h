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

#ifndef Opcode_Start
#define Opcode_Start 21
#endif

#ifndef Opcode_End
#define Opcode_End 24
#endif

#ifndef Rd_Start
#define Rd_Start 12
#endif

#ifndef Rd_End
#define Rd_End 15
#endif

#ifndef Rn_Start
#define Rn_Start 16
#endif

#ifndef Rn_End
#define Rn_End 19
#endif

#ifndef Rm_Start
#define Rm_Start 0
#endif

#ifndef Rm_End
#define Rm_End 3
#endif

#ifndef Rs_Start
#define Rs_Start 8
#endif

#ifndef Rs_End
#define Rs_End 11
#endif

#ifndef OP2_Start
#define OP2_Start 0
#endif

#ifndef OP2_End
#define OP2_End 11
#endif

#ifndef OP2_IMM_Start
#define OP2_IMM_Start 0
#endif

#ifndef OP2_IMM_End
#define OP2_IMM_End 7
#endif

#ifndef OP2_Shift_Start
#define OP2_Shift_Start 5
#endif

#ifndef OP2_Shift_End
#define OP2_Shift_End 6
#endif

#ifndef OP2_INT_Start
#define OP2_INT_Start 7
#endif

#ifndef OP2_INT_End
#define OP2_INT_End 11
#endif

#ifndef OP2_Rt_St
#define OP2_Rt_St 8
#endif

#ifndef OP2_Rt_End
#define OP2_Rt_End 11
#endif

#ifndef Branch_Start
#define Branch_Start 25
#endif

#ifndef Branch_End
#define Branch_End 27
#endif

#ifndef Br_Cond_Start
#define Br_Cond_Start 28
#endif

#ifndef Br_Cond_End
#define Br_Cond_End 31
#endif

