#ifdef SKAMPI_OPENSHMEM

#include <mpi.h>
#include <shmem.h>
#include <stdlib.h>
#include "../mpiversiontest.h"

#include "../misc.h"
#include "../mem.h"
#include "../synchronize.h"
#include "../private_misc.h"

int* ack;
shmem_ctx_t context;

/*---------------------------------------------------------------------------*/
/*        Measurement routines for fetching atomic routines                  */
/*---------------------------------------------------------------------------*/

double erusaem_Shmem_Atomic_XXX( int iterations, int (*routine)( int*, int ) ){
    
    double start_time, t1 = 1.0, end_time, t2 = 0.0, mytime = 0.0;
    int r, mod, i, rank, size;
    size = get_measurement_size();
    rank = get_measurement_rank();
    mod = rank%2;
    
    start_time = start_synchronization();
    
    for( i = 0 ; i < iterations ; i++ ) {
        /* Odd-numbered processes fetch from their -1 neighbor */
        if  ( mod == 1 ){
          start_time = wtime();
          r = routine( ack, rank - 1 );
          end_time = wtime();
          mytime += ( end_time - start_time );
        }
        shmem_barrier_all();
        
        /* Even-numbered processes fetch from their +1 neighbor, if they have one */
        if  ( mod == 0 && rank < size - 1 ){
            start_time = wtime();
            r = routine( ack, rank + 1 );
            end_time = wtime();
            mytime += ( end_time - start_time );
        }
        shmem_barrier_all();
      
        /* Otherwise, the last process fetches from 0 */
        if( size % 2 > 0 ){
            if  ( rank == size - 1 ){
                start_time = wtime();
                r = routine( ack, 0 );
                end_time = wtime();
                mytime += ( end_time - start_time );
            }
        }
    }
    end_time = stop_synchronization();
    
    return mytime / iterations;
}

double erusaem_Shmem_Ctx_Atomic_XXX( int iterations, int (*routine)( shmem_ctx_t, int*, int ) ){
    
    double start_time, t1 = 1.0, end_time, t2 = 0.0, mytime = 0.0;
    int r, mod, i, rank, size;
    size = get_measurement_size();
    rank = get_measurement_rank();
    mod = rank%2;
    
    start_time = start_synchronization();
    
    for( i = 0 ; i < iterations ; i++ ) {
        /* Odd-numbered processes fetch from their -1 neighbor */
        if  ( mod == 1 ){
          start_time = wtime();
          r = routine( context, ack, rank - 1 );
          end_time = wtime();
          mytime += ( end_time - start_time );
        }
        shmem_barrier_all();
        
        /* Even-numbered processes fetch from their +1 neighbor, if they have one */
        if  ( mod == 0 && rank < size - 1 ){
            start_time = wtime();
            r = routine( context, ack, rank + 1 );
            end_time = wtime();
            mytime += ( end_time - start_time );
        }
        shmem_barrier_all();
      
        /* Otherwise, the last process fetches from 0 */
        if( size % 2 > 0 ){
            if  ( rank == size - 1 ){
                start_time = wtime();
                r = routine( context, ack, 0 );
                end_time = wtime();
                mytime += ( end_time - start_time );
            }
        }
    }
    end_time = stop_synchronization();
    
    return mytime / iterations;
}

/*---------------------------------------------------------------------------*/

double erusaem_Shmem_Atomic_XXX_2( int iterations, int (*routine)( int*, int, int ) ){
    
    double start_time, t1 = 1.0, end_time, t2 = 0.0, mytime = 0.0;
    int r, mod, i, rank, size, value;
    size = get_measurement_size();
    rank = get_measurement_rank();
    mod = rank%2;
    value = 4242;
    
    start_time = start_synchronization();
    
    for( i = 0 ; i < iterations ; i++ ) {
        /* Odd-numbered processes fetch from their -1 neighbor */
        if  ( mod == 1 ){
          start_time = wtime();
          r = routine( ack, value, rank - 1 );
          end_time = wtime();
          mytime += ( end_time - start_time );
        }
        shmem_barrier_all();
        
        /* Even-numbered processes fetch from their +1 neighbor, if they have one */
        if  ( mod == 0 && rank < size - 1 ){
            start_time = wtime();
            r = routine( ack, value, rank + 1 );
            end_time = wtime();
            mytime += ( end_time - start_time );
        }
        shmem_barrier_all();
      
        /* Otherwise, the last process fetches from 0 */
        if( size % 2 > 0 ){
            if  ( rank == size - 1 ){
                start_time = wtime();
                r = routine( ack, value, 0 );
                end_time = wtime();
                mytime += ( end_time - start_time );
            }
        }
    }
    end_time = stop_synchronization();
    
    return mytime / iterations;
}

double erusaem_Shmem_Ctx_Atomic_XXX_2( int iterations, int (*routine)( shmem_ctx_t, int*, int, int ) ){
    
    double start_time, t1 = 1.0, end_time, t2 = 0.0, mytime = 0.0;
    int r, mod, i, rank, size, value;
    size = get_measurement_size();
    rank = get_measurement_rank();
    mod = rank % 2;
    value = 4242;
    
    start_time = start_synchronization();
    
    for( i = 0 ; i < iterations ; i++ ) {
        /* Odd-numbered processes fetch from their -1 neighbor */
        if  ( mod == 1 ){
          start_time = wtime();
          r = routine( context, ack, value, rank - 1 );
          end_time = wtime();
          mytime += ( end_time - start_time );
        }
        shmem_barrier_all();
        
        /* Even-numbered processes fetch from their +1 neighbor, if they have one */
        if  ( mod == 0 && rank < size - 1 ){
            start_time = wtime();
            r = routine( context, ack, value, rank + 1 );
            end_time = wtime();
            mytime += ( end_time - start_time );
        }
        shmem_barrier_all();
      
        /* Otherwise, the last process fetches from 0 */
        if( size % 2 > 0 ){
            if  ( rank == size - 1 ){
                start_time = wtime();
                r = routine( context, ack, value, 0 );
                end_time = wtime();
                mytime += ( end_time - start_time );
            }
        }
    }
    end_time = stop_synchronization();
    
    return mytime / iterations;
}

/*---------------------------------------------------------------------------*/
/* This one is used for compare-and-swap routines 
 */

double erusaem_Shmem_Atomic_XXX_3( int iterations, bool cond, int (*routine)( int*, int, int, int ) ){
    
    double start_time, t1 = 1.0, end_time, t2 = 0.0, mytime = 0.0;
    int r, mod, i, rank, size, value, cmp;
    size = get_measurement_size();
    rank = get_measurement_rank();
    mod = rank%2;
    value = 4242;
    if( cond == True ){
        if( mod == 1 ) cmp = rank - 1;
        if( mod == 0 && rank < size - 1 ) cmp = rank + 1;
        if( size % 2 > 0 && rank == size - 1 ) cmp = 0;
    } else {
        cmp = size;
    }

    start_time = start_synchronization();
    
    for( i = 0 ; i < iterations ; i++ ) {
        /* Odd-numbered processes fetch from their -1 neighbor */
        if  ( mod == 1 ){
          start_time = wtime();
          r = routine( ack, cmp, value, rank - 1 );
          end_time = wtime();
          mytime += ( end_time - start_time );
        }
        shmem_barrier_all();
        
        /* Even-numbered processes fetch from their +1 neighbor, if they have one */
        if  ( mod == 0 && rank < size - 1 ){
            start_time = wtime();
            r = routine( ack, cmp, value, rank + 1 );
            end_time = wtime();
            mytime += ( end_time - start_time );
        }
        shmem_barrier_all();
      
        /* Otherwise, the last process fetches from 0 */
        if( size % 2 > 0 ){
            if  ( rank == size - 1 ){
                start_time = wtime();
                r = routine( ack, cmp, value, 0 );
                end_time = wtime();
                mytime += ( end_time - start_time );
            }
        }
    }
    end_time = stop_synchronization();
    
    return mytime / iterations;
}

double erusaem_Shmem_Ctx_Atomic_XXX_3( int iterations, bool cond, int (*routine)( shmem_ctx_t, int*, int, int, int ) ){
    
    double start_time, t1 = 1.0, end_time, t2 = 0.0, mytime = 0.0;
    int r, mod, i, rank, size, value, cmp;
    size = get_measurement_size();
    rank = get_measurement_rank();
    mod = rank % 2;
    value = 4242;
    if( cond == True ){
        if( mod == 1 ) cmp = rank - 1;
        if( mod == 0 && rank < size - 1 ) cmp = rank + 1;
        if( size % 2 > 0 && rank == size - 1 ) cmp = 0;
    } else {
        cmp = size;
    }
    
    start_time = start_synchronization();
    
    for( i = 0 ; i < iterations ; i++ ) {
        /* Odd-numbered processes fetch from their -1 neighbor */
        if  ( mod == 1 ){
          start_time = wtime();
          r = routine( context, ack, cmp, value, rank - 1 );
          end_time = wtime();
          mytime += ( end_time - start_time );
        }
        shmem_barrier_all();
        
        /* Even-numbered processes fetch from their +1 neighbor, if they have one */
        if  ( mod == 0 && rank < size - 1 ){
            start_time = wtime();
            r = routine( context, ack, cmp, value, rank + 1 );
            end_time = wtime();
            mytime += ( end_time - start_time );
        }
        shmem_barrier_all();
      
        /* Otherwise, the last process fetches from 0 */
        if( size % 2 > 0 ){
            if  ( rank == size - 1 ){
                start_time = wtime();
                r = routine( context, ack, cmp, value, 0 );
                end_time = wtime();
                mytime += ( end_time - start_time );
            }
        }
    }
    end_time = stop_synchronization();
    
    return mytime / iterations;
}

/*---------------------------------------------------------------------------*/
/*      Measurement routines for non-fetching atomic routines                */
/*---------------------------------------------------------------------------*/

double erusaem_Shmem_Atomic_NF( int iterations, int (*routine)( int*, int ) ){
    
    double start_time, t1 = 1.0, end_time, t2 = 0.0, mytime = 0.0, qtime = 0.0;
    int r, mod, i, rank, size;
    size = get_measurement_size();
    rank = get_measurement_rank();
    mod = rank%2;
    
    start_time = start_synchronization();

    /* Measure an empty shmem_quiet */
    
    t1 = wtime();
    for( i = 0 ; i < iterations ; i++ ) {
        shmem_quiet();
    }
    qtime = wtime() - t1;

    /* pPerform the operation and ensure completion with shmem_quiet */
    
    for( i = 0 ; i < iterations ; i++ ) {
        /* Odd-numbered processes fetch from their -1 neighbor */
        if  ( mod == 1 ){
          start_time = wtime();
          r = routine( ack, rank - 1 );
          shmem_quiet();
          end_time = wtime();
          mytime += ( end_time - start_time );
        }
        shmem_barrier_all();
        
        /* Even-numbered processes fetch from their +1 neighbor, if they have one */
        if  ( mod == 0 && rank < size - 1 ){
            start_time = wtime();
            r = routine( ack, rank + 1 );
            shmem_quiet();
            end_time = wtime();
            mytime += ( end_time - start_time );
        }
        shmem_barrier_all();
      
        /* Otherwise, the last process fetches from 0 */
        if( size % 2 > 0 ){
            if  ( rank == size - 1 ){
                start_time = wtime();
                r = routine( ack, 0 );
                shmem_quiet();
                end_time = wtime();
                mytime += ( end_time - start_time );
            }
        }
    }
    end_time = stop_synchronization();
    
    return ( mytime - qtime ) / iterations;
}

double erusaem_Shmem_Ctx_Atomic_NF( int iterations, int (*routine)( shmem_ctx_t, int*, int ) ){
    
    double start_time, t1 = 1.0, end_time, t2 = 0.0, mytime = 0.0, qtime = 0.0;
    int r, mod, i, rank, size;
    size = get_measurement_size();
    rank = get_measurement_rank();
    mod = rank%2;
    
    start_time = start_synchronization();

    /* Measure an empty shmem_quiet */
    
    t1 = wtime();
    for( i = 0 ; i < iterations ; i++ ) {
        shmem_quiet();
    }
    qtime = wtime() - t1;

    /* pPerform the operation and ensure completion with shmem_quiet */
    
    for( i = 0 ; i < iterations ; i++ ) {
        /* Odd-numbered processes fetch from their -1 neighbor */
        if  ( mod == 1 ){
          start_time = wtime();
          r = routine( context, ack, rank - 1 );
          shmem_quiet();
          end_time = wtime();
          mytime += ( end_time - start_time );
        }
        shmem_barrier_all();
        
        /* Even-numbered processes fetch from their +1 neighbor, if they have one */
        if  ( mod == 0 && rank < size - 1 ){
            start_time = wtime();
            r = routine( context, ack, rank + 1 );
            shmem_quiet();
            end_time = wtime();
            mytime += ( end_time - start_time );
        }
        shmem_barrier_all();
      
        /* Otherwise, the last process fetches from 0 */
        if( size % 2 > 0 ){
            if  ( rank == size - 1 ){
                start_time = wtime();
                r = routine( context, ack, 0 );
                shmem_quiet();
                end_time = wtime();
                mytime += ( end_time - start_time );
            }
        }
    }
    end_time = stop_synchronization();
    
    return ( mytime - qtime ) / iterations;
}

/*---------------------------------------------------------------------------*/

double erusaem_Shmem_Atomic_NF_2( int iterations, int (*routine)( int*, int, int ) ){
    
    double start_time, t1 = 1.0, end_time, t2 = 0.0, mytime = 0.0, qtime = 0.0;
    int r, mod, i, rank, size;
    size = get_measurement_size();
    rank = get_measurement_rank();
    mod = rank % 2;
    int value = 4242;
    
    start_time = start_synchronization();

    /* Measure an empty shmem_quiet */
    
    t1 = wtime();
    for( i = 0 ; i < iterations ; i++ ) {
        shmem_quiet();
    }
    qtime = wtime() - t1;

    /* pPerform the operation and ensure completion with shmem_quiet */
    
    for( i = 0 ; i < iterations ; i++ ) {
        /* Odd-numbered processes fetch from their -1 neighbor */
        if  ( mod == 1 ){
          start_time = wtime();
          r = routine( ack, value, rank - 1 );
          shmem_quiet();
          end_time = wtime();
          mytime += ( end_time - start_time );
        }
        shmem_barrier_all();
        
        /* Even-numbered processes fetch from their +1 neighbor, if they have one */
        if  ( mod == 0 && rank < size - 1 ){
            start_time = wtime();
            r = routine( ack, value, rank + 1 );
            shmem_quiet();
            end_time = wtime();
            mytime += ( end_time - start_time );
        }
        shmem_barrier_all();
      
        /* Otherwise, the last process fetches from 0 */
        if( size % 2 > 0 ){
            if  ( rank == size - 1 ){
                start_time = wtime();
                r = routine( ack, value, 0 );
                shmem_quiet();
                end_time = wtime();
                mytime += ( end_time - start_time );
            }
        }
    }
    end_time = stop_synchronization();
    
    return ( mytime - qtime ) / iterations;
}

double erusaem_Shmem_Ctx_Atomic_NF_2( int iterations, int (*routine)( shmem_ctx_t, int*, int, int ) ){
    
    double start_time, t1 = 1.0, end_time, t2 = 0.0, mytime = 0.0, qtime = 0.0;
    int r, mod, i, rank, size;
    size = get_measurement_size();
    rank = get_measurement_rank();
    mod = rank % 2;
    int value = 4242;
    
    start_time = start_synchronization();

    /* Measure an empty shmem_quiet */
    
    t1 = wtime();
    for( i = 0 ; i < iterations ; i++ ) {
        shmem_quiet();
    }
    qtime = wtime() - t1;

    /* pPerform the operation and ensure completion with shmem_quiet */
    
    for( i = 0 ; i < iterations ; i++ ) {
        /* Odd-numbered processes fetch from their -1 neighbor */
        if  ( mod == 1 ){
          start_time = wtime();
          r = routine( context, ack, value, rank - 1 );
          shmem_quiet();
          end_time = wtime();
          mytime += ( end_time - start_time );
        }
        shmem_barrier_all();
        
        /* Even-numbered processes fetch from their +1 neighbor, if they have one */
        if  ( mod == 0 && rank < size - 1 ){
            start_time = wtime();
            r = routine( context, ack, value, rank + 1 );
            shmem_quiet();
            end_time = wtime();
            mytime += ( end_time - start_time );
        }
        shmem_barrier_all();
      
        /* Otherwise, the last process fetches from 0 */
        if( size % 2 > 0 ){
            if  ( rank == size - 1 ){
                start_time = wtime();
                r = routine( context, ack, value, 0 );
                shmem_quiet();
                end_time = wtime();
                mytime += ( end_time - start_time );
            }
        }
    }
    end_time = stop_synchronization();
    
    return ( mytime - qtime ) / iterations;
}

/*---------------------------------------------------------------------------*/

#pragma weak begin_skampi_extensions

/* The initialization and finalization routines are always the same */

void tini_Shmem_Atomic( ){
  ack = shmem_malloc( sizeof( int ));
  *ack = get_measurement_rank();
  init_synchronization();
}

void tini_Shmem_Ctx_Atomic( ){
  ack = shmem_malloc( sizeof( int ));
  shmem_ctx_create( SHMEM_CTX_SERIALIZED, &context );
  init_synchronization();
}

void ezilanif_Shmem_Atomic(){
  shmem_free( ack );
}

void ezilanif_Shmem_Ctx_Atomic(){
  shmem_free( ack );
  shmem_ctx_destroy( context );
}

/*---------------------------------------------------------------------------*/
/*                      Fetching atomic routines                             */
/*---------------------------------------------------------------------------*/

void init_Shmem_Atomic_Fetch( int iterations ){
    tini_Shmem_Atomic();
}

void finalize_Shmem_Atomic_Fetch( int iterations ){
    ezilanif_Shmem_Atomic();
}

double measure_Shmem_Atomic_Fetch( int iterations ){
    return erusaem_Shmem_Atomic_XXX( iterations, &shmem_int_atomic_fetch );
}

void init_Shmem_Ctx_Atomic_Fetch( int iterations ){
    tini_Shmem_Ctx_Atomic( );
}

void finalize_Shmem_Ctx_Atomic_Fetch( int iterations ){
    ezilanif_Shmem_Ctx_Atomic();
}

double measure_Shmem_Ctx_Atomic_Fetch( int iterations ){
    return erusaem_Shmem_Ctx_Atomic_XXX( iterations, &shmem_ctx_int_atomic_fetch );
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Atomic_Fetch_Inc( int iterations ){
    tini_Shmem_Atomic( );
}

void finalize_Shmem_Atomic_Fetch_Inc( int iterations ){
    ezilanif_Shmem_Atomic();
}

double measure_Shmem_Atomic_Fetch_Inc( int iterations ){
    return erusaem_Shmem_Atomic_XXX( iterations, &shmem_int_atomic_fetch_inc );
}

void init_Shmem_Ctx_Atomic_Fetch_Inc( int iterations ){
    tini_Shmem_Ctx_Atomic();
}

void finalize_Shmem_Ctx_Atomic_Fetch_Inc( int iterations ){
    ezilanif_Shmem_Ctx_Atomic();
}

double measure_Shmem_Ctx_Atomic_Fetch_Inc( int iterations ){
    return erusaem_Shmem_Ctx_Atomic_XXX( iterations, &shmem_ctx_int_atomic_fetch_inc );
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Atomic_Swap( int iterations ){
    tini_Shmem_Atomic( );
}

void finalize_Shmem_Atomic_Swap( int iterations ){
    ezilanif_Shmem_Atomic();
}

double measure_Shmem_Atomic_Swap( int iterations ){
    return erusaem_Shmem_Atomic_XXX_2( iterations, &shmem_int_atomic_swap );
}

void init_Shmem_Ctx_Atomic_Swap( int iterations ){
    tini_Shmem_Ctx_Atomic();
}

void finalize_Shmem_Ctx_Atomic_Swap( int iterations ){
    ezilanif_Shmem_Ctx_Atomic();
}

double measure_Shmem_Ctx_Atomic_Swap( int iterations ){
    return erusaem_Shmem_Ctx_Atomic_XXX_2( iterations, &shmem_ctx_int_atomic_swap );
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Atomic_Fetch_Add( int iterations ){
    tini_Shmem_Atomic( );
}

void finalize_Shmem_Atomic_Fetch_Add( int iterations ){
    ezilanif_Shmem_Atomic();
}

double measure_Shmem_Atomic_Fetch_Add( int iterations ){
    return erusaem_Shmem_Atomic_XXX_2( iterations, &shmem_int_atomic_fetch_add );
}

void init_Shmem_Ctx_Atomic_Fetch_Add( int iterations ){
    tini_Shmem_Ctx_Atomic();
}

void finalize_Shmem_Ctx_Atomic_Fetch_Add( int iterations ){
    ezilanif_Shmem_Ctx_Atomic();
}

double measure_Shmem_Ctx_Atomic_Fetch_Add( int iterations ){
    return erusaem_Shmem_Ctx_Atomic_XXX_2( iterations, &shmem_ctx_int_atomic_fetch_add );
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Atomic_Fetch_And( int iterations ){
    tini_Shmem_Atomic( );
}

void finalize_Shmem_Atomic_Fetch_And( int iterations ){
    ezilanif_Shmem_Atomic();
}

double measure_Shmem_Atomic_Fetch_And( int iterations ){
    return erusaem_Shmem_Atomic_XXX_2( iterations, &shmem_int_atomic_fetch_and );
}

void init_Shmem_Ctx_Atomic_Fetch_And( int iterations ){
    tini_Shmem_Ctx_Atomic();
}

void finalize_Shmem_Ctx_Atomic_Fetch_And( int iterations ){
    ezilanif_Shmem_Ctx_Atomic();
}

double measure_Shmem_Ctx_Atomic_Fetch_And( int iterations ){
    return erusaem_Shmem_Ctx_Atomic_XXX_2( iterations, &shmem_ctx_int_atomic_fetch_and );
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Atomic_Fetch_Or( int iterations ){
    tini_Shmem_Atomic( );
}

void finalize_Shmem_Atomic_Fetch_Or( int iterations ){
    ezilanif_Shmem_Atomic();
}

double measure_Shmem_Atomic_Fetch_Or( int iterations ){
    return erusaem_Shmem_Atomic_XXX_2( iterations, &shmem_int_atomic_fetch_or );
}

void init_Shmem_Ctx_Atomic_Fetch_Or( int iterations ){
    tini_Shmem_Ctx_Atomic();
}

void finalize_Shmem_Ctx_Atomic_Fetch_Or( int iterations ){
    ezilanif_Shmem_Ctx_Atomic();
}

double measure_Shmem_Ctx_Atomic_Fetch_Or( int iterations ){
    return erusaem_Shmem_Ctx_Atomic_XXX_2( iterations, &shmem_ctx_int_atomic_fetch_or );
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Atomic_Fetch_Xor( int iterations ){
    tini_Shmem_Atomic( );
}

void finalize_Shmem_Atomic_Fetch_Xor( int iterations ){
    ezilanif_Shmem_Atomic();
}

double measure_Shmem_Atomic_Fetch_Xor( int iterations ){
    return erusaem_Shmem_Atomic_XXX_2( iterations, &shmem_int_atomic_fetch_xor );
}

void init_Shmem_Ctx_Atomic_Fetch_Xor( int iterations ){
    tini_Shmem_Ctx_Atomic();
}

void finalize_Shmem_Ctx_Atomic_Fetch_Xor( int iterations ){
    ezilanif_Shmem_Ctx_Atomic();
}

double measure_Shmem_Ctx_Atomic_Fetch_Xor( int iterations ){
    return erusaem_Shmem_Ctx_Atomic_XXX_2( iterations, &shmem_ctx_int_atomic_fetch_xor );
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Atomic_Compare_Swap_Yes( int iterations ){
    tini_Shmem_Atomic( );
}

void finalize_Shmem_Atomic_Compare_Swap_Yes( int iterations ){
    ezilanif_Shmem_Atomic();
}

double measure_Shmem_Atomic_Compare_Swap_Yes( int iterations ){
    return erusaem_Shmem_Atomic_XXX_3( iterations, True, &shmem_int_atomic_compare_swap );
}

void init_Shmem_Ctx_Atomic_Compare_Swap_Yes( int iterations ){
    tini_Shmem_Ctx_Atomic();
}

void finalize_Shmem_Ctx_Atomic_Compare_Swap_Yes( int iterations ){
    ezilanif_Shmem_Ctx_Atomic();
}

double measure_Shmem_Ctx_Atomic_Compare_Swap_Yes( int iterations ){
    return erusaem_Shmem_Ctx_Atomic_XXX_3( iterations, True, &shmem_ctx_int_atomic_compare_swap );
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Atomic_Compare_Swap_No( int iterations ){
    tini_Shmem_Atomic( );
}

void finalize_Shmem_Atomic_Compare_Swap_No( int iterations ){
    ezilanif_Shmem_Atomic();
}

double measure_Shmem_Atomic_Compare_Swap_No( int iterations ){
    return erusaem_Shmem_Atomic_XXX_3( iterations, False, &shmem_int_atomic_compare_swap );
}

void init_Shmem_Ctx_Atomic_Compare_Swap_No( int iterations ){
    tini_Shmem_Ctx_Atomic();
}

void finalize_Shmem_Ctx_Atomic_Compare_Swap_No( int iterations ){
    ezilanif_Shmem_Ctx_Atomic();
}

double measure_Shmem_Ctx_Atomic_Compare_Swap_No( int iterations ){
    return erusaem_Shmem_Ctx_Atomic_XXX_3( iterations, False, &shmem_ctx_int_atomic_compare_swap );
}

/*---------------------------------------------------------------------------*/
/*                    Non-fetching atomic routines                           */
/*---------------------------------------------------------------------------*/

void init_Shmem_Atomic_Inc( int iterations ){
    tini_Shmem_Atomic();
}

void finalize_Shmem_Atomic_Inc( int iterations ){
    ezilanif_Shmem_Atomic();
}

double measure_Shmem_Atomic_Inc( int iterations ){
    return erusaem_Shmem_Atomic_NF( iterations, &shmem_int_atomic_inc );
}

void init_Shmem_Ctx_Atomic_Inc( int iterations ){
    tini_Shmem_Ctx_Atomic( );
}

void finalize_Shmem_Ctx_Atomic_Inc( int iterations ){
    ezilanif_Shmem_Ctx_Atomic();
}

double measure_Shmem_Ctx_Atomic_Inc( int iterations ){
    return erusaem_Shmem_Ctx_Atomic_NF( iterations, &shmem_ctx_int_atomic_inc );
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Atomic_Set( int iterations ){
    tini_Shmem_Atomic();
}

void finalize_Shmem_Atomic_Set( int iterations ){
    ezilanif_Shmem_Atomic();
}

double measure_Shmem_Atomic_Set( int iterations ){
    return erusaem_Shmem_Atomic_NF_2( iterations, &shmem_int_atomic_set );
}

void init_Shmem_Ctx_Atomic_Set( int iterations ){
    tini_Shmem_Ctx_Atomic( );
}

void finalize_Shmem_Ctx_Atomic_Set( int iterations ){
    ezilanif_Shmem_Ctx_Atomic();
}

double measure_Shmem_Ctx_Atomic_Set( int iterations ){
    return erusaem_Shmem_Ctx_Atomic_NF_2( iterations, &shmem_ctx_int_atomic_set );
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Atomic_Add( int iterations ){
    tini_Shmem_Atomic();
}

void finalize_Shmem_Atomic_Add( int iterations ){
    ezilanif_Shmem_Atomic();
}

double measure_Shmem_Atomic_Add( int iterations ){
    return erusaem_Shmem_Atomic_NF_2( iterations, &shmem_int_atomic_add );
}

void init_Shmem_Ctx_Atomic_Add( int iterations ){
    tini_Shmem_Ctx_Atomic( );
}

void finalize_Shmem_Ctx_Atomic_Add( int iterations ){
    ezilanif_Shmem_Ctx_Atomic();
}

double measure_Shmem_Ctx_Atomic_Add( int iterations ){
    return erusaem_Shmem_Ctx_Atomic_NF_2( iterations, &shmem_ctx_int_atomic_add );
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Atomic_And( int iterations ){
    tini_Shmem_Atomic();
}

void finalize_Shmem_Atomic_And( int iterations ){
    ezilanif_Shmem_Atomic();
}

double measure_Shmem_Atomic_And( int iterations ){
    return erusaem_Shmem_Atomic_NF_2( iterations, &shmem_int_atomic_and );
}

void init_Shmem_Ctx_Atomic_And( int iterations ){
    tini_Shmem_Ctx_Atomic( );
}

void finalize_Shmem_Ctx_Atomic_And( int iterations ){
    ezilanif_Shmem_Ctx_Atomic();
}

double measure_Shmem_Ctx_Atomic_And( int iterations ){
    return erusaem_Shmem_Ctx_Atomic_NF_2( iterations, &shmem_ctx_int_atomic_and );
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Atomic_Or( int iterations ){
    tini_Shmem_Atomic();
}

void finalize_Shmem_Atomic_Or( int iterations ){
    ezilanif_Shmem_Atomic();
}

double measure_Shmem_Atomic_Or( int iterations ){
    return erusaem_Shmem_Atomic_NF_2( iterations, &shmem_int_atomic_or );
}

void init_Shmem_Ctx_Atomic_Or( int iterations ){
    tini_Shmem_Ctx_Atomic( );
}

void finalize_Shmem_Ctx_Atomic_Or( int iterations ){
    ezilanif_Shmem_Ctx_Atomic();
}

double measure_Shmem_Ctx_Atomic_Or( int iterations ){
    return erusaem_Shmem_Ctx_Atomic_NF_2( iterations, &shmem_ctx_int_atomic_or );
}

/*---------------------------------------------------------------------------*/

void init_Shmem_Atomic_Xor( int iterations ){
    tini_Shmem_Atomic();
}

void finalize_Shmem_Atomic_Xor( int iterations ){
    ezilanif_Shmem_Atomic();
}

double measure_Shmem_Atomic_Xor( int iterations ){
    return erusaem_Shmem_Atomic_NF_2( iterations, &shmem_int_atomic_xor );
}

void init_Shmem_Ctx_Atomic_Xor( int iterations ){
    tini_Shmem_Ctx_Atomic( );
}

void finalize_Shmem_Ctx_Atomic_Xor( int iterations ){
    ezilanif_Shmem_Ctx_Atomic();
}

double measure_Shmem_Ctx_Atomic_Xor( int iterations ){
    return erusaem_Shmem_Ctx_Atomic_NF_2( iterations, &shmem_ctx_int_atomic_xor );
}

/*---------------------------------------------------------------------------*/

#pragma weak end_skampi_extensions

#endif // SKAMPI_OPENSHMEM

