/* SKaMPI   MPI-Benchmark

Copyright 2003-2008 Werner Augustin, Lars Diesselberg
Lehrstuhl Informatik fuer Naturwissenschaftler und Ingenieure 
Fakultaet fuer Informatik
University of Karlsruhe

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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

#include "mpiversiontest.h"

#include "misc.h"
#include "private_misc.h"
#include "output.h"
#include "exec.h"
#include "debug.h"

static FILE *output_file;

static int output_rank;
static bool everybody_can_do_io;
static bool i_can_do_io;

static MPI_Comm output_comm;

static int initial_buffer_size;

static char *output_buffer;
static int output_size;
static int output_index;

static char **recv_output_buffers;
static int *recv_sizes;
static int *recv_indexes;
#ifdef SKAMPI_MPI
static MPI_Request *recv_reqs;
#endif

static bool sort_results = False;
static struct variable *loopvar;

struct output_item {
  double key;
  char *output_line;
};

static struct output_item *output_item_buffer;
static int output_item_buffer_size = 100; /* number of results */
static int output_item_index;

void set_output_file(FILE *f)
{
  output_file = f;
}

void init_output(void)
{
  int i, flag;
  int *mpi_io_ptr;

#ifdef SKAMPI_MPI
  MPI_Comm_dup(MPI_COMM_WORLD, &output_comm);
#endif

  /* 3 loop variables, some constant stuff and node times */
  initial_buffer_size = 3*25 + 37 + 10*get_global_size();

#ifdef SKAMPI_MPI
#if MPI_VERSION < 2
  MPI_Attr_get(MPI_COMM_WORLD, MPI_IO, &mpi_io_ptr, &flag);
#else
  MPI_Comm_get_attr(MPI_COMM_WORLD, MPI_IO, &mpi_io_ptr, &flag);
#endif
#else // SKAMPI_MPI
  
  flag = 0;
  output_rank = 0;
  everybody_can_do_io = True;
  i_can_do_io = True;

#endif // SKAMPI_MPI

  
#ifdef SKAMPI_MPI

  if( flag == 0 ) {
    everybody_can_do_io = False;
    i_can_do_io = False;
    error_with_abort(12, "couldn't determine i/o capable processor");
  } else if( *mpi_io_ptr == MPI_PROC_NULL ) {
    everybody_can_do_io = False;
    i_can_do_io = False;
    /* error_with_abort(13, "no processor can do standard conform i/o"); */
    error_without_abort("Warning: the mpi library says that no processor can do standard conform i/o\n"
                        "   You hopefully get this message nevertheless :-/\n"
                        "   We will assume that process 0 in MPI_COMM_WORLD can do enough io for us\n");
    output_rank = 0;
  } else if( *mpi_io_ptr == MPI_ANY_SOURCE ) {
    everybody_can_do_io = True;
    i_can_do_io = True;
    output_rank = 0;
  } else 
    MPI_Allreduce(mpi_io_ptr, &output_rank, 1, MPI_INT, MPI_MIN, 
		  MPI_COMM_WORLD);
#endif // SKAMPI_MPI


  /* @@ optimize for everybody_can_do_io && output_file == stdout? */

  if( get_my_global_rank() == output_rank ) {
    recv_output_buffers = skampi_malloc_charps(get_global_size());
    recv_sizes = skampi_malloc_ints(get_global_size());
    recv_indexes = skampi_malloc_ints(get_global_size());
#ifdef SKAMPI_MPI
    recv_reqs = skampi_malloc_reqs(get_global_size());
#endif // SKAMPI_MPI
    for( i = 0; i < get_global_size(); i++) {
      recv_sizes[i] = initial_buffer_size;
      recv_output_buffers[i] = skampi_malloc_chars(recv_sizes[i]);
    }
    output_item_buffer = (struct output_item*)
      skampi_malloc(sizeof(struct output_item)*output_item_buffer_size);
    assert( output_item_buffer != NULL );
    output_item_index = 0;

  }
   
  output_size = initial_buffer_size;
  output_buffer = skampi_malloc_chars(output_size);
  output_index = 0;
  output_buffer[0] = '\0';
}

int get_output_rank(void)
{
  return output_rank;
}

void print_output(char *fmt, ...)
{
  va_list args;
  int length, space;

  if( get_my_global_rank() == output_rank && !sort_results) {
    va_start(args, fmt);
    vfprintf(output_file, fmt, args);
    va_end(args);

  } else {
    space = output_size - output_index;
    va_start(args, fmt);
    length = vsnprintf(&(output_buffer[output_index]), space, fmt, args);
    while( length == -1 || length+1 > space ) {
      output_size *= 2;
      output_buffer = realloc(output_buffer, output_size);
      assert( output_buffer != NULL );
      space = output_size - output_index;
      length = vsnprintf(&(output_buffer[output_index]), space, fmt, args);
    }
    assert( output_buffer[output_index+length] == '\0'); 
    while( output_buffer[output_index] != '\0' ) output_index++;
    va_end(args);
  } 
}

void flush_output(void)
{
  int i, j, np, received_chars, still_to_complete, got_from;
  int total_size;
  char *output_line;
  double key;
#ifdef SKAMPI_MPI
 MPI_Status status;
#else
#ifdef SKAMPI_OPENSHMEM
  char* shared_output = shmem_malloc( initial_buffer_size );
  static int nb_to_send, read_done;
  int to_recv;
#endif // SKAMPI_OPENSHMEM
#endif // SKAMPI_MPI

  np = get_global_size();

  if( get_my_global_rank() == output_rank ) { /* receive outputs */
#ifdef SKAMPI_MPI
    still_to_complete = np-1;
    for( i = 0; i < np; i++ ) {
      recv_indexes[i] = 0;
      if( i == output_rank )
          recv_reqs[i] = MPI_REQUEST_NULL;
      else {
          MPI_Irecv(recv_output_buffers[i], initial_buffer_size, MPI_CHAR, i, 
                    0, output_comm, &(recv_reqs[i]));
      }
    }

    while( still_to_complete ) {
      MPI_Waitany(np, recv_reqs, &got_from, &status);
      MPI_Get_count(&status, MPI_CHAR, &received_chars);
      recv_indexes[got_from] += received_chars;
      if( received_chars == initial_buffer_size ) {
          if( recv_indexes[got_from] >= recv_sizes[got_from] ) {
              recv_sizes[got_from] += initial_buffer_size;
              recv_output_buffers[got_from] = 
                  realloc(recv_output_buffers[got_from], 
                          recv_sizes[got_from]);
              assert( recv_output_buffers[got_from] != NULL );
          }
          MPI_Irecv(&(recv_output_buffers[got_from][recv_indexes[got_from]]), 
                    initial_buffer_size, MPI_CHAR, got_from, 0, output_comm, 
                    &(recv_reqs[got_from]));
      } else 
          still_to_complete--;
    }
#else // SKAMPI_MPI
#ifdef SKAMPI_OPENSHMEM

    shmem_barrier_all();
    still_to_complete = 0;
    for( got_from = 0; got_from < np; got_from++ ) {
        recv_indexes[got_from] = 0;
        if( got_from != output_rank ){
            to_recv = shmem_int_atomic_fetch( &nb_to_send, got_from );            
            recv_sizes[got_from] = to_recv;

            if( to_recv > 0 ) {
                still_to_complete++;
                shmem_char_get_nbi( recv_output_buffers[got_from], shared_output, to_recv, got_from );
            }
        }
    }

    //   printf( "Still to complete: %d\n", still_to_complete );
    while( still_to_complete ) {
        shmem_quiet(); 

       for( got_from = 0; got_from < np; got_from++ ) {
            
            if( got_from != output_rank
                &&  recv_sizes[got_from] > 0 ){

                /* how much did I receive, is there anything left to receive */
                recv_indexes[got_from] += recv_sizes[got_from];

                if( recv_sizes[got_from] == initial_buffer_size ) {
                    if( recv_indexes[got_from] >= recv_sizes[got_from] ) {
                        recv_sizes[got_from] += initial_buffer_size;
                        recv_output_buffers[got_from] = 
                            realloc( recv_output_buffers[got_from], 
                                     recv_sizes[got_from] );
                        assert( recv_output_buffers[got_from] != NULL );
                    }
                    
                    /* Tell the remote process I am ready to receive what is next */
                    int r_done;
                    r_done = shmem_int_atomic_fetch( &read_done, got_from );
                    
                    /* Receive what is left */
                    //     while( 0 != ( r_done = shmem_int_g( &read_done, got_from ) ) ){
                    while( 0 != ( r_done = shmem_int_atomic_fetch( &read_done, got_from )) ){
                        //while( 0 != r_done ) {
                        usleep( 100 );
                        //                        r_done =  shmem_int_g( &read_done, got_from );
                    }
                    
                    to_recv = shmem_int_g( &nb_to_send,  got_from );
                    recv_sizes[got_from] = to_recv;
                    
                    shmem_char_get_nbi( &(recv_output_buffers[got_from][recv_indexes[got_from]]), shared_output, to_recv, got_from );
                
                    
                } else 
                    still_to_complete--;
            }
        }
    }
    
#endif // SKAMPI_OPENSHMEM
#endif // SKAMPI_MPI

    if( sort_results) {
      debug(DBG_AUTOREFINE, "flush_output(): save result for sorting\n");
      total_size = 0;
      for( i = 0; i < np; i++) total_size += recv_indexes[i];
      total_size += output_index;
      debug(DBG_AUTOREFINE, "flush_output(): total_size=%d\n", total_size);
      debug(DBG_AUTOREFINE, "flush_output(): output_buffer=<<%s>>\n", output_buffer);
      debug(DBG_AUTOREFINE, "flush_output(): output_index=%d\n", output_index);
      output_line = skampi_malloc_chars(total_size+1);

      for( i = 0, j = 0; i < np; i++) 
	if( i != get_my_global_rank() ) {
	  memcpy(&(output_line[j]), recv_output_buffers[i], recv_indexes[i]);
	  debug(DBG_AUTOREFINE, "flush_output(): recv_indexes[%d] = %d\n", i, recv_indexes[i]);
	  j += recv_indexes[i];
	}
      memcpy(&(output_line[j]), output_buffer, output_index);
      output_index = 0; output_buffer[0] = '\0';
      output_line[total_size] = '\0';
      debug(DBG_AUTOREFINE, "flush_output(): output_line=<<%s>>\n", output_line);


      if( loopvar->type == TYPE_INTEGER ) key = (double)loopvar->u.intv;
      else      	                  key = loopvar->u.doublev;
      
      if( output_item_index == output_item_buffer_size ) {
	output_item_buffer = (struct output_item*)
	  realloc(output_item_buffer, 2*output_item_buffer_size);
	assert( output_item_buffer != NULL );
	output_item_buffer_size *= 2;
      }

      output_item_buffer[output_item_index].key = key;
      output_item_buffer[output_item_index].output_line = output_line;
      debug(DBG_AUTOREFINE, "flush_output(): transfer line to output_item_buffer\n");
      debug(DBG_AUTOREFINE, "<<%s>>\n", output_line);
      output_item_index++;

    } else {
      for( i = 0; i < np; i++)
	if( i != output_rank && recv_indexes[i] > 0) 
	  fprintf(output_file, "%s", recv_output_buffers[i]);

      fflush(output_file);
    }

  } else {                                    /* send outputs */
#ifdef SKAMPI_MPI
    j = 0;
    for( i = 0 ; i <= output_index; i += initial_buffer_size ) { /* output_index >= 0 */
      j = imin2(output_index+1-i, initial_buffer_size);
      MPI_Send(&(output_buffer[i]), j, MPI_CHAR, output_rank, 0, output_comm);
    }
    /* receiver waits for another msg if message length was initial_buffer_size */
    if (j == initial_buffer_size) 
      MPI_Send(&(output_buffer[i]), 0, MPI_CHAR, output_rank, 0, output_comm);
    
#else // SKAMPI_MPI
#ifdef SKAMPI_OPENSHMEM

    nb_to_send = imin2(output_index+1, initial_buffer_size);
    
    /* Copy the first chunk */
    memcpy( shared_output, output_buffer, nb_to_send );
    read_done = 0;

    /* The communications can start */
    
    shmem_barrier_all();
    
    if( nb_to_send > 0 ){

        i = initial_buffer_size;
        while(i <= output_index ){
            /* Wait until the other process has read the data */
            shmem_int_wait_until( &read_done, SHMEM_CMP_NE, 0 );
            /* Copy the next chunk */
            nb_to_send = imin2( output_index+1-i, initial_buffer_size );
            memcpy( shared_output, &(output_buffer[i]), nb_to_send );

            i += initial_buffer_size;
            read_done = 0;
        }
    }


    nb_to_send = 0;
    
    /* Done */
    
#endif // SKAMPI_OPENSHMEM
#endif // SKAMPI_MPI

    output_index = 0;
    output_buffer[0] = '\0';
  }
#ifndef SKAMPI_MPI
#ifdef SKAMPI_OPENSHMEM
  shmem_free( shared_output );
#endif // SKAMPI_OPENSHMEM
#endif // SKAMPI_MPI
}

void register_output_for_sorting(struct statement *s)
{
  debug(DBG_AUTOREFINE, "register_output_for_sorting()\n");
  sort_results = True;
  loopvar = s->u.arithm_var.loopvar;
}

static int compare_output_items(const void *item1, const void *item2)
{
  double key1, key2;

  
  key1 = ((struct output_item*)item1)->key;
  key2 = ((struct output_item*)item2)->key;
  debug(DBG_AUTOREFINE, "compare_output_items(key1=%f, key2=%f)\n", key1, key2);

  if(      key1 < key2 ) return -1;
  else if( key1 == key2) return  0;
  else                   return  1;
}

void sort_and_print_output(void)
{
  int i;

  debug(DBG_AUTOREFINE, "sort_and_print_output()\n");
  qsort(output_item_buffer, output_item_index, sizeof(struct output_item), 
	compare_output_items);
  for( i = 0; i < output_item_index; i++) {
    debug(DBG_AUTOREFINE, "printing item with key: %f\n", output_item_buffer[i].key);
    debug(DBG_AUTOREFINE, "<<%s>>\n", output_item_buffer[i].output_line);
    fprintf(output_file, "%s", output_item_buffer[i].output_line);
    free(output_item_buffer[i].output_line);
  }
  sort_results = False;
}


/*---------------------------------------------------------------------------*/

void mpi_abort(int errorcode)
{
  /* we try to send a message to stderr whether we officialy can do I/O 
     (i_can_do_io) or not */ 
  /*  if( i_can_do_io ) {*/
    fprintf(stderr, "\n");
    fflush(stderr);
  /* } */
  MPI_Abort(MPI_COMM_WORLD, errorcode);
}

void error_without_abort(char *fmt, ...)
{
  va_list args;

  /* we try to send a message to stderr whether we officialy can do I/O 
     (i_can_do_io) or not */ 
  /*  if( i_can_do_io ) {*/
    va_start(args, fmt);
    fprintf(stderr, "%s error (proc %d): ", 
	    get_progname(), get_my_global_rank());
    vfprintf(stderr, fmt, args);
    va_end(args);
  /* } */
}

void error_with_abort(int errorcode, char *fmt, ...)
{
  va_list args;

  /* we try to send a message to stderr whether we officialy can do I/O 
     (i_can_do_io) or not */ 
  /*  if( i_can_do_io ) {*/
    va_start(args, fmt);
    fprintf(stderr, "%s error code %d (proc %d): ", 
	    get_progname(), errorcode, get_my_global_rank());
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
    fflush(stderr);
  /* } */
  mpi_abort(errorcode);
}

