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

#include <shmem.h>
#include <stdlib.h>
#ifdef SKAMPI_MPI
/* This must come before mpiversiontest.h which must come before the other ones */
#include <mpi.h> 
#endif
#include "../mpiversiontest.h"

#include "../misc.h"
#include "../mem.h"
#include "../synchronize.h"

#define max(a,b)              \
  ({ __typeof__ (a) _a = (a); \
  __typeof__ (b) _b = (b);    \
  _a > _b ? _a : _b; })

#define wait_for_ack( value ) \
  ({ while( *ack < value ){   \
    usleep( 100 );            \
  }                           \
     *ack = 0; })


#pragma weak begin_skampi_extensions

shmem_ctx_t context;

/*---------------------------------------------------------------------------*/
/*                             Contexts                                      */
/*---------------------------------------------------------------------------*/

/* We cannot pass the options directly, because it would require modifying the parser
   in order to accept these values. */

void tini_Shmem_Ctx_Create(){
  init_synchronization();
}

void init_Shmem_Ctx_Create_Serialized(){
  tini_Shmem_Ctx_Create();
}
void init_Shmem_Ctx_Create_Private(){
  tini_Shmem_Ctx_Create();
}
void init_Shmem_Ctx_Create_Nostore(){
  tini_Shmem_Ctx_Create();
}

double erusaem_Shmem_Ctx_Create( long options ){
  double start_time = 1.0, end_time = 0.0;
  
  start_time = start_synchronization();
  shmem_ctx_create( options, &context );
  end_time = stop_synchronization();

  shmem_ctx_destroy( context );
  return end_time - start_time;
}

double measure_Shmem_Ctx_Create_Serialized(){
  return erusaem_Shmem_Ctx_Create( SHMEM_CTX_SERIALIZED );
}
double measure_Shmem_Ctx_Create_Private(){
  return erusaem_Shmem_Ctx_Create( SHMEM_CTX_PRIVATE );
}
double measure_Shmem_Ctx_Create_Nostore(){
  return erusaem_Shmem_Ctx_Create( SHMEM_CTX_NOSTORE );
}

/*---------------------------------------------------------------------------*/

void tini_Shmem_Ctx_Destroy(){
  init_synchronization();
}

void init_Shmem_Ctx_Destroy_Serialized(){
  tini_Shmem_Ctx_Destroy();
}
void init_Shmem_Ctx_Destroy_Private(){
  tini_Shmem_Ctx_Destroy();
}
void init_Shmem_Ctx_Destroy_Nostore(){
  tini_Shmem_Ctx_Destroy();
}

double erusaem_Shmem_Ctx_Destroy( long options ){
  double start_time = 1.0, end_time = 0.0;

  shmem_ctx_create( options, &context );

  start_time = start_synchronization();
  shmem_ctx_destroy( context );
  end_time = stop_synchronization();
  
  return end_time - start_time;
}

double measure_Shmem_Ctx_Destroy_Serialized(){
  return erusaem_Shmem_Ctx_Destroy( SHMEM_CTX_SERIALIZED );
}
double measure_Shmem_Ctx_Destroy_Private(){
  return erusaem_Shmem_Ctx_Destroy( SHMEM_CTX_PRIVATE );
}
double measure_Shmem_Ctx_Destroy_Nostore(){
  return erusaem_Shmem_Ctx_Destroy( SHMEM_CTX_NOSTORE );
}

/*---------------------------------------------------------------------------*/

#pragma weak end_skampi_extensions

#endif // SKAMPI_OPENSHMEM

