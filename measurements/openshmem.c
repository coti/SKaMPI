#ifdef SKAMPI_OPENSHMEM

#include <mpi.h>
#include <shmem.h>
#include <stdlib.h>
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

#define M 120  /* The M parameter of the SK algorithms, "large" */
int* ack;
int size;
char* psync;
char* source;
char* target;
char* pwrk;
shmem_ctx_t context;
static long mylock = 0;

/*---------------------------------------------------------------------------*/
void init_Shmem_Pingpong_Put_Put( int count, int iterations ) {
    /*onesided_winsize = get_extent(count, datatype);

  set_send_buffer_usage(onesided_winsize);
  set_reported_message_size(onesided_winsize);

  set_recv_buffer_usage(onesided_winsize);*/

  init_synchronization();
}


double measure_Shmem_Pingpong_Put_Put( int count, int iterations ){
  double start_time = 1.0, end_time = 0.0;
  char* sym;
  int i;

  if (iterations<0) {
    return -1.0;   /* indicate that this definitely is an error */
  }
  if (iterations==0) {
    return 0.0;    /* avoid division by zero at the end */
  }

  sym = shmem_malloc( count );
  shmem_fence();
  start_time = start_synchronization();

  for (i=0; i<iterations; i++) {
      if (get_measurement_rank() == 0){
          shmem_putmem( sym, get_send_buffer(), count, 1 );
     }
      shmem_fence();
      if (get_measurement_rank() == 1){
          shmem_putmem( sym, get_recv_buffer(), count, 0 );
     }
      shmem_fence();
  }

  end_time = stop_synchronization();
  shmem_free( sym );
  return (end_time - start_time)/iterations;
}

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

void init_Shmem_Bcast_All( int count, int root ){
  size = shmem_n_pes();
  source = (char*) shmem_malloc( count );
  target = (char*) shmem_malloc( count );
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
  psync = malloc( SHMEM_BCAST_SYNC_SIZE );
#endif
  init_synchronization();
}
  
void finalize_Shmem_Bcast_All( int count, int root ){
  shmem_free( source );
  shmem_free( target );
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
  free( psync );
#endif
}

/* TODO not necesarily on SHMEM_TEAM_WORLD */

double measure_Shmem_Bcast_All( int count, int root ){
    double start_time, end_time;

    start_time = start_synchronization();

#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION >= 5
    shmem_broadcastmem( SHMEM_TEAM_WORLD, target, source, count, root );
#else
    shmem_broadcast32( target, source, count/4, root, 0, 0, size, (void*)psync );
#endif
    end_time = stop_synchronization();
    return  end_time - start_time;
}

/* This benchmark method is called the `broadcast rounds` benchmark method: each rank 
   of the communicator / team initiates a broadcast. There is still a possibillity
   of pipeling, depending on the order used to cycle over the tasks. 
*/

void init_Shmem_Bcast_All_Rounds( int count ){
  size = shmem_n_pes();
  source = (char*) shmem_malloc( count );
  target = (char*) shmem_malloc( count );
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
  psync = malloc( SHMEM_BCAST_SYNC_SIZE );
#endif
  init_synchronization();
}
  
void finalize_Shmem_Bcast_All_Rounds( int count ){
  shmem_free( source );
  shmem_free( target );
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
  free( psync );
#endif
}

/* TODO not necesarily on SHMEM_TEAM_WORLD */

double measure_Shmem_Bcast_All_Rounds( int count ){
    double start_time, end_time;
    int root;
    
    start_time = start_synchronization();
    for( root = 0 ; root < size ; root++ ){
#if _SHMEM_MAJOR_VERSION >= 1 && _SHMEM_MINOR_VERSION >= 5
      shmem_broadcastmem( SHMEM_TEAM_WORLD, target, source, count, root );
#else
      shmem_broadcast32( target, source, count/4, root, 0, 0, size, (void*)psync );
#endif
    }
    end_time = stop_synchronization();
    return  ( end_time - start_time ) / size;
}

/* This method is described in:
   https://www.globus.org/sites/default/files/de_Supinski_HPDC08.pdf
   by Bronis R. de Supinski and Nicholas T. Karonis, published at HPDC 99, 
   DOI 10.1109/HPDC.1999.805279
*/

void init_Shmem_Bcast_All_SK( int count, int root ){
  size = shmem_n_pes();
  source = (char*) shmem_malloc( count );
  target = (char*) shmem_malloc( count );
  ack    = (int*)  shmem_malloc( sizeof( int ) );
  *ack = 0;
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
  psync = malloc( SHMEM_BCAST_SYNC_SIZE );
#endif
  init_synchronization();
}
  
void finalize_Shmem_Bcast_All_SK( int count, int root ){
  shmem_free( source );
  shmem_free( target );
  shmem_free( ack );
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
  free( psync );
#endif
}

/* TODO not necesarily on SHMEM_TEAM_WORLD */

double measure_Shmem_Bcast_All_SK( int count, int root ){
  double start_time, t1, rt1, t2, rt2, mytime;
  int rank = shmem_my_pe();
  int i, dst, task;

  /* The algorithm is written with two-sided communications. We are using atomic puts instead. */
  shmem_fence();
  shmem_barrier_all();

  for( task = 0 ; task < size ; task++ ){
      if( task != root ) {
          
          /* First step: one-to-one communications */
          
          start_time = wtime();
          if( root == rank ){
              for( i = 0 ; i < M ; i++ ){
                  shmem_int_atomic_add( ack, 1, task );
                  wait_for_ack( 1 );
              }
          } else {
              if( task == rank ){
                  for( i = 0 ; i < M ; i++ ){
                      wait_for_ack( 1 );
                      shmem_int_atomic_add( ack, 1, root );
                  }
              }
          }
          
          t1 = wtime();
          rt1 = ( t1 - start_time ) / M;
          
          /* Second step: broadcasts, with ack. */
          /* Assumptions:
             - the acknowledgment does not arrive at the root before the root has
             finished the broadcast 
             - no task j on i's broadcast path delays the next broadcast 
          */
          
          shmem_barrier_all();
          
          /* Initial broadcast / ack: assumption consistency check */
#if _SHMEM_MAJOR_VERSION >= 1 && _SHMEM_MINOR_VERSION >= 5
          shmem_broadcastmem( SHMEM_TEAM_WORLD, target, source, count, root );
#else
          shmem_broadcast32( target, source, count/4, root, 0, 0, size, (void*)psync );
#endif
          if( root == rank ){
              wait_for_ack( 1 );
          } else {
              if( task == rank ) {
                  shmem_int_atomic_add( ack, 1, root );
              }
          }
          
          /* Mesurements */
          
          t1 = wtime();
          for( i = 0 ; i < M ; i++ ){
              
#if _SHMEM_MAJOR_VERSION >= 1 && _SHMEM_MINOR_VERSION >= 5
              shmem_broadcastmem( SHMEM_TEAM_WORLD, target, source, count, root );
#else
              shmem_broadcast32( target, source, count/4, root, 0, 0, size, (void*)psync );
#endif
              if( root == rank ){
                  wait_for_ack( 1 );
              } else {
                  if( task == rank ) {
                      shmem_int_atomic_add( ack, 1, root );
                  }
              }
          }
          t2 = wtime();
          
          if( rank == task || root == rank ){ /* the root will keep the last time taken */
              rt2 = (t2 - t1)/M;
              mytime = rt2 - rt1/2;
          }
      }
      
  }
  
  return mytime;
}

/*---------------------------------------------------------------------------*/
/* TODO other operations */

void init_Shmem_Reduce_And_All( int count, int root ){
  size = shmem_n_pes();
  source = (char*) shmem_malloc( count );
  target = (char*) shmem_malloc( count );
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
  psync = (char*) malloc( SHMEM_BCAST_SYNC_SIZE );
  pwrk  = (char*) malloc( max(count/2 + 1, SHMEM_REDUCE_MIN_WRKDATA_SIZE) );
#endif
  init_synchronization();
}
  
void finalize_Shmem_Reduce_And_All( int count, int root ){
  shmem_free( source );
  shmem_free( target );
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
  free( psync );
  free( pwrk );
#endif
}

/* TODO not necesarily on SHMEM_TEAM_WORLD */

double measure_Shmem_Reduce_And_All( int count, int root ){
    double start_time, end_time;

    start_time = start_synchronization();

#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION >= 5
    shmem_short_and_reduce( SHMEM_TEAM_WORLD, (short*)target, (short*)source, count/sizeof( short ) );
#else
    shmem_short_and_to_all( (short*)target, (short*)source, count/sizeof( short ), root, 0, size, (short*)pwrk, (long*)psync );
#endif
    end_time = stop_synchronization();
    return  end_time - start_time;
}

/* Rounds method. Cf Shmem_Bcast_All_Rounds. */

void init_Shmem_Reduce_And_All_Rounds( int count ){
  size = shmem_n_pes();
  source = (char*) shmem_malloc( count );
  target = (char*) shmem_malloc( count );
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
  psync = (char*) malloc( SHMEM_BCAST_SYNC_SIZE );
  pwrk  = (char*) malloc( max(count/2 + 1, SHMEM_REDUCE_MIN_WRKDATA_SIZE) );
#endif
  init_synchronization();
}
  
void finalize_Shmem_Reduce_And_All_Rounds( int count ){
  shmem_free( source );
  shmem_free( target );
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
  free( psync );
  free( pwrk );
#endif
}

/* TODO not necesarily on SHMEM_TEAM_WORLD */

double measure_Shmem_Reduce_And_All_Rounds( int count ){
  double start_time, end_time;
  int root;
  
  start_time = start_synchronization();
  
  for( root = 0 ; root < size ; root++ ){
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION >= 5
    shmem_short_and_reduce( SHMEM_TEAM_WORLD, (short*)target, (short*)source, count/sizeof( short ) );
#else
    shmem_short_and_to_all( (short*)target, (short*)source, count/sizeof( short ), root, 0, size, (short*)pwrk, (long*)psync );
#endif
  }
  end_time = stop_synchronization();
  return  ( end_time - start_time ) / size;
}

/* This algorithm is based on the algorithm described by Bronis de Supinski and Nicholas Karonis
   and implemented in measure_Shmem_Bcast_All_SK(). The processes perfom a reduction, and the root
   acknowledges it to a process (one for each round). 
*/

void init_Shmem_Reduce_And_All_SK( int count, int root ){
  size = shmem_n_pes();
  source = (char*) shmem_malloc( count );
  target = (char*) shmem_malloc( count );
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
  psync = (char*) malloc( SHMEM_BCAST_SYNC_SIZE );
  pwrk  = (char*) malloc( max(count/2 + 1, SHMEM_REDUCE_MIN_WRKDATA_SIZE) );
#endif
  init_synchronization();
}

void finalize_Shmem_Reduce_And_All_SK( int count, int root ){
  shmem_free( source );
  shmem_free( target );
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
  free( psync );
  free( pwrk );
#endif
}

/* TODO not necesarily on SHMEM_TEAM_WORLD */

double measure_Shmem_Reduce_And_All_SK( int count, int root ){
  double start_time, t1, rt1, t2, rt2, mytime;
  int rank = shmem_my_pe();
  int i, dst, task;

  /* The algorithm is written with two-sided communications. We are using atomic puts instead. */
  shmem_fence();
  shmem_barrier_all();
  //start_time = start_synchronization();

  for( task = 0 ; task < size ; task++ ){
    if( task != root ) {

        start_time = wtime();
      /* First step: one-to-one communications */
      
      if( root == rank ){
	for( i = 0 ; i < M ; i++ ){
	  shmem_int_atomic_add( ack, 1, task );
	  wait_for_ack( 1 );
	}
      } else {
	if( task == rank ){
	  for( i = 0 ; i < M ; i++ ){
	    wait_for_ack( 1 );
	    shmem_int_atomic_add( ack, 1, root );
	  }
	}
      }
      
      t1 = wtime();
      rt1 = ( t1 - start_time ) / M;
      
      /* Second step: reductions, with ack. */
      
      shmem_barrier_all();
      
      /* Initial broadcast / ack: assumption consistency check */
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION >= 5
    shmem_short_and_reduce( SHMEM_TEAM_WORLD, (short*)target, (short*)source, count/sizeof( short ) );
#else
    shmem_short_and_to_all( (short*)target, (short*)source, count/sizeof( short ), root, 0, size, (short*)pwrk, (long*)psync );
#endif
      if( root == rank ){
	shmem_int_atomic_add( ack, 1, task );
      } else {
	if( task == rank ) {
	  wait_for_ack( 1 );
	}
      }
  
      /* Mesurements */
      
      t1 = wtime();

      for( i = 0 ; i < M ; i++ ){
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION >= 5
	shmem_short_and_reduce( SHMEM_TEAM_WORLD, (short*)target, (short*)source, count/sizeof( short ) );
#else
	shmem_short_and_to_all( (short*)target, (short*)source, count/sizeof( short ), root, 0, size, (short*)pwrk, (long*)psync );
#endif
	if( root == rank ){
	  shmem_int_atomic_add( ack, 1, task );
	} else {
	  if( task == rank ) {
	    wait_for_ack( 1 );
	  }
	}
      }
      t2 = wtime();
      
      if( rank == task || root == rank ){ /* the root will keep the last time taken */
	rt2 = (t2 - t1)/M;
	mytime = rt2 - rt1/2;
      }
    }

  }

  return mytime;
}

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

double measure_Shmem_Malloc( int size ){
  double start_time = 1.0, end_time = 0.0;
  char* ptr;

  start_time = start_synchronization();
  ptr = (char*)shmem_malloc( size );
  end_time = stop_synchronization();

  shfree( ptr );
  return end_time - start_time;
}

/*---------------------------------------------------------------------------*/

double measure_Shmem_Free(){
  double start_time = 1.0, end_time = 0.0;
  char* ptr;
  ptr = (char*)shmem_malloc( size );

  start_time = start_synchronization();
  shfree( ptr );
  end_time = stop_synchronization();

  return end_time - start_time;
}

/*---------------------------------------------------------------------------*/

/* Using this function, only process 0 will have the time 
*/

/* TODO cf measure_Shmem_Lock_Test_Busy_All */

double measure_Shmem_Lock_Test_Busy(){

    double t1 = 1.0, t2 = 0.0, btime;
    static long lock = 0;
    static int cnt = 0;
    int val;
    size = get_measurement_size();
    if( size < 2 ){
        return -1.0;
    }
    
    /* Measure the time to perform a barrier */
    t1 = wtime();
    shmem_barrier_all();
    t2 = wtime();
    btime = t2 - t1;
    
    /* Process 1 takes the lock, holds it during 10*btime */
    if( get_measurement_rank() == 1 ){
        shmem_set_lock( &lock );
        val = 1;
        shmem_int_put( &cnt, &val, 1, 0 );
        shmem_barrier_all();
        usleep( (int) btime*10 );
        shmem_clear_lock( &lock );
        lock = 0; /* might be a bug causing this necessity */
    } else {
        shmem_barrier_all();
    }

    /* Process 0 tests the lock */
    if( get_measurement_rank() == 0 ){
        t1 = wtime();
        shmem_test_lock( &lock );
        t2 = wtime();
        return t2 - t1;
    }
    return -1.0;
}

/*---------------------------------------------------------------------------*/

/* Using this function, all the processes but the last one are testing the lock. 
   The lock is held by the lst process.
*/
void init_Shmem_Lock_Test_Busy_All(){
    int rank, size, last;
    rank = shmem_my_pe();
    size = shmem_n_pes();
    last = size - 1;
    if( rank == last ){
        shmem_set_lock( &mylock );
    }
}

void finalize_Shmem_Lock_Test_Busy_All(){
    int rank, size, last;
    rank = shmem_my_pe();
    size = shmem_n_pes();
    last = size - 1;
    if( rank == last ){
        shmem_clear_lock( &mylock );
    }
}

double measure_Shmem_Lock_Test_Busy_All(){

    double t1 = 1.0, t2 = 0.0, btime;
    static long lock = 0;
    static int cnt = 0;
    int val;
    int rank, size, last;
    rank = shmem_my_pe();
    size = shmem_n_pes();
    last = size - 1;
    
    if( size < 2 ){
        return -1.0;
    }
        
    /* All the other ones try to test it */
    if( rank != last ){
        t1 = wtime();
        shmem_test_lock( &mylock );
        t2 = wtime();
    }

    shmem_barrier_all();

    if( rank != last )
        return t2 - t1;
    else
        return -1.0;
}

/*---------------------------------------------------------------------------*/

/* All the processes are trying to get the lock, in turns.
   The lock is held by the last process.  */

void init_Shmem_Lock_Test_Busy_Turns(){
    int rank, size, last;
    rank = shmem_my_pe();
    size = shmem_n_pes();
    last = size - 1;
    if( rank == last ){
        shmem_set_lock( &mylock );
    }
}

void finalize_Shmem_Lock_Test_Busy_Turns(){
    int rank, size, last;
    rank = shmem_my_pe();
    size = shmem_n_pes();
    last = size - 1;
    if( rank == last ){
        shmem_clear_lock( &mylock );
    }
}

double measure_Shmem_Lock_Test_Busy_Turns(){

    double t1 = 1.0, t2 = 0.0;
    int i;
    int rank, size;
    rank = shmem_my_pe();
    size = shmem_n_pes();
    
    if( size < 2 ){
        return -1.0;
    }
        
    /* All the other ones try to test it */
    for( i = 0 ; i < size ; i++ ){
        if( rank == i ){
            t1 = wtime();
            shmem_test_lock( &mylock );
            t2 = wtime();
        }
        shmem_barrier_all();
    }

    shmem_barrier_all();

    return t2 - t1;
}

/*---------------------------------------------------------------------------*/

/* Using this function, each process tests the lock in turns. 
   A process holds the lock and another tests it.
 */

double measure_Shmem_Lock_Test_Busy_Round(){
    
    double t1, t2, tt = 0.0, btime;
    static long lock = 0;
    int i, locker;
    int rank, size;
    rank = shmem_my_pe();
    size = shmem_n_pes();
    if( get_measurement_size() < 2 ){
        return -1.0;
    }
    
    /* Measure the time to perform a barrier */
    t1 = wtime();
    shmem_barrier_all();
    t2 = wtime();
    btime = t2 - t1;

    for( locker = 0 ; locker <  size ; locker++ ) {
        /* A process takes the lock */
        if( rank == locker ){
            shmem_set_lock(&lock);
            for( i = 0 ; i < size + 1; i++ ) {
                shmem_barrier_all();
                usleep( btime );
            }
            shmem_clear_lock(&lock);
        } else {
            /* All the processes test it in turn */
            for( i = 0 ; i < size ; i++ ){
                
                shmem_barrier_all();
                if( i == rank ){
                    t1 = wtime();
                    shmem_test_lock( &lock );
                    t2 = wtime();
                }
                tt += t2 - t1;
            }
            shmem_barrier_all();
        }
        
        shmem_barrier_all();
    }

    return tt / (double)(size-1);

}

/*---------------------------------------------------------------------------*/

void init_Shmem_Set_Lock(){
    init_synchronization();
}

double measure_Shmem_Set_Lock(){
    double start_time = 1.0, end_time = 0.0, t1 = 1.0, t2 = 0.0;
    int rank, size, i;
    static long lock = 0;
    start_time = start_synchronization();
    
    rank = shmem_my_pe();
    size = shmem_n_pes();

    for( i = 0 ; i < size ; i++ ) {
        shmem_barrier_all();
        if( rank == i ){
            t1 = wtime();
            shmem_set_lock( &lock );
            t2 = wtime();
            shmem_clear_lock( &lock );
        }
    }
    end_time = stop_synchronization();

    return t2 - t1;
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Clear_Lock( ){
    init_synchronization();
}

double measure_Shmem_Clear_Lock(){
    double start_time = 1.0, end_time = 0.0, t1 = 1.0, t2 = 0.0;
    int rank, size, i;
    static long lock = 0;
    start_time = start_synchronization();
    
    rank = shmem_my_pe();
    size = shmem_n_pes();

    for( i = 0 ; i < size ; i++ ) {
        shmem_barrier_all();
        if( rank == i ){
            shmem_set_lock( &lock );
            t1 = wtime();
            shmem_clear_lock( &lock );
            t2 = wtime();
        }
    }
    end_time = stop_synchronization();

    return t2 - t1;
}

/*---------------------------------------------------------------------------*/

#pragma weak end_skampi_extensions

#endif // SKAMPI_OPENSHMEM

