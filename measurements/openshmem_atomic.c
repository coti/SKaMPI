#ifdef SKAMPI_OPENSHMEM

#include <mpi.h>
#include <shmem.h>
#include <stdlib.h>
#include "../mpiversiontest.h"
#include <unistd.h> // for usleep

#include "../misc.h"
#include "../mem.h"
#include "../synchronize.h"
#include "../private_misc.h"

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

#define M 120  /* The M parameter of the SK algorithms, "large" */
int* ack;
int size;
char* psync;
char* source;
char* target;
char* pwrk;
shmem_ctx_t context;

/*---------------------------------------------------------------------------*/
/*                          Atomic routines                                  */
/*---------------------------------------------------------------------------*/

void init_Shmem_Atomic_Fetch(){
  ack = shmem_malloc( sizeof( int ));
  *ack = get_measurement_rank();
  init_synchronization();
}

double measure_Shmem_Atomic_Fetch(){
  double start_time = 1.0, end_time = 0.0, mytime;
  int r, mod;
  mod = get_measurement_rank() %2;

  start_time = start_synchronization();

  /* Odd-numbered processes fetch from their -1 neighbor */
  if  ( mod == 1 ){
    start_time = wtime();
    r = shmem_int_atomic_fetch( ack, get_measurement_rank() - 1 );
    end_time = wtime();
  } 
  if  ( mod == 1 ){
    mytime = end_time - start_time;
  }
  shmem_barrier_all();

  /* Even-numbered processes fetch from their +1 neighbor, if they have one */
  if  ( mod == 0 && get_measurement_rank() < get_measurement_size() - 1 ){
    start_time = wtime();
    r = shmem_int_atomic_fetch( ack, get_measurement_rank() + 1 );
    end_time = wtime();
  } 
  if  ( mod ==  0 && get_measurement_rank() < get_measurement_size() - 1 ){
    mytime = end_time - start_time;
  }
  shmem_barrier_all();

  /* Otherwise, the last process fetches from 0 */
  if( get_measurement_size() % 2 > 0 ){
    if  ( get_measurement_rank() == get_measurement_size() - 1 ){
      start_time = wtime();
      r = shmem_int_atomic_fetch( ack, 0 );
      end_time = wtime();
    } 
    shmem_barrier_all();
    if  ( get_measurement_rank() == get_measurement_size() - 1 ){
      mytime = end_time - start_time;
    }
  }
  end_time = stop_synchronization();
  
  return mytime;
}

void finalize_Shmem_Atomic_Fetch(){
  shmem_free( ack );
}

void init_Shmem_Ctx_Atomic_Fetch(){
  ack = shmem_malloc( sizeof( int ));
  shmem_ctx_create( SHMEM_CTX_SERIALIZED, &context );
  init_synchronization();
}

double measure_Shmem_Ctx_Atomic_Fetch(){
  double start_time = 1.0, end_time = 0.0, mytime;
  int r, mod;
  mod = get_measurement_rank() %2;

  /* Odd-numbered processes fetch from their -1 neighbor */
  start_time = start_synchronization();
  if  ( mod == 1 ){
    start_time = wtime();
    r = shmem_ctx_int_atomic_fetch( context, ack, get_measurement_rank() - 1 );
    end_time = wtime();
    mytime = end_time - start_time;
  }
  shmem_barrier_all();

  /* Even-numbered processes fetch from their +1 neighbor, if they have one */
  if  ( mod == 0 && get_measurement_rank() < get_measurement_size() - 1 ){
    start_time = wtime();
    r = shmem_ctx_int_atomic_fetch( context, ack, get_measurement_rank() + 1 );
    end_time = wtime();
    mytime = end_time - start_time;
  }
  shmem_barrier_all();

  /* Otherwise, the last process fetches from 0 */
  if( get_measurement_size() % 2 > 0 ){
    if  ( get_measurement_rank() == get_measurement_size() - 1 ){
      start_time = wtime();
      r = shmem_ctx_int_atomic_fetch( context, ack, 0 );
      end_time = wtime();
      mytime = end_time - start_time;
    }
    shmem_barrier_all();
  }
  end_time = stop_synchronization();

  return mytime;
}

void finalize_Shmem_Ctx_Atomic_Fetch(){
  shmem_free( ack );
  shmem_ctx_destroy( context );
}

/*---------------------------------------------------------------------------*/

#pragma weak end_skampi_extensions

#endif // SKAMPI_OPENSHMEM

