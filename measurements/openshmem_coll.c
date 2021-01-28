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

/*---------------------------------------------------------------------------*/
/*                       Collective routines                                 */
/*---------------------------------------------------------------------------*/

void init_Shmem_Bcast_All( int count, int root, int iterations ){
  size = shmem_n_pes();
  source = (char*) shmem_malloc( count );
  target = (char*) shmem_malloc( count );
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
  psync = malloc( SHMEM_BCAST_SYNC_SIZE );
#endif
  init_synchronization();
}
  
void finalize_Shmem_Bcast_All( int count, int root, int iterations ){
  shmem_free( source );
  shmem_free( target );
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
  free( psync );
#endif
}

/* TODO not necesarily on SHMEM_TEAM_WORLD */

double measure_Shmem_Bcast_All( int count, int root, int iterations ){
    double start_time, end_time, btime, ttime = 0.0;
    int i;

    /* Warm-up */
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION >= 5
    shmem_broadcastmem( SHMEM_TEAM_WORLD, target, source, count, root );
#else
    shmem_broadcast32( target, source, count/4, root, 0, 0, size, (void*)psync );
#endif

    start_time = start_synchronization();

    /* Measure the time to perform a barrier */
    start_time = wtime();
    for( i = 0 ; i < iterations ; i++ ){
        shmem_barrier_all();
    }
    end_time = wtime();
    btime = end_time - start_time;

    /* Measure */

    shmem_barrier_all();

    for( i = 0 ; i < iterations ; i++ ){
        start_time = wtime();
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION >= 5
        shmem_broadcastmem( SHMEM_TEAM_WORLD, target, source, count, root );
#else
        shmem_broadcast32( target, source, count/4, root, 0, 0, size, (void*)psync );
#endif
        shmem_barrier_all();
        end_time = wtime();
        ttime += (end_time - start_time );
    }
    end_time = stop_synchronization();

    return  ( ttime - btime ) / iterations;
}

/* This measurement method relies on SKaMPI's synchronization routines
 */

void init_Shmem_Bcast_All_Synchro( int count, int root ){
  size = shmem_n_pes();
  source = (char*) shmem_malloc( count );
  target = (char*) shmem_malloc( count );
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
  psync = malloc( SHMEM_BCAST_SYNC_SIZE );
#endif
  init_synchronization();
}
  
void finalize_Shmem_Bcast_All_Synchro( int count , int root ){
  shmem_free( source );
  shmem_free( target );
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
  free( psync );
#endif
}

double measure_Shmem_Bcast_All_Synchro( int count, int root ){
    double start_time, end_time;
    int i;

    /* Warm-up */
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION >= 5
    shmem_broadcastmem( SHMEM_TEAM_WORLD, target, source, count, root );
#else
    shmem_broadcast32( target, source, count/4, root, 0, 0, size, (void*)psync );
#endif
    
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

void init_Shmem_Bcast_All_Rounds( int count, int iterations ){
  size = shmem_n_pes();
  source = (char*) shmem_malloc( count );
  target = (char*) shmem_malloc( count );
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
  psync = malloc( SHMEM_BCAST_SYNC_SIZE );
#endif
  init_synchronization();
}
  
void finalize_Shmem_Bcast_All_Rounds( int count, int iterations ){
  shmem_free( source );
  shmem_free( target );
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
  free( psync );
#endif
}

/* TODO not necesarily on SHMEM_TEAM_WORLD */

double measure_Shmem_Bcast_All_Rounds( int count, int iterations ){
    double start_time, end_time;
    int root, i;
    
    start_time = start_synchronization();
    for( i = 0 ; i < iterations ; i++ ){
        for( root = 0 ; root < size ; root++ ){
#if _SHMEM_MAJOR_VERSION >= 1 && _SHMEM_MINOR_VERSION >= 5
            shmem_broadcastmem( SHMEM_TEAM_WORLD, target, source, count, root );
#else
            shmem_broadcast32( target, source, count/4, root, 0, 0, size, (void*)psync );
#endif
        }
    }
    end_time = stop_synchronization();
    return  (( end_time - start_time ) / size ) / iterations;
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
  start_time = start_synchronization();
  shmem_fence();

  for( task = 0 ; task < size ; task++ ){
      if( task != root ) {
          
          /* First step: one-to-one communications */
          *ack = 0;
          shmem_barrier_all();

          start_time = wtime();
          if( root == rank ){
              for( i = 0 ; i < M ; i++ ){
                  shmem_int_atomic_add( ack, 1, task );
                  shmem_int_wait_until( ack, SHMEM_CMP_GE, 1 );
                  *ack = 0;
              }
          } else {
              if( task == rank ){
                  //          printf( "TUTU %d %d\n", rank, task );
                  for( i = 0 ; i < M ; i++ ){
                      shmem_int_wait_until( ack, SHMEM_CMP_GE, 1 );
                      *ack = 0;
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
          
          //     printf( "TITI %d %d\n", rank, task );
           *ack = 0;
          shmem_barrier_all();
          
         /* Initial broadcast / ack: assumption consistency check */
#if _SHMEM_MAJOR_VERSION >= 1 && _SHMEM_MINOR_VERSION >= 5
          shmem_broadcastmem( SHMEM_TEAM_WORLD, target, source, count, root );
#else
          shmem_broadcast32( target, source, count/4, root, 0, 0, size, (void*)psync );
#endif
          if( root == rank ){
              shmem_int_wait_until( ack, SHMEM_CMP_EQ, 1 );
              *ack = 0;
          } else {
              if( task == rank ) {
                shmem_int_atomic_add( ack, 1, root );
              }
          }

          shmem_barrier_all();
          
          /* Mesurements */
          
          t1 = wtime();
          for( i = 0 ; i < M ; i++ ){
              
#if _SHMEM_MAJOR_VERSION >= 1 && _SHMEM_MINOR_VERSION >= 5
              shmem_broadcastmem( SHMEM_TEAM_WORLD, target, source, count, root );
#else
              shmem_broadcast32( target, source, count/4, root, 0, 0, size, (void*)psync );
#endif

              if( root == rank ){
                 /* This is not possible because since these two operations are not 
                     a single, atomic operation, a remote add can arrive between
                     the wait_until and the *ack=0:
                     shmem_int_wait_until( ack, SHMEM_CMP_GE, 1 );
                     *ack = 0; */
                  shmem_int_wait_until( ack, SHMEM_CMP_GE, i+1 );
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
  t1 = stop_synchronization();

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
      shmem_int_wait_until( ack, SHMEM_CMP_EQ, 1 );
      *ack = 0;
	}
      } else {
	if( task == rank ){
	  for( i = 0 ; i < M ; i++ ){
          shmem_int_wait_until( ack, SHMEM_CMP_EQ, 1 );
          *ack = 0;
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
        shmem_int_wait_until( ack, SHMEM_CMP_EQ, 1 );
        *ack = 0;
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
        shmem_int_wait_until( ack, SHMEM_CMP_EQ, 1 );
        *ack = 0;
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

#pragma weak end_skampi_extensions

#endif // SKAMPI_OPENSHMEM

