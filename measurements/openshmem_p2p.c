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
char* source;
char* target;

/*---------------------------------------------------------------------------*/
/*                    Point-to-point routines                                */
/*---------------------------------------------------------------------------*/

/* Just a simple remote put. Since the put operation returns as soon as
   the buffer can be resused, there is no guarantee that the remote operation 
   has completed. 
*/

void init_Shmem_Put_Simple( int count, int iterations ) {
  init_synchronization();
}

double measure_Shmem_Put_Simple( int count, int iterations ){
    double start_time, t1 = 1.0, end_time, t2 = 0.0, ttime = 0.0;
    char* sym;
    int i;
    
    if (iterations<0) {
        return -1.0;   /* indicate that this definitely is an error */
    }
    if (iterations==0) {
        return 0.0;    /* avoid division by zero at the end */
    }
    
    sym = shmem_malloc( count );
    start_time = start_synchronization();
    
    shmem_fence();
    for( i = 0 ; i < iterations ; i++ ) {
        if (get_measurement_rank() == 0){
            t1 = wtime();
            shmem_putmem( sym, get_send_buffer(), count, 1 );
            t2 = wtime();
            ttime += (t2 - t1);
            shmem_quiet();
        }
    }
    
    end_time = stop_synchronization();
    shmem_free( sym );
    return ttime/iterations;
}

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
  start_time = start_synchronization();
  
  shmem_fence();
  for (i=0; i<iterations; i++) {
      if (get_measurement_rank() == 0){
          shmem_putmem( sym, get_send_buffer(), count, 1 );
          shmem_quiet();
      }
      if (get_measurement_rank() == 1){
          shmem_putmem( sym, get_recv_buffer(), count, 0 );
      }
      shmem_quiet();
  }

  end_time = stop_synchronization();
  shmem_free( sym );
  return (end_time - start_time)/iterations;
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Put_Fence_Bidirectional( int count, int iterations ) {
  init_synchronization();
}


double measure_Shmem_Put_Fence_Bidirectional( int count, int iterations ){
  double start_time = 1.0, end_time = 0.0;
  char* sym;
  int i, rank, size;
  rank = shmem_my_pe();
  size = shmem_n_pes();

  if (iterations<0) {
    return -1.0;   /* indicate that this definitely is an error */
  }
  if (iterations==0) {
    return 0.0;    /* avoid division by zero at the end */
  }

  sym = shmem_malloc( count );
  start_time = start_synchronization();

  shmem_fence();
  for (i=0; i<iterations; i++) {
      shmem_putmem( sym, get_send_buffer(), count, (rank + 1 ) % size );
      shmem_fence();
  }

  end_time = stop_synchronization();
  shmem_free( sym );
  return (end_time - start_time)/iterations;
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Put_Full( int count, int iterations ) {
  init_synchronization();
}


double measure_Shmem_Put_Full( int count, int iterations ){
  double start_time = 1.0, end_time = 0.0;
  char* sym;
  int i, rank, size;
  rank = shmem_my_pe();
  size = shmem_n_pes();

  if (iterations<0) {
    return -1.0;   /* indicate that this definitely is an error */
  }
  if (iterations==0) {
    return 0.0;    /* avoid division by zero at the end */
  }

  sym = shmem_malloc( count );
  start_time = start_synchronization();

  for (i=0; i<iterations; i++) {
      shmem_putmem( sym, get_send_buffer(), count, (rank + 1 ) % size );
      shmem_quiet();
  }

  end_time = stop_synchronization();
  shmem_free( sym );
  return (end_time - start_time)/iterations;
}

/*---------------------------------------------------------------------------*/

/* This routine measures the time to post a non-blocking operation, 
   and only the time to post it.
*/

void init_Shmem_Put_Nonblocking_Post( int count, int iterations ) {
  init_synchronization();
}

double measure_Shmem_Put_Nonblocking_Post( int count, int iterations ){
    double start_time, t1 = 1.0, end_time, t2 = 0.0, ttime = 0.0;
    char* sym;
    int i, rank, size;
    rank = shmem_my_pe();
    size = shmem_n_pes();

    if (iterations<0) {
        return -1.0;   /* indicate that this definitely is an error */
    }
    if (iterations==0) {
        return 0.0;    /* avoid division by zero at the end */
    }
    
    sym = shmem_malloc( count );
    start_time = start_synchronization();
    
    shmem_fence();

    for (i=0; i<iterations; i++) {
        t1 = wtime();
        shmem_putmem_nbi( sym, get_send_buffer(), count,  (rank + 1 ) % size );
        t2 = wtime();
        ttime += (t2 - t1);
        shmem_quiet();
    }
    
    end_time = stop_synchronization();
    shmem_free( sym );
    return ttime/iterations;
}

/*---------------------------------------------------------------------------*/

/* This routine measures the time to complete a non-blocking operation, 
   including the time to post it and the time to wait for its completion.
*/

void init_Shmem_Put_Nonblocking_Full( int count, int iterations ) {
  init_synchronization();
}

double measure_Shmem_Put_Nonblocking_Full( int count, int iterations ){
    double start_time, t1 = 1.0, end_time, t2 = 0.0, ttime = 0.0;
    char* sym;
    int i, rank, size;
    rank = shmem_my_pe();
    size = shmem_n_pes();

    if (iterations<0) {
        return -1.0;   /* indicate that this definitely is an error */
    }
    if (iterations==0) {
        return 0.0;    /* avoid division by zero at the end */
    }
    
    sym = shmem_malloc( count );
    start_time = start_synchronization();
    
    shmem_fence();
    for (i=0; i<iterations; i++) {
        t1 = wtime();
        shmem_putmem_nbi( sym, get_send_buffer(), count,  (rank + 1 ) % size );
        shmem_quiet();
        t2 = wtime();
        ttime += (t2 - t1);
    }
    
    end_time = stop_synchronization();
    shmem_free( sym );
    return ttime/iterations;
}


/*---------------------------------------------------------------------------*/

/* This routine measures the time to transfer all the data in shmem_quiet,
   ie the transfer time if no overlap is achieved.
*/

void init_Shmem_Put_Nonblocking_Quiet( int count, int iterations ) {
  init_synchronization();
}

double measure_Shmem_Put_Nonblocking_Quiet( int count, int iterations ){
    double start_time, t1 = 1.0, end_time, t2 = 0.0, ttime = 0.0;
    char* sym;
    int i, rank, size;
    rank = shmem_my_pe();
    size = shmem_n_pes();

    if (iterations<0) {
        return -1.0;   /* indicate that this definitely is an error */
    }
    if (iterations==0) {
        return 0.0;    /* avoid division by zero at the end */
    }
    
    sym = shmem_malloc( count );
    start_time = start_synchronization();
    
    shmem_fence();
    for (i=0; i<iterations; i++) {
        shmem_putmem_nbi( sym, get_send_buffer(), count,  (rank + 1 ) % size );
        t1 = wtime();
        shmem_quiet();
        t2 = wtime();
        ttime += (t2 - t1);
    }
    
    end_time = stop_synchronization();
    shmem_free( sym );
    return ttime/iterations;
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

void init_Shmem_Put_Nonblocking_Overlap( int count, int iterations ) {
  init_synchronization();
}

double measure_Shmem_Put_Nonblocking_Overlap( int count, int iterations ){
    double start_time, t1 = 1.0, end_time, t2 = 0.0, btime = 0.0, ttime = 0.0;
    char* sym;
    int i, rank, size;
    rank = shmem_my_pe();
    size = shmem_n_pes();

    if (iterations<0) {
        return -1.0;   /* indicate that this definitely is an error */
    }
    if (iterations==0) {
        return 0.0;    /* avoid division by zero at the end */
    }
    
    sym = shmem_malloc( count );
    start_time = start_synchronization();
    
    /* Measure the time to perform a blocking operation */
    
    shmem_fence();
    for( i = 0 ; i < iterations ; i++ ) {
        t1 = wtime();
        shmem_putmem( sym, get_send_buffer(), count, (rank + 1 ) % size );
        shmem_quiet();
        t2 = wtime();
        btime += (t2 - t1);
    }

    btime /= iterations;
    shmem_fence();

    /* Perform the non-blocking operations */
    for (i=0; i<iterations; i++) {
        shmem_putmem_nbi( sym, get_send_buffer(), count,  (rank + 1 ) % size );
        usleep( 2*btime );

        /* This is what we are measuring */
        t1 = wtime();
        shmem_quiet();
        t2 = wtime();
        ttime += (t2 - t1);
    }
    
    ttime /= iterations;
    
    end_time = stop_synchronization();
    shmem_free( sym );
    return ttime /= iterations;
}

/*---------------------------------------------------------------------------*/

#pragma weak end_skampi_extensions

#endif // SKAMPI_OPENSHMEM

