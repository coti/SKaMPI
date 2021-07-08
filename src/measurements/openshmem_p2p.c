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
#include <unistd.h> // for usleep

#include "../misc.h"
#include "../mem.h"
#include "../synchronize.h"
#include "../private_misc.h"

double measure_quiet( int iterations ){
    double t1;
    int i;
    t1 = wtime();
    for( i = 0 ; i < iterations ; i++ ) {
        shmem_quiet();
    }
    return ( wtime() - t1 ) / iterations;
}

#pragma weak begin_skampi_extensions

static char* source;
static char* target;
static char* sym;

/*---------------------------------------------------------------------------*/
/*                    Point-to-point routines                                */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                                  Put                                      */
/*---------------------------------------------------------------------------*/

/* Just a simple remote put. Since the put operation returns as soon as
   the buffer can be resused, there is no guarantee that the remote operation 
   has completed. 
*/

void init_Shmem_Put_Simple( int count, int iterations ) {
    sym = shmem_malloc( count );
    init_synchronization();
}
void finalize_Shmem_Put_Simple( int count, int iterations ) {
    shmem_free( sym ); 
}

double measure_Shmem_Put_Simple( int count, int iterations ){
    double start_time, t1 = 1.0, end_time, t2 = 0.0, ttime = 0.0, qtime = 1.0;
    int i;
    
    if (iterations<0) {
        return -1.0;   /* indicate that this definitely is an error */
    }
    if (iterations==0) {
        return 0.0;    /* avoid division by zero at the end */
    }
    
    start_time = start_synchronization();

    if (get_measurement_rank() == 0){
        qtime = /*0.0; /*/ measure_quiet( iterations );
    }
    shmem_fence();

    if (get_measurement_rank() == 0){
        t1 = wtime();
        for( i = 0 ; i < iterations ; i++ ) {
            shmem_putmem( sym, get_send_buffer(), count, 1 );
            shmem_quiet();
        }
        t2 = wtime();
        ttime += (t2 - t1);
    }

    end_time = stop_synchronization();
    return ( ttime/iterations ) - qtime;
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Pingpong_Put_Put( int count, int iterations ) {
    /*onesided_winsize = get_extent(count, datatype);

  set_send_buffer_usage(onesided_winsize);
  set_reported_message_size(onesided_winsize);

  set_recv_buffer_usage(onesided_winsize);*/

  sym = shmem_malloc( count );
  shmem_fence();
  init_synchronization();
}
void finalize_Shmem_Pingpong_Put_Put( int count, int iterations ) {
    shmem_free( sym );
}

double measure_Shmem_Pingpong_Put_Put( int count, int iterations ){
  double start_time = 1.0, end_time = 0.0, qtime = 0.0;
  int i;

  if (iterations<0) {
    return -1.0;   /* indicate that this definitely is an error */
  }
  if (iterations==0) {
    return 0.0;    /* avoid division by zero at the end */
  }
  
  start_time = start_synchronization();
  
  if (get_measurement_rank() <= 1){
    qtime =  measure_quiet( iterations );
  }

  start_time = wtime();
  
  for (i=0; i<iterations; i++) {
      if (get_measurement_rank() == 0){
          shmem_putmem( sym, get_send_buffer(), count, 1 );
          shmem_quiet();
      }
      if (get_measurement_rank() == 1){
          shmem_putmem( sym, get_recv_buffer(), count, 0 );
          shmem_quiet();
      }
  }

  end_time = stop_synchronization();
  if(get_measurement_rank() == 0 )
      return ((end_time - start_time)/iterations) - qtime;
  else
      return -1.0;
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Put_Round( int count, int iterations ) {
  sym = shmem_malloc( count );
  shmem_fence();
  init_synchronization();
}

void finalize_Shmem_Put_Round( int count, int iterations ) {
    shmem_free( sym );
}

double measure_Shmem_Put_Round( int count, int iterations ){
    double start_time = 1.0, end_time = 0.0, qtime;
    int i, rank, size, dst;
  rank = shmem_my_pe();
  size = shmem_n_pes();

  if (iterations<0) {
    return -1.0;   /* indicate that this definitely is an error */
  }
  if (iterations==0) {
    return 0.0;    /* avoid division by zero at the end */
  }

  dst = (rank + 1 ) % size;
  start_time = start_synchronization();
  qtime =  measure_quiet( iterations );

  start_time = wtime();

  for (i=0; i<iterations; i++) {
      shmem_putmem( sym, get_send_buffer(), count, dst );
      shmem_quiet();
  }

  end_time = stop_synchronization();
  return ((end_time - start_time)/iterations) - qtime;
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Put_Full( int count, int iterations ) {
  sym = shmem_malloc( count );
  shmem_fence();
  init_synchronization();
}
void finalize_Shmem_Put_Full( int count, int iterations ) {
    shmem_free( sym );
}

double measure_Shmem_Put_Full( int count, int iterations ){
  double start_time = 1.0, end_time = 0.0;
  int i, rank, size, dst;
  rank = shmem_my_pe();
  size = shmem_n_pes();

  if (iterations<0) {
    return -1.0;   /* indicate that this definitely is an error */
  }
  if (iterations==0) {
    return 0.0;    /* avoid division by zero at the end */
  }
  
  dst = (rank + 1 ) % size ;
  start_time = start_synchronization();

  for (i=0; i<iterations; i++) {
      shmem_putmem( sym, get_send_buffer(), count, dst );
      shmem_quiet();
  }

  end_time = stop_synchronization();
  return (end_time - start_time)/iterations;
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Put_Full_Single( int count, int iterations, int therank ) {
  sym = shmem_malloc( count );
  shmem_fence();
  init_synchronization();
}

void finalize_Shmem_Put_Full_Single( int count, int iterations, int therank ) {
    shmem_free( sym );
}

double measure_Shmem_Put_Full_Single( int count, int iterations, int therank ){
  double start_time = 1.0, end_time = 0.0;
  int i, rank, size, dst;
  rank = shmem_my_pe();
  size = shmem_n_pes();

  if (iterations<0) {
    return -1.0;   /* indicate that this definitely is an error */
  }
  if (iterations==0) {
    return 0.0;    /* avoid division by zero at the end */
  }

  dst = (rank + 1 ) % size;
  start_time = start_synchronization();

  if( therank == rank ){
      for (i=0; i<iterations; i++) {
          shmem_putmem( sym, get_send_buffer(), count, dst );
          shmem_quiet();
      }
  }

  end_time = stop_synchronization();
  shmem_barrier_all();
  
  if( therank == rank ){
      return (end_time - start_time)/iterations;
  } else {
      return -1.0;
  }  
}

/*---------------------------------------------------------------------------*/

void init_Shmem_P_Simple( int iterations ) {
  sym = shmem_malloc( sizeof( int ) );
  init_synchronization();
}
void finalize_Shmem_P_Simple( int iterations ) {
    shmem_free( sym );
}

double measure_Shmem_P_Simple( int iterations ){
    double btime, ptime = -1.0, t1, t2;
    int i, rank, size, k;
    rank = shmem_my_pe();
    size = shmem_n_pes();

    if (iterations<0) {
        return -1.0;   /* indicate that this definitely is an error */
    }
    if (iterations==0) {
        return 0.0;    /* avoid division by zero at the end */
    }
    
    t1 = start_synchronization();

    if( get_measurement_rank() == 0){

        /* Measure a large number of shmem_quiet operations */

        btime =  measure_quiet( iterations );

        /* Measure the shmem_p itself */
        
        t1 = wtime();
        for( i = 0 ; i < iterations ; i++ ) {
            shmem_int_p( (int*)sym, i, 1 );
            shmem_quiet();
        }
        ptime = ( wtime() - t1 ) / iterations;
    }
    
    t1 = stop_synchronization();
    if( get_measurement_rank() == 0)
        return ptime - btime;
    else
        return -1.0;
}
/*---------------------------------------------------------------------------*/

void init_Shmem_P_Round( int iterations ) {
  sym = shmem_malloc( sizeof( int ) );
  init_synchronization();
}
void finalize_Shmem_P_Round( int iterations ) {
    shmem_free( sym );
}

double measure_Shmem_P_Round( int iterations ){
    double btime, ptime = -1.0, t1, t2;
    int i, rank, size, k, dst;
    rank = shmem_my_pe();
    size = shmem_n_pes();

    if (iterations<0) {
        return -1.0;   /* indicate that this definitely is an error */
    }
    if (iterations==0) {
        return 0.0;    /* avoid division by zero at the end */
    }
    
    dst = (rank + 1 ) % size;
    t1 = start_synchronization();

    /* Measure a large number of shmem_quiet operations */
    
    btime =  measure_quiet( iterations );
    
    /* Measure the shmem_p itself */
    
    t1 = wtime();
    for( i = 0 ; i < iterations ; i++ ) {
        shmem_int_p( (int*)sym, i, dst );
        shmem_quiet();
    }
    ptime = ( wtime() - t1 ) / iterations;
    
    t1 = stop_synchronization();
    return ptime - btime;
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Iput_Round( int count, int stride, int iterations ) {
    sym = shmem_malloc( count*sizeof( char ) * stride );
    shmem_fence();
    init_synchronization();
}
void finalize_Shmem_Iput_Round( int count, int stride, int iterations ) {
    shmem_free( sym );
}

double measure_Shmem_Iput_Round( int count, int stride, int iterations ){
    double start_time = 1.0, end_time = 0.0, qtime;
    int i, rank, size, dst;
    rank = shmem_my_pe();
    size = shmem_n_pes();
    
    if (iterations<0) {
        return -1.0;   /* indicate that this definitely is an error */
    }
    if (iterations==0) {
        return 0.0;    /* avoid division by zero at the end */
    }
    
    dst = (rank + 1 ) % size;
    start_time = start_synchronization();
    
    qtime = measure_quiet( iterations );
    
    start_time = wtime();
    for (i=0; i<iterations; i++) {
        shmem_char_iput( sym, get_send_buffer(), stride, stride, count, dst );
        shmem_quiet();
    }
    
    end_time = stop_synchronization();
    return ((end_time - start_time)/iterations) - qtime;
}

/*---------------------------------------------------------------------------*/

/* This routine measures the time to post a non-blocking operation, 
   and only the time to post it.
*/

void init_Shmem_Put_Nonblocking_Post( int count, int iterations ) {
  sym = shmem_malloc( count );
  init_synchronization();
}
void finalize_Shmem_Put_Nonblocking_Post( int count, int iterations ) {
    shmem_free( sym );
}
double measure_Shmem_Put_Nonblocking_Post( int count, int iterations ){
    double start_time, t1 = 1.0, end_time, t2 = 0.0, ttime = 0.0;
    int i, rank, size, dst;
    rank = shmem_my_pe();
    size = shmem_n_pes();

    if (iterations<0) {
        return -1.0;   /* indicate that this definitely is an error */
    }
    if (iterations==0) {
        return 0.0;    /* avoid division by zero at the end */
    }

    dst = (rank + 1 ) % size;   
    start_time = start_synchronization();
    
    t1 = wtime();
    for (i=0; i<iterations; i++) {
        shmem_putmem_nbi( sym, get_send_buffer(), count,  dst );
    }
    t2 = wtime();
    ttime = t2 - t1;
    shmem_quiet();
    
    end_time = stop_synchronization();
    return ttime/iterations;
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Put_Nonblocking_Post_Single( int count, int iterations, int therank ) {
    sym = shmem_malloc( count );
    init_synchronization();
}
void finalize_Shmem_Put_Nonblocking_Post_Single( int count, int iterations, int therank ) {
    shmem_free( sym );
}
double measure_Shmem_Put_Nonblocking_Post_Single( int count, int iterations, int therank ){
    double start_time, t1 = 1.0, end_time, t2 = 0.0, ttime = 0.0;
    int i, rank, size, dst;
    rank = shmem_my_pe();
    size = shmem_n_pes();

    if (iterations<0) {
        return -1.0;   /* indicate that this definitely is an error */
    }
    if (iterations==0) {
        return 0.0;    /* avoid division by zero at the end */
    }
    
    dst = (rank + 1 ) % size;
    start_time = start_synchronization();
    
    if( rank == therank ){
        t1 = wtime();
        for (i=0; i<iterations; i++) {
            shmem_putmem_nbi( sym, get_send_buffer(), count, dst );
        }
        t2 = wtime();
        ttime = t2 - t1;
    }
    
    shmem_quiet();

    end_time = stop_synchronization();
    shmem_barrier_all();
    
    if( rank == therank ){
        return ttime/iterations;
    } else {
        return -1.0;
    }
}

/*---------------------------------------------------------------------------*/

/* This routine measures the time to complete a non-blocking operation, 
   including the time to post it and the time to wait for its completion.
*/

void init_Shmem_Put_Nonblocking_Full( int count, int iterations ) {
    sym = shmem_malloc( count );
    init_synchronization();
}
void finalize_Shmem_Put_Nonblocking_Full( int count, int iterations ) {
    shmem_free( sym );
}

double measure_Shmem_Put_Nonblocking_Full( int count, int iterations ){
    double start_time, t1 = 1.0, end_time, t2 = 0.0, ttime = 0.0;
    int i, rank, size, dst;
    rank = shmem_my_pe();
    size = shmem_n_pes();

    if (iterations<0) {
        return -1.0;   /* indicate that this definitely is an error */
    }
    if (iterations==0) {
        return 0.0;    /* avoid division by zero at the end */
    }
    dst = (rank + 1 ) % size;
    start_time = start_synchronization();
    
    for (i=0; i<iterations; i++) {
        t1 = wtime();
        shmem_putmem_nbi( sym, get_send_buffer(), count, dst );
        shmem_quiet();
        t2 = wtime();
        ttime += (t2 - t1);
    }
    
    end_time = stop_synchronization();
    return ttime/iterations;
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Put_Nonblocking_Full_Single( int count, int iterations, int therank ) {
    sym = shmem_malloc( count );
    init_synchronization();
}
void finalize_Shmem_Put_Nonblocking_Full_Single( int count, int iterations, int therank ) {
    shmem_free( sym );
}

double measure_Shmem_Put_Nonblocking_Full_Single( int count, int iterations, int therank ){
    double start_time, t1 = 1.0, end_time, t2 = 0.0, ttime = 0.0;
    int i, rank, size, dst;
    rank = shmem_my_pe();
    size = shmem_n_pes();

    if (iterations<0) {
        return -1.0;   /* indicate that this definitely is an error */
    }
    if (iterations==0) {
        return 0.0;    /* avoid division by zero at the end */
    }
    dst = (rank + 1 ) % size;
    start_time = start_synchronization();
    
   if( therank == rank ){
       for (i=0; i<iterations; i++) {
           t1 = wtime();
           shmem_putmem_nbi( sym, get_send_buffer(), count, dst );
           shmem_quiet();
           t2 = wtime();
           ttime += (t2 - t1);
       }
   }
   
   end_time = stop_synchronization();
   shmem_barrier_all();
   
   if( therank == rank ){
       return ttime/iterations;
   } else {
       return -1.0;
   }  
}

/*---------------------------------------------------------------------------*/

/* This routine measures the time to transfer all the data in shmem_quiet,
   ie the transfer time if no overlap is achieved.
*/

void init_Shmem_Put_Nonblocking_Quiet_Single( int count, int iterations, int therank ) {
  sym = shmem_malloc( count );
  init_synchronization();
}
void finalize_Shmem_Put_Nonblocking_Quiet_Single( int count, int iterations, int therank ) {
    shmem_free( sym );
}

double measure_Shmem_Put_Nonblocking_Quiet_Single( int count, int iterations, int therank ){
    double start_time, t1 = 1.0, end_time, t2 = 0.0, ttime = 0.0;
    int i, rank, size, dst;
    rank = shmem_my_pe();
    size = shmem_n_pes();

    if (iterations<0) {
        return -1.0;   /* indicate that this definitely is an error */
    }
    if (iterations==0) {
        return 0.0;    /* avoid division by zero at the end */
    }

    dst = (rank + 1 ) % size;
    start_time = start_synchronization();
    
    if( therank == rank ){
        for (i=0; i<iterations; i++) {
            shmem_putmem_nbi( sym, get_send_buffer(), count, dst );
            t1 = wtime();
            shmem_quiet();
            t2 = wtime();
            ttime += (t2 - t1);
        }
    }
    
    end_time = stop_synchronization();
    shmem_barrier_all();
    
    if( therank == rank ){
        return ttime/iterations;
    } else {
        return -1.0;
    }  
}

/*---------------------------------------------------------------------------*/

/* This routine can be used to measure the percentage of overlap achieved. 
   First, it measures the time taken by blocking operations. Then it posts 
   a non-blocking put, and sleeps during 2*the time to perform the blocking 
   operation. Then the time pent in shmem_quiet is measured. It returns 
   he time spent in shemm_quiet. The overlap can be obtained by the ratio 
   between the time spent in wait (returned here) and the time to perform 
   the blocking operation.
*/

unsigned int overlap_sleep;
double sleeptime;

void init_Shmem_Put_Nonblocking_Overlap( int count, int iterations ) {
    double t1, t2, btime, mytime;
    int i, rank, size, dst;
    rank = shmem_my_pe();
    size = shmem_n_pes();
    
    init_synchronization();

    dst = ( rank + 1 ) % size;
    sym = shmem_malloc( count );
    
    /* Measure the time to perform a non-blocking operation */
    
    t1 = wtime();
    for( i = 0 ; i < iterations ; i++ ) {
        shmem_putmem_nbi( sym, get_send_buffer(), count, dst );
        shmem_quiet();
    }
    t2 = wtime();
    btime = (t2 - t1);
    btime /= iterations;

    //    shmem_fence();
	
    /* We cannot use usleep here */
    mytime = 0.0;
    overlap_sleep = 1e2;
    while( mytime < btime*iterations ){
        int ww = 0;
        overlap_sleep *= 2;
        t1 = wtime();
        for( i = 0 ; i < iterations ; i++ ) {
            for( unsigned int k = 0 ; k < overlap_sleep ; k++ ){
                ww++;
                asm( "" );
                ww--;                
            }
      }
      mytime = wtime() - t1;
    }
    overlap_sleep *= 2;
    sleeptime = mytime * 2 / iterations;
}

void finalize_Shmem_Put_Nonblocking_Overlap( int count, int iterations ){
    shmem_free( sym );
}

double measure_Shmem_Put_Nonblocking_Overlap( int count, int iterations ){
    double start_time, t1 = 1.0, end_time, t2 = 0.0, btime = 0.0, ttime = 0.0;
    int i, rank, size, dst;
    int ww = 0;
    rank = shmem_my_pe();
    size = shmem_n_pes();

    if (iterations<0) {
        return -1.0;   /* indicate that this definitely is an error */
    }
    if (iterations==0) {
        return 0.0;    /* avoid division by zero at the end */
    }
    srand( getpid() );
    dst = (rank + 1 ) % size;
    start_time = start_synchronization();
    
    /* Perform the non-blocking operations */
    for (i=0; i<iterations; i++) {
        t1 = wtime();
        shmem_putmem_nbi( sym, get_send_buffer(), count, dst );
        for( unsigned int k = 0 ; k < overlap_sleep ; k++ ){
            ww++;
            asm( "" ); /* prevents optimization */
            ww--;                
        }
        /* This is what we are measuring */
        shmem_quiet();
        ttime += wtime() - t1;
    }
    shmem_barrier_all();
      
    end_time = stop_synchronization();
    return ttime / iterations - sleeptime;    
}

/*---------------------------------------------------------------------------*/
/* Performed on a single process */

void init_Shmem_Put_Nonblocking_Overlap_Single( int count, int iterations, int rank ){
    init_Shmem_Put_Nonblocking_Overlap(  count,  iterations );
}
void finalize_Shmem_Put_Nonblocking_Overlap_Single( int count, int iterations, int rank ){
    finalize_Shmem_Put_Nonblocking_Overlap(  count,  iterations );
}

double measure_Shmem_Put_Nonblocking_Overlap_Single( int count, int iterations, int therank ){
  double start_time, t1 = 1.0, end_time, t2 = 0.0, btime = 0.0, ttime = 0.0, t3;
    int i, rank, size, dst;
    int ww = 0;
    rank = shmem_my_pe();
    size = shmem_n_pes();

    if (iterations<0) {
        return -1.0;   /* indicate that this definitely is an error */
    }
    if (iterations==0) {
        return 0.0;    /* avoid division by zero at the end */
    }
    srand( getpid() );
    dst = (rank + 1 ) % size;
    start_time = start_synchronization();

    if( rank == therank ){
        /* Perform the non-blocking operations */
        for (i=0; i<iterations; i++) {
            t1 = wtime();
            shmem_putmem_nbi( sym, get_send_buffer(), count, dst );
            for( unsigned int k = 0 ; k < overlap_sleep ; k++ ){
                ww++;
                asm( "" ); /* prevents optimization */
                ww--;                
            }
            /* This is what we are measuring */
            shmem_quiet();
            ttime += wtime() - t1;
        }
    }
    shmem_barrier_all();
      
    end_time = stop_synchronization();
    if( rank == therank ){
        return ttime / iterations - sleeptime;
    } else {
        return -1;
    }
}

/*---------------------------------------------------------------------------*/
/*                                  Get                                      */
/*---------------------------------------------------------------------------*/
/* Just a simple remote put. The get operation returns when the operation 
   has completed. 
*/

void init_Shmem_Get_Simple( int count, int iterations ) {
    sym = shmem_malloc( count );
    init_synchronization();
}
void finalize_Shmem_Get_Simple( int count, int iterations ) {
    shmem_free( sym );
}

double measure_Shmem_Get_Simple( int count, int iterations ){
    double start_time = 1.0, end_time = 0.0;
    int i;
    
    if (iterations<0) {
        return -1.0;   /* indicate that this definitely is an error */
    }
    if (iterations==0) {
        return 0.0;    /* avoid division by zero at the end */
    }

    if (get_measurement_rank() == 0){
        start_time = start_synchronization();
        
        for( i = 0 ; i < iterations ; i++ ) {
            shmem_getmem( get_recv_buffer(), sym, count, 1 );
        }
        
        end_time = stop_synchronization();
    }
    
  if(get_measurement_rank() == 0 )
      return ( end_time - start_time )/iterations;
  else
      return -1.0;
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Get_Round( int count, int iterations ) {
  sym = shmem_malloc( count );
  init_synchronization();
}
void finalize_Shmem_Get_Round( int count, int iterations ) {
    shmem_free( sym );
}

double measure_Shmem_Get_Round( int count, int iterations ){
  double start_time = 1.0, end_time = 0.0;
  int i, rank, size, dst;
  rank = shmem_my_pe();
  size = shmem_n_pes();

  if (iterations<0) {
    return -1.0;   /* indicate that this definitely is an error */
  }
  if (iterations==0) {
    return 0.0;    /* avoid division by zero at the end */
  }

  dst = (rank + 1 ) % size;
  start_time = start_synchronization();

  for (i=0; i<iterations; i++) {
      shmem_getmem( get_recv_buffer(), sym, count, dst );
  }

  end_time = stop_synchronization();
  return (end_time - start_time)/iterations;
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Iget_Round( int count, int stride, int iterations ) {
  sym = shmem_malloc( count*sizeof( char ) * stride );
  init_synchronization();
}
void finalize_Shmem_Iget_Round( int count, int stride, int iterations ) {
    shmem_free( sym );
}

double measure_Shmem_Iget_Round( int count, int stride, int iterations ){
  double start_time = 1.0, end_time = 0.0;
  char* sym;
  int i, rank, size, dst;
  rank = shmem_my_pe();
  size = shmem_n_pes();

  if (iterations<0) {
    return -1.0;   /* indicate that this definitely is an error */
  }
  if (iterations==0) {
    return 0.0;    /* avoid division by zero at the end */
  }

  dst = (rank + 1 ) % size;
  start_time = start_synchronization();

  for (i=0; i<iterations; i++) {
      shmem_char_iget( get_recv_buffer(), sym, stride, stride, count, dst );
  }

  end_time = stop_synchronization();
  return (end_time - start_time)/iterations;
}

/*---------------------------------------------------------------------------*/

/* This routine measures the time to post a non-blocking operation, 
   and only the time to post it.
*/

void init_Shmem_Get_Nonblocking_Post( int count, int iterations ) {
    sym = shmem_malloc( count );
    init_synchronization();
}
void finalize_Shmem_Get_Nonblocking_Post( int count, int iterations ) {
    shmem_free( sym );
}

double measure_Shmem_Get_Nonblocking_Post( int count, int iterations ){
    double start_time, t1 = 1.0, end_time, t2 = 0.0, ttime = 0.0;
    int i, rank, size, dst;
    rank = shmem_my_pe();
    size = shmem_n_pes();

    if (iterations<0) {
        return -1.0;   /* indicate that this definitely is an error */
    }
    if (iterations==0) {
        return 0.0;    /* avoid division by zero at the end */
    }    

   dst = (rank + 1 ) % size;
   start_time = start_synchronization();
   
   t1 = wtime();
   for (i=0; i<iterations; i++) {
       shmem_getmem_nbi( get_send_buffer(), sym, count, dst );
   }
   t2 = wtime();
   ttime = t2 - t1;
   shmem_quiet();
   
   end_time = stop_synchronization();
   return ttime/iterations;
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Get_Nonblocking_Post_Single( int count, int iterations, int therank ) {
    sym = shmem_malloc( count );
    init_synchronization();
}
void finalize_Shmem_Get_Nonblocking_Post_Single( int count, int iterations, int therank ) {
    shmem_free( sym );
}

double measure_Shmem_Get_Nonblocking_Post_Single( int count, int iterations, int therank ){
    double start_time, t1 = 1.0, end_time, t2 = 0.0, ttime = 0.0;
    int i, rank, size, dst;
    rank = shmem_my_pe();
    size = shmem_n_pes();

    if (iterations<0) {
        return -1.0;   /* indicate that this definitely is an error */
    }
    if (iterations==0) {
        return 0.0;    /* avoid division by zero at the end */
    }    

    dst = (rank + 1 ) % size;
    start_time = start_synchronization();
    
    if( therank == rank ){
        t1 = wtime();
        for (i=0; i<iterations; i++) {
            shmem_getmem_nbi( get_send_buffer(), sym, count,  dst );
        }
        t2 = wtime();
        ttime = t2 - t1;
        shmem_quiet();
    }
    
    end_time = stop_synchronization();
    shmem_barrier_all();
    
    if( therank == rank ){
        return ttime/iterations;
    } else {
        return -1.0;
    }  
}

/*---------------------------------------------------------------------------*/

/* This routine measures the time to complete a non-blocking operation, 
   including the time to post it and the time to wait for its completion.
*/

void init_Shmem_Get_Nonblocking_Full( int count, int iterations ) {
  sym = shmem_malloc( count );
  shmem_fence();
  init_synchronization();
}

void finalize_Shmem_Get_Nonblocking_Full( int count, int iterations ) {
    shmem_free( sym );
}

double measure_Shmem_Get_Nonblocking_Full( int count, int iterations ){
    double start_time, t1 = 1.0, end_time, t2 = 0.0, ttime = 0.0;
    int i, rank, size, dst;
    rank = shmem_my_pe();
    size = shmem_n_pes();

    if (iterations<0) {
        return -1.0;   /* indicate that this definitely is an error */
    }
    if (iterations==0) {
        return 0.0;    /* avoid division by zero at the end */
    }
    
    dst = (rank + 1 ) % size;
    start_time = start_synchronization();
    
    for (i=0; i<iterations; i++) {
        t1 = wtime();
        shmem_getmem_nbi( get_recv_buffer(), sym, count, dst );
        shmem_quiet();
        t2 = wtime();
        ttime += (t2 - t1);
    }
    
    end_time = stop_synchronization();
    return ttime/iterations;
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Get_Nonblocking_Full_Single( int count, int iterations, int therank ) {
  sym = shmem_malloc( count );
  shmem_fence();
  init_synchronization();
}

void finalize_Shmem_Get_Nonblocking_Full_Single( int count, int iterations, int therank ) {
    shmem_free( sym );
}

double measure_Shmem_Get_Nonblocking_Full_Single( int count, int iterations, int therank ){
    double start_time, t1 = 1.0, end_time, t2 = 0.0, ttime = 0.0;
    int i, rank, size, dst;
    rank = shmem_my_pe();
    size = shmem_n_pes();

    if (iterations<0) {
        return -1.0;   /* indicate that this definitely is an error */
    }
    if (iterations==0) {
        return 0.0;    /* avoid division by zero at the end */
    }
    
    dst = (rank + 1 ) % size;
    start_time = start_synchronization();
    
    if( therank == rank ){
        for (i=0; i<iterations; i++) {
            t1 = wtime();
            shmem_getmem_nbi( get_recv_buffer(), sym, count, dst );
            shmem_quiet();
            t2 = wtime();
            ttime += (t2 - t1);
        }
    }
    
    end_time = stop_synchronization();
    shmem_barrier_all();
    
    if( therank == rank ){
        return ttime/iterations;
    } else {
        return -1.0;
    }  
}

/*---------------------------------------------------------------------------*/

/* This routine measures the time to transfer all the data in shmem_quiet,
   ie the transfer time if no overlap is achieved.
*/

void init_Shmem_Get_Nonblocking_Quiet( int count, int iterations ) {
  sym = shmem_malloc( count );
  init_synchronization();
}
void finalize_Shmem_Get_Nonblocking_Quiet( int count, int iterations ) {
    shmem_free( sym );
}

double measure_Shmem_Get_Nonblocking_Quiet( int count, int iterations ){
    double start_time, t1 = 1.0, end_time, t2 = 0.0, ttime = 0.0;
    int i, rank, size, dst;
    rank = shmem_my_pe();
    size = shmem_n_pes();

    if (iterations<0) {
        return -1.0;   /* indicate that this definitely is an error */
    }
    if (iterations==0) {
        return 0.0;    /* avoid division by zero at the end */
    }
    
    dst = (rank + 1 ) % size;
    start_time = start_synchronization();
    
    shmem_fence();
    for (i=0; i<iterations; i++) {
        shmem_getmem_nbi( get_recv_buffer(), sym, count, dst );
        t1 = wtime();
        shmem_quiet();
        t2 = wtime();
        ttime += (t2 - t1);
    }
    
    end_time = stop_synchronization();
    return ttime/iterations;
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Get_Nonblocking_Quiet_Single( int count, int iterations, int therank ) {
  sym = shmem_malloc( count );
  init_synchronization();
}
void finalize_Shmem_Get_Nonblocking_Quiet_Single( int count, int iterations, int therank ) {
    shmem_free( sym );
}

double measure_Shmem_Get_Nonblocking_Quiet_Single( int count, int iterations, int therank ){
    double start_time, t1 = 1.0, end_time, t2 = 0.0, ttime = 0.0;
    int i, rank, size, dst;
    rank = shmem_my_pe();
    size = shmem_n_pes();

    if (iterations<0) {
        return -1.0;   /* indicate that this definitely is an error */
    }
    if (iterations==0) {
        return 0.0;    /* avoid division by zero at the end */
    }
    
    dst = (rank + 1 ) % size;
    start_time = start_synchronization();
    
    if( therank == rank ){
        for (i=0; i<iterations; i++) {
            shmem_getmem_nbi( get_recv_buffer(), sym, count, dst );
            t1 = wtime();
            shmem_quiet();
            t2 = wtime();
            ttime += (t2 - t1);
        }
    }
    
    end_time = stop_synchronization();
    shmem_barrier_all();
    
    if( therank == rank ){
        return ttime/iterations;
    } else {
        return -1.0;
    }  
}

/*---------------------------------------------------------------------------*/

/* This routine can be used to measure the percentage of overlap achieved. 
   First, it measures the time taken by blocking operations. Then it posts 
   a non-blocking put, and sleeps during 2*the time to perform the blocking 
   operation. Then the time pent in shmem_quiet is measured. It returns 
   he time spent in shemm_quiet. The overlap can be obtained by the ratio 
   between the time spent in wait (returned here) and the time to perform 
   the blocking operation.
*/

void init_Shmem_Get_Nonblocking_Overlap( int count, int iterations ) {
    init_synchronization();
    double t1, t2, btime, mytime;
    int i, rank, size, dst;
    rank = shmem_my_pe();
    size = shmem_n_pes();
    
    init_synchronization();

    dst = (rank + 1 ) % size;
    sym = shmem_malloc( count );
    
    /* Measure the time to perform a non-blocking operation */
    
    t1 = wtime();
    for( i = 0 ; i < iterations ; i++ ) {
        shmem_getmem_nbi( get_recv_buffer(), sym, count,  dst );
        shmem_quiet();
    }
    t2 = wtime();
    btime = (t2 - t1);
    btime /= iterations;
    //    shmem_fence();
	
    /* We cannot use usleep here */
    mytime = 0.0;
    overlap_sleep = 1e2;
    while( mytime < btime*iterations ){
        int ww = 0;
        overlap_sleep *= 2;
        t1 = wtime();
        for( i = 0 ; i < iterations ; i++ ) {
            for( unsigned int k = 0 ; k < overlap_sleep ; k++ ){
                ww++;
                asm( "" );
                ww--;                
            }
        }
        mytime = wtime() - t1;
    }
    overlap_sleep *= 2;
    sleeptime = mytime * 2 / iterations;
}

void finalize_Shmem_Get_Nonblocking_Overlap( int count, int iterations ){
  shmem_free( sym );
}
double measure_Shmem_Get_Nonblocking_Overlap( int count, int iterations ){
    double start_time, t1 = 1.0, end_time, t2 = 0.0, btime = 0.0, ttime = 0.0;
    int i, rank, size, dst;
    int ww = 0;
    rank = shmem_my_pe();
    size = shmem_n_pes();

    if (iterations<0) {
        return -1.0;   /* indicate that this definitely is an error */
    }
    if (iterations==0) {
        return 0.0;    /* avoid division by zero at the end */
    }
    
    dst = (rank + 1 ) % size;
	srand( getpid() );
    start_time = start_synchronization();
    
    /* Perform the non-blocking operations */
    for (i=0; i<iterations; i++) {
        t1 = wtime();
        shmem_getmem_nbi( get_recv_buffer(), sym, count, dst );
        for( unsigned int k = 0 ; k < overlap_sleep ; k++ ){
            ww++;
            asm( "" ); /* prevents optimization */
            ww--;                
        }
        shmem_quiet();
        t2 = wtime();
        ttime += (t2 - t1);
    }
    
    shmem_barrier_all();
      
    end_time = stop_synchronization();
    return ttime / iterations - sleeptime;    
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Get_Nonblocking_Overlap_Single( int count, int iterations, int therank ) {
    init_Shmem_Get_Nonblocking_Overlap( count, iterations );
}

void finalize_Shmem_Get_Nonblocking_Overlap_Single( int count, int iterations, int therank ){
    finalize_Shmem_Get_Nonblocking_Overlap( count, iterations );
}

double measure_Shmem_Get_Nonblocking_Overlap_Single( int count, int iterations, int therank ){
    double start_time, t1 = 1.0, end_time, t2 = 0.0, btime = 0.0, ttime = 0.0;
    int i, rank, size, dst;
    int ww = 0;
    rank = shmem_my_pe();
    size = shmem_n_pes();

    if (iterations<0) {
        return -1.0;   /* indicate that this definitely is an error */
    }
    if (iterations==0) {
        return 0.0;    /* avoid division by zero at the end */
    }
    
	srand( getpid() );
    dst = (rank + 1 ) % size;
    start_time = start_synchronization();
    
    /* Perform the non-blocking operations */
    if( therank == rank ){
        for (i=0; i<iterations; i++) {
            t1 = wtime();
            shmem_getmem_nbi( get_recv_buffer(), sym, count, dst );
            for( unsigned int k = 0 ; k < overlap_sleep ; k++ ){
                ww++;
                asm( "" ); /* prevents optimization */
                ww--;                
            }
            shmem_quiet();
            t2 = wtime();
            ttime += (t2 - t1);
        }
    }
    
    shmem_barrier_all();
    end_time = stop_synchronization();
    
    if( therank == rank ){
        return ttime/iterations - sleeptime;
    } else {
        return -1.0;
    }  
}

/*---------------------------------------------------------------------------*/

void init_Shmem_G_Round( int iterations ) {
  sym = shmem_malloc( sizeof( int ) );
  init_synchronization();
}
void finalize_Shmem_G_Round( int iterations ) {
    shmem_free( sym );
}

double measure_Shmem_G_Round( int iterations ){
    double start_time = 1.0, end_time = 0.0;
    int i, rank, size, k, dst;
    rank = shmem_my_pe();
    size = shmem_n_pes();

    if (iterations<0) {
        return -1.0;   /* indicate that this definitely is an error */
    }
    if (iterations==0) {
        return 0.0;    /* avoid division by zero at the end */
    }
    
    dst = (rank + 1 ) % size; 
    start_time = start_synchronization();
    
    for( i = 0 ; i < iterations ; i++ ) {
        k = shmem_int_g( (int*)sym, dst );
    }
    
    end_time = stop_synchronization();
    return (end_time - start_time) / iterations;
}

/*---------------------------------------------------------------------------*/

void init_Shmem_G_Simple( int iterations ) {
  sym = shmem_malloc( sizeof( int ) );
  init_synchronization();
}
void finalize_Shmem_G_Simple( int iterations ) {
    shmem_free( sym );
}
double measure_Shmem_G_Simple( int iterations ){
    double start_time = 1.0, end_time = 0.0;
    int i, rank, size, k, dst;
    rank = shmem_my_pe();
    size = shmem_n_pes();

    if (iterations<0) {
        return -1.0;   /* indicate that this definitely is an error */
    }
    if (iterations==0) {
        return 0.0;    /* avoid division by zero at the end */
    }

    dst = (rank + 1 ) % size;
    start_time = start_synchronization();
    if( get_measurement_rank() == 0){        
        for( i = 0 ; i < iterations ; i++ ) {
            k = shmem_int_g( (int*)sym, dst );
        }
    }
    end_time = stop_synchronization();
    if( get_measurement_rank() == 0){
        return (end_time - start_time) / iterations;
    } else{
        return -1.0;
    }
}

/*---------------------------------------------------------------------------*/

#pragma weak end_skampi_extensions

#endif // SKAMPI_OPENSHMEM

