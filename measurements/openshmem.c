#ifdef SKAMPI_OPENSHMEM

#include <mpi.h>
#include <shmem.h>
#include <stdlib.h>
#include "../mpiversiontest.h"

#include "../misc.h"
#include "../mem.h"
#include "../synchronize.h"
#include "../private_misc.h"

#pragma weak begin_skampi_extensions

char* source;
char* target;

/*---------------------------------------------------------------------------*/
/*                       Memory management routines                          */
/*---------------------------------------------------------------------------*/

double measure_Shmem_Malloc( int iterations, int size ){
    double start_time = 1.0, end_time = 0.0, ttime = 0.0;
    char* ptr;
    int i;
    
    start_time = start_synchronization();
    
    for( i = 0 ; i < iterations ; i++ ){
        start_time = wtime();
        ptr = (char*)shmem_malloc( size );
        ttime += ( wtime() - start_time );
        shfree( ptr );
    }
    end_time = stop_synchronization();
    
    return ttime / iterations;
}

/*---------------------------------------------------------------------------*/

double measure_Shmem_Free( int iterations ){
  double start_time = 1.0, end_time = 0.0, ttime = 0.0;
  char* ptr;
  int size = 4; // arbitrary
  int i;

  start_time = start_synchronization();

  for( i = 0 ; i < iterations ; i++ ){
      ptr = (char*)shmem_malloc( size );
      start_time = wtime();
      shfree( ptr );
      ttime += ( wtime() - start_time );
    }
    end_time = stop_synchronization();
    
    return ttime / iterations;
}

/*---------------------------------------------------------------------------*/

double measure_Shmem_Realloc( int iterations, int size ){
    double start_time = 1.0, end_time = 0.0, ttime = 0.0;
    int i;
    char* ptr;
    char* newptr;
    start_time = start_synchronization();
    
    for( i = 0 ; i < iterations ; i++ ){
        ptr = (char*)shmem_malloc( size/2 );
        start_time = wtime();
        newptr = (char*)shmem_realloc( ptr, size );
        ttime += ( wtime() - start_time );
        shfree( ptr );
    }
    end_time = stop_synchronization();
    
    return ttime / iterations;
}

/*---------------------------------------------------------------------------*/

double measure_Shmem_Align( int iterations, int size ){
    double start_time = 1.0, end_time = 0.0, ttime = 0.0;
    char* ptr;
    int align = 2;
    int i;

    start_time = start_synchronization();
    for( i = 0 ; i < iterations ; i++ ){
        start_time = wtime();
        ptr = (char*)shmem_align( align, size );
        ttime += ( wtime() - start_time );
        shfree( ptr );
    }      
    end_time = stop_synchronization();
    
    return ttime / iterations;
}

/*---------------------------------------------------------------------------*/
/*                    Memory ordering and synchronizations                   */
/*---------------------------------------------------------------------------*/

/* This function simply measures the time taken by a shmem_quiet call */

void init_Shmem_Quiet( int iterations ){
    init_synchronization();
}

double measure_Shmem_Quiet( int iterations ){
    double start_time = 1.0, end_time = 0.0;
    int i;
    start_time = start_synchronization();
    for( i = 0 ; i < iterations ; i++ ) {
        shmem_quiet();
    }
    end_time = stop_synchronization();
    
    return (end_time - start_time)/iterations;
}

/*---------------------------------------------------------------------------*/

/* shmem_quiet ensures completion of RMA routines, so the time to complete
   a shmem_quiet can depend on the RMA routines issued before. This function
   measures the time taken by shmem_quiet with respect to the size of a shemm_put 
   operation issued just before.
*/

void init_Shmem_Quiet_Put( int count, int iterations ){
    init_synchronization();
    target = (char*) shmalloc( count );
}

void finalize_Shmem_Quiet_Put( int count, int iterations ){
    shfree( target );
    target = NULL;
}

double measure_Shmem_Quiet_Put( int count, int iterations ){
    double start_time, t1 = 1.0, end_time, t2 = 0.0, ttime = 0.0;
    char* sym;
    int i, rank, size;
    rank = shmem_my_pe();
    size = shmem_n_pes();
    
    start_time = start_synchronization();
    for( i = 0 ; i < iterations ; i++ ) {

        shmem_char_put( target, get_send_buffer(), count, (rank+1)%size );
        
        t1 = wtime();
        shmem_quiet();
        t2 = wtime();
        ttime = ( t2 - t1 );
    }
    end_time = stop_synchronization();
    
    return ttime/iterations;
}


/*---------------------------------------------------------------------------*/

#pragma weak end_skampi_extensions

#endif // SKAMPI_OPENSHMEM

