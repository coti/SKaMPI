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

void init_Shmem_Bcast_All_SK( int count, int root, int iterations ){
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
  
void finalize_Shmem_Bcast_All_SK( int count, int root, int iterations ){
  shmem_free( source );
  shmem_free( target );
  shmem_free( ack );
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
  free( psync );
#endif
}

/* TODO not necesarily on SHMEM_TEAM_WORLD */

double measure_Shmem_Bcast_All_SK( int count, int root, int iterations ){
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
              for( i = 0 ; i < iterations ; i++ ){
                  shmem_int_atomic_fetch_inc( ack, task );
                  shmem_int_wait_until( ack, SHMEM_CMP_EQ, 1 );
                  *ack = 0;
              }
          } else {
              if( task == rank ){
                  for( i = 0 ; i < iterations ; i++ ){
                      shmem_int_wait_until( ack, SHMEM_CMP_EQ, 1 );
                      *ack = 0;
                      shmem_int_atomic_fetch_inc( ack, root );
                  }
              }
          }           
          t1 = wtime();
          rt1 = ( t1 - start_time );
          
          /* Second step: broadcasts, with ack. */
          /* Assumptions:
             - the acknowledgment does not arrive at the root before the root has
             finished the broadcast 
             - no task j on i's broadcast path delays the next broadcast 
          */
          
          *ack = 0;
          shmem_barrier_all();
          
         /* Initial broadcast / ack: assumption consistency check */
#if _SHMEM_MAJOR_VERSION >= 1 && _SHMEM_MINOR_VERSION >= 5
          shmem_broadcastmem( SHMEM_TEAM_WORLD, target, source, count, root );
#else
          shmem_broadcast32( target, source, count/4, root, 0, 0, size, (void*)psync );
#endif
          if( root == rank ){
              shmem_int_atomic_fetch_inc( ack, task );
              shmem_int_wait_until( ack, SHMEM_CMP_EQ, 1 );
          } else {
              if( task == rank ) {
                  shmem_int_wait_until( ack, SHMEM_CMP_EQ, 1 );
                  shmem_int_atomic_fetch_inc( ack, root );
              }
          }
          
          *ack = 0;
          shmem_barrier_all();
          
          /* Mesurements */
          
          t1 = wtime();
          for( i = 0 ; i < iterations ; i++ ){

              if( source == rank ) source[0] = 1;
              
#if _SHMEM_MAJOR_VERSION >= 1 && _SHMEM_MINOR_VERSION >= 5
              shmem_broadcastmem( SHMEM_TEAM_WORLD, target, source, count, root );
#else
              shmem_broadcast32( target, source, count/4, root, 0, 0, size, (void*)psync );
#endif

              shmem_quiet(); // or shmem_fence
              
              if( root == rank ){
                  shmem_int_atomic_fetch_inc( ack, task );
                  shmem_int_wait_until( ack, SHMEM_CMP_EQ, 1 );
                  *ack = 0;
              } else {
                  if( task == rank ) {
                      shmem_int_wait_until( ack, SHMEM_CMP_EQ, 1 );
                      *ack = 0;
                      shmem_int_atomic_fetch_inc( ack, root );
                  }
              }
              
          }
          t2 = wtime();
          
          if( rank == task || root == rank ){ /* the root will keep the last time taken */
              rt2 = t2 - t1;
              mytime = (rt2 - rt1)/iterations;
          }
      }
  }
  t1 = stop_synchronization();

  return mytime;
}

/*---------------------------------------------------------------------------*/
/*                              Reductions                                   */
/*---------------------------------------------------------------------------*/
/* TODO other operations */
/* and, or, xor, max, min, sum, prod */

void tini_shmem_reduce( int count ){
  size = shmem_n_pes();
  source = (char*) shmem_malloc( count );
  target = (char*) shmem_malloc( count );
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
  psync = (char*) malloc( SHMEM_BCAST_SYNC_SIZE );
  pwrk  = (char*) malloc( max(count/2 + 1, SHMEM_REDUCE_MIN_WRKDATA_SIZE) );
#endif
  init_synchronization();
}

void ezilanif_shmem_reduce( int count ){
  shmem_free( source );
  shmem_free( target );
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
  free( psync );
  free( pwrk );
#endif
}

#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION >= 5
double erusaem_shmem_reduce_consecutive( int count, int iterations, void (*routine)( team, short, const short*, size_t) ){
#else
double erusaem_shmem_reduce_consecutive( int count, int iterations, void (*routine)( short*, const short*, int, int, int, int, short*, long* ) ){
#endif
    int i;
    double start_time = 1.0, end_time = 0.0;
    start_time = start_synchronization();

    for( i = 0 ; i < iterations ; i++ ){
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION >= 5
        routine( SHMEM_TEAM_WORLD, (short*)target, (short*)source, count/sizeof( short ) );
#else
        routine( (short*)target, (short*)source, count/sizeof( short ), 0, 0, size, (short*)pwrk, (long*)psync );
#endif
    }
    
    end_time = stop_synchronization();
    return  ( end_time - start_time) / iterations;
}

#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION >= 5
double erusaem_shmem_reduce_barrier( int count, int iterations, void (*routine)( team, short, const short*, size_t) ){
#else
double erusaem_shmem_reduce_barrier( int count, int iterations, void (*routine)( short*, const short*, int, int, int, int, short*, long* ) ){
#endif
    int i;
    double start_time = 1.0, end_time = 0.0, btime, ttime;
    
    /* Warm-up */
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION >= 5
        routine( SHMEM_TEAM_WORLD, (short*)target, (short*)source, count/sizeof( short ) );
#else
        routine( (short*)target, (short*)source, count/sizeof( short ), 0, 0, size, (short*)pwrk, (long*)psync );
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
        routine( SHMEM_TEAM_WORLD, (short*)target, (short*)source, count/sizeof( short ) );
#else
        routine( (short*)target, (short*)source, count/sizeof( short ), 0, 0, size, (short*)pwrk, (long*)psync );
#endif
        shmem_barrier_all();
        end_time = wtime();
        ttime += (end_time - start_time );
    }
    end_time = stop_synchronization();

    return  ( ttime - btime ) / iterations;
}

#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION >= 5
double erusaem_shmem_reduce_synchro( int count, void (*routine)( team, short, const short*, size_t) ){
#else
double erusaem_shmem_reduce_synchro( int count, void (*routine)( short*, const short*, int, int, int, int, short*, long* ) ){
#endif
    int i;
    double start_time = 1.0, end_time = 0.0;
    start_time = start_synchronization();

#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION >= 5
    routine( SHMEM_TEAM_WORLD, (short*)target, (short*)source, count/sizeof( short ) );
#else
    routine( (short*)target, (short*)source, count/sizeof( short ), 0, 0, size, (short*)pwrk, (long*)psync );
#endif
    
    end_time = stop_synchronization();
    return  ( end_time - start_time);
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Reduce_And_Consecutive( int count, int iterations ){
    tini_shmem_reduce( count );
 }
  
void finalize_Shmem_Reduce_And_Consecutive( int count, int iterations ){
    ezilanif_shmem_reduce( count );
}

/* TODO not necesarily on SHMEM_TEAM_WORLD */
/* TODO other operations */

double measure_Shmem_Reduce_And_Consecutive( int count, int iterations ){
    return erusaem_shmem_reduce_consecutive( count, iterations, &shmem_short_and_to_all );
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Reduce_And_Barrier( int count, int iterations ){
    tini_shmem_reduce( count );
 }
  
void finalize_Shmem_Reduce_And_Barrier( int count, int iterations ){
    ezilanif_shmem_reduce( count );
}

double measure_Shmem_Reduce_And_Barrier( int count, int iterations ){
    return erusaem_shmem_reduce_barrier( count, iterations, &shmem_short_and_to_all );
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Reduce_And_Synchro( int count ){
    tini_shmem_reduce( count );
 }
  
void finalize_Shmem_Reduce_And_Synchro( int count ){
    ezilanif_shmem_reduce( count );
}

double measure_Shmem_Reduce_And_Synchro( int count ){
    return erusaem_shmem_reduce_synchro( count, &shmem_short_and_to_all );
}

/*---------------------------------------------------------------------------*/
/*                              Collect                                      */
/*---------------------------------------------------------------------------*/

void tini_shmem_collect( int count ){
  size = shmem_n_pes();
  source = (char*) shmem_malloc( count );
  target = (char*) shmem_malloc( count*size );
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
  psync = (char*) malloc( SHMEM_BCAST_SYNC_SIZE );
#endif
  init_synchronization();
}

void ezilanif_shmem_collect( int count ){
  shmem_free( source );
  shmem_free( target );
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
  free( psync );
#endif
}
 
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION >= 5
double erusaem_shmem_collect_consecutive( int count, int iterations, void (*routine)( team, void*, const void*, size_t) ){
#else
double erusaem_shmem_collect_consecutive( int count, int iterations, void (*routine)( void*, const void*, size_t, int, int, int, long *)){
#endif
    int i;
    double start_time = 1.0, end_time = 0.0;
    start_time = start_synchronization();

    for( i = 0 ; i < iterations ; i++ ){
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION >= 5
        routine( SHMEM_TEAM_WORLD, (void*)target, (void*)source, count );
#else
        routine( (void*)target, (void*)source, count/4, 0, 0, size, (long*)psync );
#endif
    }
    
    end_time = stop_synchronization();
    return  ( end_time - start_time) / iterations; 
}
    
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION >= 5
double erusaem_shmem_collect_barrier( int count, int iterations, void (*routine)( team, void*, const void*, size_t) ){
#else
double erusaem_shmem_collect_barrier( int count, int iterations, void (*routine)( void*, const void*, size_t, int, int, int, long *)){
#endif
    int i;
    double start_time = 1.0, end_time = 0.0, btime, ttime = 0.0;
    
    /* Warm-up */
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION >= 5
    routine( SHMEM_TEAM_WORLD, (void*)target, (void*)source, count );
#else
    routine( (void*)target, (void*)source, count/4, 0, 0, size, (long*)psync );
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
        routine( SHMEM_TEAM_WORLD, (void*)target, (void*)source, count );
#else
        routine( (void*)target, (void*)source, count/4, 0, 0, size, (long*)psync );
#endif
        shmem_barrier_all();
        end_time = wtime();
        ttime += (end_time - start_time );
    }
    end_time = stop_synchronization();

    return  ( ttime - btime ) / iterations;
}

#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION >= 5
double erusaem_shmem_collect_synchro( int count, void (*routine)( team, void*, const void*, size_t) ){
#else
double erusaem_shmem_collect_synchro( int count, void (*routine)( void*, const void*, size_t, int, int, int, long *)){
#endif
    int i;
    double start_time = 1.0, end_time = 0.0;
    start_time = start_synchronization();
    
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION >= 5
    routine( SHMEM_TEAM_WORLD, (void*)target, (void*)source, count );
#else
    routine( (void*)target, (void*)source, count/4, 0, 0, size, (long*)psync );
#endif
    
    end_time = stop_synchronization();
    return  ( end_time - start_time);
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Collect_Consecutive( int count, int iterations ){
    tini_shmem_collect( count );
 }
  
void finalize_Shmem_Collect_Consecutive( int count, int iterations ){
    ezilanif_shmem_collect( count );
}

double measure_Shmem_Collect_Consecutive( int count, int iterations ){
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
    return erusaem_shmem_collect_consecutive( count, iterations, &shmem_collect32 );
#else
    return erusaem_shmem_collect_consecutive( count, iterations, &shmem_collectmem );
#endif
}
 
void init_Shmem_Fcollect_Consecutive( int count, int iterations ){
    tini_shmem_collect( count );
 }
  
void finalize_Shmem_Fcollect_Consecutive( int count, int iterations ){
    ezilanif_shmem_collect( count );
}
 
double measure_Shmem_Fcollect_Consecutive( int count, int iterations ){
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
    return erusaem_shmem_collect_consecutive( count, iterations, &shmem_fcollect32 );
#else
    return erusaem_shmem_collect_consecutive( count, iterations, &shmem_fcollectmem );
#endif
}
 
/*---------------------------------------------------------------------------*/

void init_Shmem_Collect_Barrier( int count, int iterations ){
    tini_shmem_collect( count );
 }
  
void finalize_Shmem_Collect_Barrier( int count, int iterations ){
    ezilanif_shmem_collect( count );
}

double measure_Shmem_Collect_Barrier( int count, int iterations ){
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
    return erusaem_shmem_collect_barrier( count, iterations, &shmem_collect32 );
#else
    return erusaem_shmem_collect_barrier( count, iterations, &shmem_collectmem );
#endif
}

void init_Shmem_Fcollect_Barrier( int count, int iterations ){
    tini_shmem_collect( count );
 }
  
void finalize_Shmem_Fcollect_Barrier( int count, int iterations ){
    ezilanif_shmem_collect( count );
}

double measure_Shmem_Fcollect_Barrier( int count, int iterations ){
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
    return erusaem_shmem_collect_barrier( count, iterations, &shmem_fcollect32 );
#else
    return erusaem_shmem_collect_barrier( count, iterations, &shmem_fcollectmem );
#endif
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Collect_Synchro( int count ){
    tini_shmem_collect( count );
 }
  
void finalize_Shmem_Collect_Synchro( int count ){
    ezilanif_shmem_collect( count );
}

double measure_Shmem_Collect_Synchro( int count ){
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
    return erusaem_shmem_collect_synchro( count, &shmem_collect32 );
#else
    return erusaem_shmem_collect_synchro( count, &shmem_collectmem );
#endif
}

void init_Shmem_Fcollect_Synchro( int count ){
    tini_shmem_collect( count );
 }
  
void finalize_Shmem_Fcollect_Synchro( int count ){
    ezilanif_shmem_collect( count );
}

double measure_Shmem_Fcollect_Synchro( int count ){
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
    return erusaem_shmem_collect_synchro( count, &shmem_fcollect32 );
#else
    return erusaem_shmem_collect_synchro( count, &shmem_fcollectmem );
#endif
}
 
/*---------------------------------------------------------------------------*/
/*                             All-to-all                                    */
/*---------------------------------------------------------------------------*/

void tini_shmem_alltoall( int count ){
  size = shmem_n_pes();
  source = (char*) shmem_malloc( count*size );
  target = (char*) shmem_malloc( count*size );
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
  psync = (char*) malloc( SHMEM_BCAST_SYNC_SIZE );
#endif
  init_synchronization();
}

void ezilanif_shmem_alltoall( int count ){
  shmem_free( source );
  shmem_free( target );
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
  free( psync );
#endif
}
 
#if _SHMEM_MAJOR_VERSION >= 1 && _SHMEM_MINOR_VERSION >= 5
double erusaem_shmem_alltoalls_consecutive( int count, int iterations,
                                            void (*routine)( team, void*, const void*, ptrdiff_t, ptrdiff_t, size_t) ){
#else
double erusaem_shmem_alltoalls_consecutive( int count, int iterations,
                                            void (*routine)( void*, const void*, ptrdiff_t, ptrdiff_t, size_t, int, int, int, long *)){
#endif

    int i;
    double start_time = 1.0, end_time = 0.0;
    ptrdiff_t dst = 1, sst = 1;
    
    start_time = start_synchronization();

    for( i = 0 ; i < iterations ; i++ ){
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION >= 5
        routine( SHMEM_TEAM_WORLD, (void*)target, (void*)source, dst, sst, count );
#else
        routine( (void*)target, (void*)source, dst, sst, count/4, 0, 0, size, (long*)psync );
#endif
    }
    
    end_time = stop_synchronization();
    return  ( end_time - start_time) / iterations; 
}
    
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION >= 5
double erusaem_shmem_alltoalls_barrier( int count, int iterations, void (*routine)( team, void*, const void*, ptrdiff_t, ptrdiff_t, size_t) ){
#else
double erusaem_shmem_alltoalls_barrier( int count, int iterations, void (*routine)( void*, const void*, ptrdiff_t, ptrdiff_t, size_t, int, int, int, long *)){
#endif
    int i;
    double start_time = 1.0, end_time = 0.0, btime, ttime = 0.0;
    ptrdiff_t dst = 1, sst = 1;

    /* Warm-up */
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION >= 5
    routine( SHMEM_TEAM_WORLD, (void*)target, (void*)source, dst, sst, count );
#else
    routine( (void*)target, (void*)source, dst, sst, count/4, 0, 0, size, (long*)psync );
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
        routine( SHMEM_TEAM_WORLD, (void*)target, (void*)source, dst, sst, count );
#else
        routine( (void*)target, (void*)source, dst, sst, count/4, 0, 0, size, (long*)psync );
#endif
        shmem_barrier_all();
        end_time = wtime();
        ttime += (end_time - start_time );
    }
    end_time = stop_synchronization();

    return  ( ttime - btime ) / iterations;
}

#if _SHMEM_MAJOR_VERSION >= 1 && _SHMEM_MINOR_VERSION >= 5
double erusaem_shmem_alltoalls_synchro( int count, void (*routine)( team, void*, const void*, ptrdiff_t, ptrdiff_t, size_t) ){
#else
double erusaem_shmem_alltoalls_synchro( int count, void (*routine)( void*, const void*, ptrdiff_t, ptrdiff_t, size_t, int, int, int, long *)){
#endif
    int i;
    double start_time = 1.0, end_time = 0.0;
    ptrdiff_t dst = 1, sst = 1;
    start_time = start_synchronization();
    
#if _SHMEM_MAJOR_VERSION >= 1 && _SHMEM_MINOR_VERSION >= 5
    routine( SHMEM_TEAM_WORLD, (void*)target, (void*)source, dst, sst, count );
#else
    routine( (void*)target, (void*)source, dst, sst, count/4, 0, 0, size, (long*)psync );
#endif
    
    end_time = stop_synchronization();
    return  ( end_time - start_time);
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Alltoall_Consecutive( int count, int iterations ){
    tini_shmem_alltoall( count );
 }
  
void finalize_Shmem_Alltoall_Consecutive( int count, int iterations ){
    ezilanif_shmem_alltoall( count );
}

double measure_Shmem_Alltoall_Consecutive( int count, int iterations ){
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5 /* Same API, same measurement */
    return erusaem_shmem_collect_consecutive( count, iterations, &shmem_alltoall32 );
#else
    return erusaem_shmem_collect_consecutive( count, iterations, &shmem_alltoallmem );
#endif
}

void init_Shmem_Alltoalls_Consecutive( int count, int iterations ){
    tini_shmem_alltoall( count );
 }
  
void finalize_Shmem_Alltoalls_Consecutive( int count, int iterations ){
    ezilanif_shmem_alltoall( count );
}

double measure_Shmem_Alltoalls_Consecutive( int count, int iterations ){
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5 /* Same API, same measurement */
    return erusaem_shmem_alltoalls_consecutive( count, iterations, &shmem_alltoalls32 );
#else
    return erusaem_shmem_alltoalls_consecutive( count, iterations, &shmem_alltoallsmem );
#endif
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Alltoall_Barrier( int count, int iterations ){
    tini_shmem_alltoall( count );
 }
  
void finalize_Shmem_Alltoall_Barrier( int count, int iterations ){
    ezilanif_shmem_alltoall( count );
}

double measure_Shmem_Alltoall_Barrier( int count, int iterations ){
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5 /* Same API, same measurement */
    return erusaem_shmem_collect_barrier( count, iterations, &shmem_alltoall32 );
#else
    return erusaem_shmem_collect_barrier( count, iterations, &shmem_alltoallmem );
#endif
}

void init_Shmem_Alltoalls_Barrier( int count, int iterations ){
    tini_shmem_alltoall( count );
 }
  
void finalize_Shmem_Alltoalls_Barrier( int count, int iterations ){
    ezilanif_shmem_alltoall( count );
}

double measure_Shmem_Alltoalls_Barrier( int count, int iterations ){
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5 /* Same API, same measurement */
    return erusaem_shmem_alltoalls_barrier( count, iterations, &shmem_alltoalls32 );
#else
    return erusaem_shmem_alltoalls_barrier( count, iterations, &shmem_alltoallsmem );
#endif
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Alltoall_Synchro( int count ){
    tini_shmem_collect( count );
 }
  
void finalize_Shmem_Alltoall_Synchro( int count ){
    ezilanif_shmem_collect( count );
}

double measure_Shmem_Alltoall_Synchro( int count ){
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5
    return erusaem_shmem_collect_synchro( count, &shmem_alltoall32 );
#else
    return erusaem_shmem_collect_synchro( count, &shmem_alltoallmem );
#endif
}

void init_Shmem_Alltoalls_Synchro( int count ){
    tini_shmem_alltoall( count );
 }
  
void finalize_Shmem_Alltoalls_Synchro( int count ){
    ezilanif_shmem_alltoall( count );
}

double measure_Shmem_Alltoalls_Synchro( int count ){
#if _SHMEM_MAJOR_VERSION <= 1 && _SHMEM_MINOR_VERSION < 5 /* Same API, same measurement */
    return erusaem_shmem_alltoalls_synchro( count, &shmem_alltoalls32 );
#else
    return erusaem_shmem_alltoalls_synchro( count, &shmem_alltoallsmem );
#endif
}

/*---------------------------------------------------------------------------*/
#pragma weak end_skampi_extensions

#endif // SKAMPI_OPENSHMEM

