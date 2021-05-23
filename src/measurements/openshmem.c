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

double measure_Shmem_Calloc( int iterations, int nb, int size ){
    double start_time = 1.0, end_time = 0.0, ttime = 0.0;
    char* ptr;
    int i;
    
    start_time = start_synchronization();
    
    for( i = 0 ; i < iterations ; i++ ){
        start_time = wtime();
        ptr = (char*)shmem_calloc( nb, size );
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
/*                                     Fence                                 */
/*---------------------------------------------------------------------------*/

/* This function simply measures the time taken by a shmem_fence call */

void init_Shmem_Fence( int iterations ){
    init_synchronization();
}

double measure_Shmem_Fence( int iterations ){
    double start_time = 1.0, end_time = 0.0;
    int i;
    start_time = start_synchronization();
    for( i = 0 ; i < iterations ; i++ ) {
        shmem_fence();
    }
    end_time = stop_synchronization();
    
    return (end_time - start_time)/iterations;
}

/*---------------------------------------------------------------------------*/

/* shmem_fence does not guarantee delivery of the previously posted communications
   but it guarantees they will be delivered in order. Might have a different cost 
   when posted after a communication (such as a put)
*/

void init_Shmem_Fence_Put( int count, int iterations ){
    init_synchronization();
    target = (char*) shmalloc( count );
}

void finalize_Shmem_Fence_Put( int count, int iterations ){
    shfree( target );
    target = NULL;
}

double measure_Shmem_Fence_Put( int count, int iterations ){
    double start_time, t1 = 1.0, end_time, t2 = 0.0, ttime = 0.0;
    char* sym;
    int i, rank, size;
    rank = shmem_my_pe();
    size = shmem_n_pes();
    
    start_time = start_synchronization();
    for( i = 0 ; i < iterations ; i++ ) {

        shmem_char_put( target, get_send_buffer(), count, (rank+1)%size );
        
        t1 = wtime();
        shmem_fence();
        t2 = wtime();
        ttime = ( t2 - t1 );
    }
    end_time = stop_synchronization();
    
    return ttime/iterations;
}

/*---------------------------------------------------------------------------*/
/*                         Point-to-pont synchronization                     */
/*---------------------------------------------------------------------------*/

/* Just wait for something which is already there */

void init_Shmem_Wait_Until( int iterations ){
    target = (char*) shmalloc( sizeof( int ) );
    init_synchronization();
}

void finalize_Shmem_Wait_Until( int iterations ){
    shfree( target );
    target = NULL;
}

double measure_Shmem_Wait_Until( int iterations ){
    double start_time = 1.0, end_time = 0.0;
    int i;
    int* a = (int*) target;
    *a = 1;
    
    start_time = start_synchronization();
    for( i = 0 ; i < iterations ; i++ ) {
        shmem_int_wait_until( a, SHMEM_CMP_EQ, 1 );
    }
    end_time = stop_synchronization();
    
    return (end_time - start_time)/iterations;
}

/*---------------------------------------------------------------------------*/

/* Just test the value */

void init_Shmem_Test( int iterations ){
    target = (char*) shmalloc( sizeof( int ) );
    init_synchronization();
}

void finalize_Shmem_Test( int iterations ){
    shfree( target );
    target = NULL;
}

double measure_Shmem_Test( int iterations ){
    double start_time = 1.0, end_time = 0.0;
    int i;
    int* a = (int*) target;
    *a = 1;
    
    start_time = start_synchronization();
    for( i = 0 ; i < iterations ; i++ ) {
        shmem_int_test( a, SHMEM_CMP_EQ, 1 );
    }
    end_time = stop_synchronization();
    
    return (end_time - start_time)/iterations;
}

/*---------------------------------------------------------------------------*/

#pragma weak end_skampi_extensions

#endif // SKAMPI_OPENSHMEM

