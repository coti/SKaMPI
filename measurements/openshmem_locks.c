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

static long mylock = 0;

/*---------------------------------------------------------------------------*/
/*                                Locks                                      */
/*---------------------------------------------------------------------------*/

/* Using this function, only process 0 will have the time 
*/

/* TODO cf measure_Shmem_Lock_Test_Busy_All */

double measure_Shmem_Lock_Test_Busy(){

    double t1 = 1.0, t2 = 0.0, btime;
    static long lock = 0;
    static int cnt = 0;
    int val;
    int size = get_measurement_size();
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

