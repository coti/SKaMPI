/* SKaMPI   MPI-Benchmark

2021 Camille Coti, Laboratoire d'Informatique de Paris Nord
Universite Sorbonne Paris Nord.

This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as
published by the Free Software Foundation

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA */

#ifdef SKAMPI_OPENSHMEM
#ifndef SKAMPI_MPI

#ifndef SKAMPI_INTERFACE_FOR_SHMEM
#define SKAMPI_INTERFACE_FOR_SHMEM

#include <shmem.h>

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


#define MPI_SUCCESS      0     /* Successful return code.*/
#define MPI_ERR_BUFFER   1  /* Invalid buffer pointer */
#define MPI_ERR_COUNT    2  /* Invalid count argument */
#define MPI_ERR_TYPE     3  /* Invalid datatype argument */
#define MPI_ERR_TAG      4  /* Invalid tag argument */
#define MPI_ERR_COMM     5  /* Invalid communicator */
#define MPI_ERR_RANK     6  /* Invalid rank         */
#define MPI_ERR_ROOT     7  /* Invalid root         */
#define MPI_ERR_GROUP    8  /* Invalid group        */
#define MPI_ERR_OP       9  /* Invalid operation    */
#define MPI_ERR_TOPOLOGY   10  /* Invalid topology    */
#define MPI_ERR_DIMS       11  /* Invalid dimension argument */
#define MPI_ERR_ARG        12  /* Invalid argument    */
#define MPI_ERR_UNKNOWN    13  /* Unknown error       */
#define MPI_ERR_TRUNCATE   14  /* Message truncated on receive  */
#define MPI_ERR_OTHER      15  /* Other error; use Error_string */
#define MPI_ERR_INTERN     16  /* Internal error code           */
#define MPI_ERR_IN_STATUS   17  /* Error code is in status      */
#define MPI_ERR_PENDING     18  /* Pending request              */
#define MPI_ERR_REQUEST     19  /* Invalid request (handle)     */
#define MPI_ERR_ACCESS      20  /* Permission denied            */
#define MPI_ERR_AMODE         21  /* Error related to amode passed to #define MPI_File_open      */
#define MPI_ERR_BAD_FILE      22  /* Invalid file name (e.g.  path name too long)                */
#define MPI_ERR_CONVERSION    23  /* Error in user data conversion function                      */
#define MPI_ERR_DUP_DATAREP   24  /* Data representation identifier already registered           */
#define MPI_ERR_FILE_EXISTS   25  /* File exists                                                 */
#define MPI_ERR_FILE_IN_USE    26  /* File operation could not be completed  file in use         */
#define MPI_ERR_FILE           27  /* Invalid file handle                                        */
#define MPI_ERR_INFO           28  /* Invalid info argument                                      */
#define MPI_ERR_INFO_KEY       29  /* Key longer than #define MPI_MAX_INFO_KEY                   */
#define MPI_ERR_INFO_VALUE     30  /* Value longer than #define MPI_MAX_INFO_VAL                 */
#define MPI_ERR_INFO_NOKEY     31  /* Invalid key passed to #define MPI_Info_delete              */
#define MPI_ERR_IO             32  /* Other I/O error                                            */
#define MPI_ERR_NAME           33  /* Invalid service name in #define MPI_Lookup_name            */
#define MPI_ERR_NO_MEM         34  /* Alloc_mem could not allocate memory                        */
#define MPI_ERR_NOT_SAME       35  /* Collective argument/sequence not the same on all processes */
#define MPI_ERR_NO_SPACE       36  /* Not enough space                                           */
#define MPI_ERR_NO_SUCH_FILE   37  /* File does not exist                                        */
#define MPI_ERR_PORT           38  /* Invalid port name in #define MPI_comm_connect              */
#define MPI_ERR_QUOTA          39  /* Quota exceeded                                             */
#define MPI_ERR_READ_ONLY      40  /* Read-only file or file system                              */
#define MPI_ERR_SERVICE        41  /* Invalid service name in #define MPI_Unpublish_name         */
#define MPI_ERR_SPAWN          42  /* Error in spawning processes                                */
#define MPI_ERR_UNSUPPORTED_DATAREP   43  /* Unsupported dararep in #define MPI_File_set_view    */
#define MPI_ERR_UNSUPPORTED_OPERATION   44  /* Unsupported operation on file                     */
#define MPI_ERR_WIN            45  /* Invalid win argument                                       */
#define MPI_ERR_BASE           46  /* Invalid base passed to #define MPI_Free_mem                */
#define MPI_ERR_LOCKTYPE       47  /* Invalid locktype argument                                  */
#define MPI_ERR_KEYVAL         48  /* Invalid keyval                                             */
#define MPI_ERR_RMA_CONFLICT   49  /* Conflicting accesses to window                             */
#define MPI_ERR_RMA_SYNC       50  /* Wrong synchronization of RMA calls                         */
#define MPI_ERR_SIZE           51  /* Invalid size argument                                      */
#define MPI_ERR_DISP           52  /* Invalid disp argument                                      */
#define MPI_ERR_ASSERT         53  /* Invalid assert argument                                    */
#define MPI_ERR_LASTCODE       0x3fffffff



#ifndef MPI_MAX_PROCESSOR_NAME
#define MPI_MAX_PROCESSOR_NAME 256
#endif // MPI_MAX_PROCESSOR_NAME

#endif // SKAMPI_MPI
#endif // SKAMPI_INTERFACE_FOR_SHMEM
#endif // SKAMPI_OPENSHMEM
