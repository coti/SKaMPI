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
#endif // SKAMPI_MPI
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "version.h"
#ifdef SKAMPI_OPENSHMEM
#include <shmem.h>
#endif
#if SKAMPI_USE_PAPI
#include <papi.h>
#endif

#include "misc.h"
#include "output.h"
#include "debug.h"
#include "main.h"
#include "private_misc.h"

#include "exec.h"
#include "symboltable.h"
#include "synchronize.h"

extern void yyparse(void);
extern void init_lexer(char *infilename);
extern void finalize_lexer(void);

extern int* global_ranks;

bool syntax_check_only;
bool parser_result;
int inside_measurement_block;
int just_entered_measurement_block;
int measurement_num;
SourceBuf source_buf;
bool log_source;

#ifndef SKAMPI_MPI2
#ifdef SKAMPI_OPENSHMEM
long* psync = NULL;
#endif
#endif

void print_version_info(void)
{
  printf(" This is SKaMPI, version %1d.%1d.%1d, revision %s.\n",
         skampi_maj_version, skampi_min_version, skampi_sub_release, svn_revision);
}

void print_help_info(void)
{
  printf("Usage: %s [-h] [-V] [-n] [-s] [-d debugflags] [-i inputfile] [-o outputfile]\n", 
	 get_progname());
  printf("\n");
  printf("Options:\n");
  printf("  -h        print this help information and exit\n");
  printf("  -V        print skampi version number and exit\n");
  printf("  -n        dry-run, syntax check only\n");
  printf("  -s        log measurement source in outputfile\n");
  printf("  -d flags  set debug flags, 0 is no debug information\n");
  printf("  -i file   set inputfile\n");
  printf("  -o file   set outputfile, if none given standard out is used\n");
  printf("\n");
}

int main(int argc, char* argv[]) 
{

  int arg_it;
  int dry_run;

  char *input_filename = NULL;
  int input_filename_len;
  char *output_filename = NULL;
  FILE *output_file;

  time_t t1, t2;

  set_progname(argv[0]);
#ifdef SKAMPI_MPI
  MPI_Init(&argc, &argv);
#endif
#ifdef SKAMPI_OPENSHMEM
  shmem_init();
#endif
#if SKAMPI_USE_PAPI
  PAPI_library_init( PAPI_VER_CURRENT );
#endif
  init_globals();
  init_output();

  if( get_my_global_rank() == get_output_rank() ) {

    debug_flags = 0;
    dry_run = False;
    log_source = False;

    /* pre-processing the command line */
    for( arg_it = 1; arg_it < argc; arg_it++ ) 
      if( argv[arg_it][0] == '-' ) {
	switch( argv[arg_it][1] ) {
	case 'i': input_filename = argv[++arg_it]; break;
	case 'o': output_filename = argv[++arg_it]; break;
	case 'd': debug_flags = atoi(argv[++arg_it]); break;
	case 'n': dry_run = True; break;
	case 's': log_source = True; break;
	case 'V': print_version_info(); mpi_abort(1); break;
	case 'h': print_help_info(); mpi_abort(1); break;
  
	default:
	  error_without_abort("unknown option '%s'.\n\n", argv[arg_it]);
	  print_help_info();
	  mpi_abort(1);
	}
      } else {
	error_without_abort("unknown option '%s'.\n\n", argv[arg_it]);
	print_help_info(); 
	mpi_abort(1);
      }
    
    if( input_filename == NULL  ) {
      error_with_abort(2, "no input file specified, see skampi -h for help\n"); 
      mpi_abort(2);
    }

    input_filename_len = strlen(input_filename);
    
    if( output_filename != NULL ) {
      output_file = fopen(output_filename, "w");
      if( output_file == NULL ) 
	error_with_abort(4, "cannot open output file \"%s\".\n", 
			 output_filename); 
    } else 
      output_file = stdout;
    set_output_file(output_file);

    inside_measurement_block = False;
    syntax_check_only = True;

    init_lexer(input_filename);
    init_symboltable();

    source_buf.srcbuf_maxlen = 8192; /* will be increased dynamically when needed */
    source_buf.srcbuf = skampi_malloc_chars(source_buf.srcbuf_maxlen);
    source_buf.srcbuf_len = 0;
    source_buf.max_measurements = 64; /* this one, too */
    source_buf.measurement_pos = skampi_malloc_ints(source_buf.max_measurements);
    source_buf.num_measurements = 1;
    source_buf.measurement_pos[0] = 0;
    measurement_num = 0;

    yyparse();
    finalize_lexer();
    
    if( parser_result ) 
      debug(DBG_PARSER1, "syntax check successfull\n");
    else
      mpi_abort(4);

    /* clear_symboltable();  @@@ */

  }

#ifdef SKAMPI_MPI2
  MPI_Bcast(&debug_flags, 1, MPI_INT, get_output_rank(), MPI_COMM_WORLD); /* @@@ optimize!!! @@@ */
  MPI_Bcast(&dry_run, 1, MPI_INT, get_output_rank(), MPI_COMM_WORLD);
  MPI_Bcast(&input_filename_len, 1, MPI_INT, get_output_rank(), MPI_COMM_WORLD); 
  if (get_my_global_rank() != get_output_rank()) {
    input_filename = skampi_malloc_chars(input_filename_len + 1);
  }
  MPI_Bcast(input_filename, input_filename_len+1, MPI_CHAR, get_output_rank(), MPI_COMM_WORLD);

#else // SKAMPI_MPI
#ifdef SKAMPI_OPENSHMEM
  // TODO OSHMEM the interface is changing in OpenSHMEM 1.5

  static int32_t gl_debug[6];
  int used_len;
  char* used_filename;
  int size = shmem_n_pes();
  int rank = shmem_my_pe();
  
  if( NULL == psync ) psync = shmem_malloc( SHMEM_COLLECT_SYNC_SIZE );
  if( rank == get_output_rank() ){
      gl_debug[3] =  debug_flags;        /* source in 3, result in 0 */
      gl_debug[4] =  dry_run;            /* source in 4, result in 1 */
      gl_debug[5] =  input_filename_len; /* source in 5, result in 2 */
  }
  shmem_broadcast32( gl_debug, gl_debug+3, 3, get_output_rank(), 0, 0, size, psync );
  shmem_quiet();
  if( rank != get_output_rank() ){
      debug_flags        = gl_debug[0];
      dry_run            = gl_debug[1];
      input_filename_len = gl_debug[2];
  }
  /* round to 4 bytes */
  used_len = input_filename_len + ( 4 - input_filename_len%4 );
  used_filename = shmem_malloc( used_len + 1 );
  if (rank  == get_output_rank()) {
      memcpy( used_filename, input_filename, input_filename_len+1 );
  }
  if (rank != get_output_rank()) {
    input_filename = skampi_malloc_chars( input_filename_len + 1 );
  }

  shmem_broadcast32( used_filename, used_filename, used_len/4, get_output_rank(), 0, 0, size, psync );
  shmem_quiet();

  memcpy( input_filename, used_filename, input_filename_len );
  input_filename[input_filename_len] = '\0';
  
  shmem_free( used_filename );
#endif // SKAMPI_OPENSHMEM
#endif // SKAMPI_MPI

  init_logging();
  init_debugging();
  init_synchronization_module();

  if( ! dry_run ) {

    inside_measurement_block = False;
    syntax_check_only = False;
    init_lexer(input_filename);
    init_symboltable();

    if( get_my_global_rank() == get_output_rank() ) {
      time(&t1);
      print_output("# SKaMPI Version %d.%d.%d rev. %s\n\n", 
		   skampi_maj_version, skampi_min_version, skampi_sub_release, svn_revision);
      print_output("# Started at %s\n\n", ctime(&t1));
    }

    measurement_num = 0;
    yyparse();
    finalize_lexer();

    if( get_my_global_rank() == get_output_rank() ) {
      time(&t2);
      print_output("\n# Finished at %s", ctime(&t2));
      print_output("# Total runtime %0.0f seconds\n\n", difftime(t2, t1));
    }
  }

  free(source_buf.srcbuf);
  free(source_buf.measurement_pos);

  /*  print_symboltable(); */
  /* print_global_time_differences();  @@ */
  finish_logging();
  finalize_ranks();
  
#ifdef SKAMPI_OPENSHMEM
  shmem_finalize();
#endif
#ifdef SKAMPI_MPI
  MPI_Finalize();
#endif
  return 0;
}
