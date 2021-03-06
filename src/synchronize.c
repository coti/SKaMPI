/* SKaMPI   MPI-Benchmark

Copyright 2003-2008 Werner Augustin, Lars Diesselberg
Lehrstuhl Informatik fuer Naturwissenschaftler und Ingenieure 
Fakultaet fuer Informatik
University of Karlsruhe

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

#ifdef SKAMPI_MPI
#include <mpi.h>
#endif
#ifdef SKAMPI_OPENSHMEM
#include <shmem.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <assert.h>

#include "mpiversiontest.h"

#include "misc.h"
#include "private_misc.h"
#include "debug.h"
#include "synchronize.h"
#include "accounting.h"

double *ping_pong_min_time; /* ping_pong_min_time[i] is the minimum time of one ping_pong
			       between the current node and node i, negative value means 
			       time not yet determined */
double *tds;                /* tds[i] is the time difference between the 
			       current node and global node i */

#ifndef SKAMPI_MPI
#ifndef MPI_MAX_PROCESSOR_NAME
#define MPI_MAX_PROCESSOR_NAME 256
#endif
#include <string.h>
#endif

#ifndef SKAMPI_MPI
#ifdef SKAMPI_OPENSHMEM
extern long* psync;
#endif
#endif


enum {
  Number_ping_pongs = 100,
  Minimum_ping_pongs = 8
};

bool mpi_wtime_is_global;

extern int wait_till(double time_stamp, double *last_time_stamp);
extern double should_wait_till(int counter, double interval, double offset);

void init_synchronization_module(void)
{
  int i, flag;
  int *mpi_wtime_is_global_ptr;

  tds = skampi_malloc_doubles(get_global_size());
  for(i = 0; i < get_global_size(); i++) tds[i] = 0.0;
  ping_pong_min_time = skampi_malloc_doubles(get_global_size());
  for( i = 0; i < get_global_size(); i++) ping_pong_min_time[i] = -1.0;

#ifdef SKAMPI_MPI
#if MPI_VERSION < 2
  MPI_Attr_get(MPI_COMM_WORLD, MPI_WTIME_IS_GLOBAL, &mpi_wtime_is_global_ptr, &flag);
#else
  MPI_Comm_get_attr(MPI_COMM_WORLD, MPI_WTIME_IS_GLOBAL, &mpi_wtime_is_global_ptr, &flag);
#endif
#else
  /* no, MPI clocks are not synchronizer because we have no MPI */
  flag = 0;
#endif
  if( flag == 0 ) 
    mpi_wtime_is_global = False;
  else
    mpi_wtime_is_global = *mpi_wtime_is_global_ptr;
  
  rdebug(DBG_TIMEDIFF, "mpi_wtime_is_global = %d\n", mpi_wtime_is_global);
}

void print_global_time_differences(void)
{
  int i, p, name_len, pid;
  char my_name[MPI_MAX_PROCESSOR_NAME];


  double *all_tds = NULL;
  char *names = NULL;
  int *pids = NULL;

  if( grootproc() ) {
    all_tds = skampi_malloc_doubles(get_global_size()*get_global_size());
    names = skampi_malloc_chars(get_global_size()*MPI_MAX_PROCESSOR_NAME);
    pids = skampi_malloc_ints(get_global_size());
  }

  get_processor_name( my_name, &name_len );
  my_name[name_len] = '\0';
  pid = getpid();

#ifdef SKAMPI_MPI
  MPI_Gather(tds, get_global_size(), MPI_DOUBLE, 
	     all_tds, get_global_size(), MPI_DOUBLE, 
	     0, MPI_COMM_WORLD);
  MPI_Gather(my_name, MPI_MAX_PROCESSOR_NAME, MPI_CHAR,
	     names, MPI_MAX_PROCESSOR_NAME, MPI_CHAR, 0, MPI_COMM_WORLD);
  MPI_Gather(&pid, 1, MPI_INT, pids, 1, MPI_INT, 0, MPI_COMM_WORLD);

#else // SKAMPI_MPI
#ifdef SKAMPI_OPENSHMEM

  double* gl_tds;
  double* gl_tds_all;
  
  gl_tds_all = shmem_malloc( get_global_size()*get_global_size() * sizeof( double) );
  gl_tds = shmem_malloc( get_global_size() * sizeof(double ) );

  memcpy( gl_tds, tds, get_global_size() * sizeof(double ) );
  shmem_collect64( gl_tds_all, gl_tds, get_global_size(), 0, 0, get_global_size(), psync );
  memcpy( all_tds, gl_tds_all, get_global_size()*get_global_size() * sizeof(double ) );
  
  shmem_free( gl_tds );
  shmem_free( gl_tds_all );

#endif // SKAMPI_OPENSHMEM
#endif // SKAMPI_MPI

  if( grootproc() ) {
    for( i = 0; i < get_global_size(); i++)
      printf("name[%d] = \"%s\" pid=%d\n", i, &(names[i*MPI_MAX_PROCESSOR_NAME]), pids[i]);

    for( p = 0; p < get_global_size(); p++) {
      printf("tds[%d -> ..] = [%9.6f", p, all_tds[p*get_global_size() + 0]);
      for( i = 1; i < get_global_size(); i++) 
	printf(", %9.6f", all_tds[p*get_global_size() + i]);
      printf("]\n");
    }

    free(all_tds);
    free(names);
    free(pids);
  }
}

/*----------------------------------------------------------------------------*/

static void ping_pong(int p1, int p2)
{
  int i, other_global_id;
  double my_time, my_last_time, other_time;
  double td_min, td_max;
  double invalid_time = -1.0;
#ifdef SKAMPI_MPI
  MPI_Status status;
#else // SKAMPI_MPI
#ifdef SKAMPI_OPENSHMEM

  static double sh_p1_time = 0, sh_p2_time = 0;
  static int64_t* it;

#endif // SKAMPI_OPENSHMEM
#endif // SKAMPI_MPI
  int pp_tag = 43;

#ifdef SKAMPI_MPI

  /* I had to unroll the main loop because I didn't find a portable way
     to define the initial td_min and td_max with INFINITY and NINFINITY */
  if( get_measurement_rank() == p1 ) {
    other_global_id = get_global_rank(p2);
    my_last_time = wtime();
    MPI_Send(&my_last_time, 1, MPI_DOUBLE, p2, pp_tag, get_measurement_comm());
    MPI_Recv(&other_time, 1, MPI_DOUBLE, p2, pp_tag, get_measurement_comm(), &status);
    my_time = wtime();
    td_min = other_time - my_time;
    td_max = other_time - my_last_time;
    MPI_Send(&my_time, 1, MPI_DOUBLE, p2, pp_tag, get_measurement_comm());
  } else {
    other_global_id = get_global_rank(p1);
    MPI_Recv(&other_time, 1, MPI_DOUBLE, p1, pp_tag, get_measurement_comm(), &status);
    my_last_time = wtime();
    td_min = other_time - my_last_time;
    MPI_Send(&my_last_time, 1, MPI_DOUBLE, p1, pp_tag, get_measurement_comm());
    MPI_Recv(&other_time, 1, MPI_DOUBLE, p1, pp_tag, get_measurement_comm(), &status);
    my_time = wtime();
    td_min = fmax2(td_min, other_time - my_time);
    td_max = other_time - my_last_time;
  }

  if( get_measurement_rank() == p1 ) {
    i = 1;
    while( 1 ) {
      MPI_Recv(&other_time, 1, MPI_DOUBLE, p2, pp_tag, get_measurement_comm(), &status);
      if( other_time < 0.0 ) break; 
      my_last_time = my_time; 
      my_time = wtime();
      td_min = fmax2(td_min, other_time - my_time);
      td_max = fmin2(td_max, other_time - my_last_time);
      if( ping_pong_min_time[other_global_id] >= 0.0  && 
	  i >= Minimum_ping_pongs && 
	  my_time - my_last_time < ping_pong_min_time[other_global_id]*1.10 ) {
	MPI_Send(&invalid_time, 1, MPI_DOUBLE, p2, pp_tag, get_measurement_comm());
	break;
      }
      i++;
      if( i == Number_ping_pongs ) {
	MPI_Send(&invalid_time, 1, MPI_DOUBLE, p2, pp_tag, get_measurement_comm());
	break;
      }
      MPI_Send(&my_time, 1, MPI_DOUBLE, p2, pp_tag, get_measurement_comm());

    }
  } else {
    i = 1;
    while( 1 ) {
      MPI_Send(&my_time, 1, MPI_DOUBLE, p1, pp_tag, get_measurement_comm());
      MPI_Recv(&other_time, 1, MPI_DOUBLE, p1, pp_tag, get_measurement_comm(), &status);
      if( other_time < 0.0 ) break; 
      my_last_time = my_time;
      my_time = wtime();
      td_min = fmax2(td_min, other_time - my_time);
      td_max = fmin2(td_max, other_time - my_last_time);
      if( ping_pong_min_time[other_global_id] >= 0.0 && 
	  i >= Minimum_ping_pongs &&
	  my_time - my_last_time < ping_pong_min_time[other_global_id]*1.10 ) {
	MPI_Send(&invalid_time, 1, MPI_DOUBLE, p1, pp_tag, get_measurement_comm());
	break;
      }
      i++;
    }
  }
 #else // SKAMPI_MPI
#ifdef SKAMPI_OPENSHMEM

  //  double invalid_time2 = 0b11111111111111111111111111111111; //0xffffffffffffffff;
  //  int64_t invalid_time2 = 0b11111111111111111111111111111111; //0xffffffffffffffff;

  /* there is no shmem_double_wait_until, so I cheat with an int* */
  
  if( get_measurement_rank() == p1 ) {
      it = (int64_t*)&sh_p2_time;
      
      other_global_id = get_global_rank(p2);
      my_last_time = wtime();
      
      shmem_double_p( &sh_p1_time, my_last_time, p2 );
      shmem_quiet();
      shmem_int64_wait_until( it, SHMEM_CMP_NE, 0 );
      other_time = sh_p2_time;
      sh_p2_time = 0.0;
    
      my_time = wtime();
      td_min = other_time - my_time;
      td_max = other_time - my_last_time;
      
      shmem_double_p( &sh_p1_time, my_time, p2 );
      
  } else {
      other_global_id = get_global_rank(p1);
      it = (int64_t*)&sh_p1_time;
      
      shmem_int64_wait_until( it, SHMEM_CMP_NE, 0 );
      other_time = sh_p1_time;
      sh_p1_time = 0.0;
      
      my_last_time = wtime();
      td_min = other_time - my_last_time;
      
      shmem_double_p( &sh_p2_time, my_last_time, p1 );
      shmem_quiet();
      
      shmem_int64_wait_until( it, SHMEM_CMP_NE, 0 );
      other_time = sh_p1_time;
      sh_p1_time = 0.0;
      
      my_time = wtime();
      td_min = fmax2(td_min, other_time - my_time);
      td_max = other_time - my_last_time;
  }


  if( get_measurement_rank() == p1 ) {
      i = 1;
      while( 1 ) {
          shmem_int64_wait_until( it, SHMEM_CMP_NE, 0 );
          other_time = sh_p2_time;
          sh_p2_time = 0.0;
          
          if( other_time < 0.0 )break;
          
          my_last_time = my_time; 
          my_time = wtime();
          td_min = fmax2(td_min, other_time - my_time);
          td_max = fmin2(td_max, other_time - my_last_time);
          if( ping_pong_min_time[other_global_id] >= 0.0  && 
              i >= Minimum_ping_pongs && 
              my_time - my_last_time < ping_pong_min_time[other_global_id]*1.10 ) {
              shmem_double_p( &sh_p1_time, -1, p2 );
              shmem_quiet();
              break;
          }
          i++;
          if( i == Number_ping_pongs ) {
              shmem_double_p( &sh_p1_time, -1, p2 );
              shmem_quiet();
              break;
          }
          shmem_double_p( &sh_p1_time, my_time, p2 );
          shmem_quiet();
      }
      
  } else {
      i = 1;
      while( 1 ) {
          shmem_double_p( &sh_p2_time, my_time, p1 );
          shmem_quiet();

          shmem_int64_wait_until( it, SHMEM_CMP_NE, 0.0 );
          other_time = sh_p1_time;
          sh_p1_time = 0.0;

          if( other_time < 0 ){
              break;
          }
          my_last_time = my_time;
          my_time = wtime();
          td_min = fmax2(td_min, other_time - my_time);
          td_max = fmin2(td_max, other_time - my_last_time);
          if( ping_pong_min_time[other_global_id] >= 0.0 && 
              i >= Minimum_ping_pongs &&
              my_time - my_last_time < ping_pong_min_time[other_global_id]*1.10 ) {
              shmem_double_p( &sh_p2_time, -1, p1 );
              shmem_quiet();
              break;
          }
          i++;
      }
  }
  

 #endif // SKAMPI_OPENSHMEM
#endif // SKAMPI_MPI

 
  if( ping_pong_min_time[other_global_id] < 0.0) 
    ping_pong_min_time[other_global_id] = td_max-td_min;
  else 
    ping_pong_min_time[other_global_id] = 
      fmin2(ping_pong_min_time[other_global_id], td_max-td_min);

  tds[other_global_id] = (td_min+td_max)/2.0;
}

void determine_time_differences(void)
{
  int i;
  double *tmp_tds;
  double start_time, finish_time;
  double mult;
#ifdef SKAMPI_USE_PAPI
    mult = 1.0;
#else
    mult = 1.0e6;
#endif   

  if( lrootproc() ) 
    start_time = acc_start_sync();
  else
    start_time = wtime();
  logging(DBG_SYNC, "determine_time_differences\n");
  for( i = 1; i < get_measurement_size(); i++) {
#ifdef SKAMPI_MPI
    MPI_Barrier(get_measurement_comm());
#else
#ifdef SKAMPI_OPENSHMEM
    shmem_barrier_all();
#endif // SKAMPI_OPENSHMEM
#endif // SKAMPI_MPI
    if( get_measurement_rank() == 0 || get_measurement_rank() == i ) 
      ping_pong(0,i);
  }
#ifdef SKAMPI_MPI
  tmp_tds = skampi_malloc_doubles(get_measurement_size());
#else 
#ifdef SKAMPI_OPENSHMEM
  tmp_tds = (double*) shmem_malloc( get_measurement_size()*sizeof( double) );
#endif // SKAMPI_OPENSHMEM
#endif // SKAMPI_MPI
  if( lrootproc() ) {
    for( i = 1; i < get_measurement_size(); i++) 
            tmp_tds[i] = tds[get_global_rank(i)];
  }
  
  assert(get_measurement_size() - 1 >= 0 );
#ifdef SKAMPI_MPI
  MPI_Bcast(&(tmp_tds[1]), get_measurement_size()-1, MPI_DOUBLE, 0, get_measurement_comm());
#else
#ifdef SKAMPI_OPENSHMEM
  shmem_broadcast64( &(tmp_tds[1]), &(tmp_tds[1]), get_measurement_size()-1, 0, 0, 0, get_measurement_size(), psync );
#endif // SKAMPI_OPENSHMEM
#endif // SKAMPI_MPI

  if( get_measurement_rank() != 0 ) {
    logging(DBG_TIMEDIFF, "tds[%d] = %9.1f us\n", 
	    get_global_rank(0), tds[get_global_rank(0)]*mult);
    for( i = 1; i < get_measurement_size(); i++) {
      tds[get_global_rank(i)] = tmp_tds[i] + tds[get_global_rank(0)];
      logging(DBG_TIMEDIFF, "tds[%d] = %9.1f us\n", 
	      get_global_rank(i), tds[get_global_rank(i)]*mult);
    }
  } 
#ifdef SKAMPI_MPI
  MPI_Barrier(get_measurement_comm());
  free(tmp_tds);
#else
#ifdef SKAMPI_OPENSHMEM
  shmem_barrier_all(); // Not really necessary because there is one in shmem_free
  shmem_free( tmp_tds ); 
#endif // SKAMPI_OPENSHMEM
#endif // SKAMPI_MPI
  if( lrootproc() )
    finish_time = acc_stop_sync();
  else
    finish_time = wtime();
  rdebug(DBG_TIMEDIFF, "determine_time_differences() on %d processes: %f sec.\n", 
	 get_measurement_size(), finish_time - start_time);
}
