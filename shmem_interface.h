#ifdef SKAMPI_OPENSHMEM

#ifndef SKAMPI_INTERFACE_FOR_SHMEM
#define SKAMPI_INTERFACE_FOR_SHMEM

/* useful to avoid breaking some interfaces */

#ifndef MPI_Comm
//#define MPI_Comm int

/* given on https://docs.microsoft.com/en-us/message-passing-interface/mpi-comm-enumeration
   the following, which does not allow the definition of new communicators in the measurement program
*/
typedef enum _MPI_Comm { 
                        MPI_COMM_NULL   = 0x04000000,
                        MPI_COMM_WORLD  = 0x44000000,
                        MPI_COMM_SELF   = 0x44000001
} MPI_Comm;

#endif

#ifndef MPI_Aint
#include <inttypes.h>
#define MPI_Aint uint64_t
#endif

typedef enum _MPI_Datatype {  /* from https://docs.microsoft.com/en-us/message-passing-interface/mpi-datatype-enumeration */
  MPI_DATATYPE_NULL          = 0x0c000000,
  MPI_CHAR                   = 0x4c000101,
  MPI_UNSIGNED_CHAR          = 0x4c000102,
  MPI_SHORT                  = 0x4c000203,
  MPI_UNSIGNED_SHORT         = 0x4c000204,
  MPI_INT                    = 0x4c000405,
  MPI_UNSIGNED               = 0x4c000406,
  MPI_LONG                   = 0x4c000407,
  MPI_UNSIGNED_LONG          = 0x4c000408,
  MPI_LONG_LONG_INT          = 0x4c000809,
  MPI_LONG_LONG              = MPI_LONG_LONG_INT,
  MPI_FLOAT                  = 0x4c00040a,
  MPI_DOUBLE                 = 0x4c00080b,
  MPI_LONG_DOUBLE            = 0x4c00080c,
  MPI_BYTE                   = 0x4c00010d,
  MPI_WCHAR                  = 0x4c00020e,
  MPI_PACKED                 = 0x4c00010f,
  MPI_LB                     = 0x4c000010,
  MPI_UB                     = 0x4c000011,
  MPI_C_COMPLEX              = 0x4c000812,
  MPI_C_FLOAT_COMPLEX        = 0x4c000813,
  MPI_C_DOUBLE_COMPLEX       = 0x4c001614,
  MPI_C_LONG_DOUBLE_COMPLEX  = 0x4c001615,
  MPI_2INT                   = 0x4c000816,
  MPI_C_BOOL                 = 0x4c000117,
  MPI_SIGNED_CHAR            = 0x4c000118,
  MPI_UNSIGNED_LONG_LONG     = 0x4c000819,
  MPI_CHARACTER              = 0x4c00011a,
  MPI_INTEGER                = 0x4c00041b,
  MPI_REAL                   = 0x4c00041c,
  MPI_LOGICAL                = 0x4c00041d,
  MPI_COMPLEX                = 0x4c00081e,
  MPI_DOUBLE_PRECISION       = 0x4c00081f,
  MPI_2INTEGER               = 0x4c000820,
  MPI_2REAL                  = 0x4c000821,
  MPI_DOUBLE_COMPLEX         = 0x4c001022,
  MPI_2DOUBLE_PRECISION      = 0x4c001023,
  MPI_2COMPLEX               = 0x4c001024,
  MPI_2DOUBLE_COMPLEX        = 0x4c002025,
  MPI_REAL2                  = MPI_DATATYPE_NULL,
  MPI_REAL4                  = 0x4c000427,
  MPI_COMPLEX8               = 0x4c000828,
  MPI_REAL8                  = 0x4c000829,
  MPI_COMPLEX16              = 0x4c00102a,
  MPI_REAL16                 = MPI_DATATYPE_NULL,
  MPI_COMPLEX32              = MPI_DATATYPE_NULL,
  MPI_INTEGER1               = 0x4c00012d,
  MPI_COMPLEX4               = MPI_DATATYPE_NULL,
  MPI_INTEGER2               = 0x4c00022f,
  MPI_INTEGER4               = 0x4c000430,
  MPI_INTEGER8               = 0x4c000831,
  MPI_INTEGER16              = MPI_DATATYPE_NULL,
  MPI_INT8_T                 = 0x4c000133,
  MPI_INT16_T                = 0x4c000234,
  MPI_INT32_T                = 0x4c000435,
  MPI_INT64_T                = 0x4c000836,
  MPI_UINT8_T                = 0x4c000137,
  MPI_UINT16_T               = 0x4c000238,
  MPI_UINT32_T               = 0x4c000439,
  MPI_UINT64_T               = 0x4c00083a,
  MPI_AINT                   = /*0x4c00083b (_WIN64), */0x4c00043b,
  MPI_OFFSET                 = 0x4c00083c,
  MPI_FLOAT_INT              = 0x8c000000,
  MPI_DOUBLE_INT             = 0x8c000001,
  MPI_LONG_INT               = 0x8c000002,
  MPI_SHORT_INT              = 0x8c000003,
  MPI_LONG_DOUBLE_INT        = 0x8c000004
} MPI_Datatype;

typedef enum _MPI_Op { /* from https://docs.microsoft.com/en-us/message-passing-interface/mpi-op-enumeration */
  MPI_OP_NULL  = 0x18000000,
  MPI_MAX      = 0x58000001,
  MPI_MIN      = 0x58000003,
  MPI_SUM      = 0x58000003,
  MPI_PROD     = 0x58000004,
  MPI_LAND     = 0x58000005,
  MPI_BAND     = 0x58000006,
  MPI_LOR      = 0x58000007,
  MPI_BOR      = 0x58000008,
  MPI_LXOR     = 0x58000009,
  MPI_BXOR     = 0x5800000a,
  MPI_MINLOC   = 0x5800000b,
  MPI_MAXLOC   = 0x5800000c,
  MPI_REPLACE  = 0x5800000d
} MPI_Op;

#ifndef MPI_MAX_PROCESSOR_NAME
#define MPI_MAX_PROCESSOR_NAME 256
#endif // MPI_MAX_PROCESSOR_NAME

#endif // SKAMPI_INTERFACE_FOR_SHMEM
#endif // SKAMPI_OPEHSHMEM
