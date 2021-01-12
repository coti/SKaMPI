#ifdef SKAMPI_OPENSHMEM

#include <mpi.h>
#include <shmem.h>
#include "../mpiversiontest.h"

#include "../misc.h"
#include "../mem.h"
#include "../synchronize.h"

#pragma weak begin_skampi_extensions


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



#pragma weak end_skampi_extensions

#endif // SKAMPI_OPENSHMEM

