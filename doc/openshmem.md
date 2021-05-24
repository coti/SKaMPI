[[_TOC_]]


# Notes specific to OpenSHMEM

There is no such thing as communicators in OpenSHMEM as of version 1.4. Therefore, all the measurements must be done in MPI_COMM_WORLD:
```
comm_pt2pt = MPI_COMM_WORLD
measure comm_pt2pt : Shmem_Put_Simple(10, 5)
```

# New functions

## Set unit

```
set_unit(1000000000)
```

This function is useful to get more precise results (in case of very fast communications). Default value: 1000000 (1e6: measurements given in microseconds). Scientific notation is not supported yet.

# Point to point communications

The time taken by point-to-point communications can refer to several time intervals, according to the OpenSHMEM communication model. We provide several measurement routines in order to make it possible to measure them. In particular, the OpenSHMEM standard states that "these routines (put-based routines) start the remote transfer and may return before the data is delivered to the remote PE". Hence, we give the possibility to measure the time to post the communication and the total time to ensure its completion.

## Put

* `Shmem_Put_Simple`

Measures the time to perform a simple put operation between process 0 and process 1. Process 0 only measures the time. The time measurement step includes a call to `shmem_quiet` in order to ensure completion of the data transfer. Outside of the data transfer measurement, we are measuring the time to complete an "empty" `shmem_quiet` and substracting this time from the total time.

```
Shmem_Put_Simple( int count, int iterations )
```


* `Shmem_Pingpong_Put_Put`

Processes 0 and 1 simultaneously put data on each other. Like `Shmem_Put_Simple`, the time to perform an empty `shmem_quiet` is substracted from the total time.

```
Shmem_Pingpong_Put_Put( int count, int iterations )
```

* `Shmem_Put_Round`

Processes are putting data into their +1 neighbor's memory in a round fashion. Like `Shmem_Put_Simple`, the time to perform an empty `shmem_quiet` is substracted from the total time.

```
Shmem_Put_Round( int count, int iterations )
```

* `Shmem_Put_Full`

Processes are putting data into their +1 neighbor's memory in a round fashion. Unlike the previous measurement routines, this routine counts the time to perform `shmem_quiet`.

```
Shmem_Put_Full( int count, int iterations )
```

* `Shmem_Iput_Round`

Measures the time to perform a strided put operation between processes that communicate in a ring fashion. Like `Shmem_Put_Simple`, the time to perform an empty `shmem_quiet` is substracted from the total time.

```
Shmem_Iput_Round( int count, int stride, int iterations )
```

Attention: the size of the buffer used for communications is set by SKaMPI by:
```
set_skampi_buffer( MAXSIZE )
```

Therefore, the *strided* data must fit in this buffer!

```
begin measurement "Iput_Round"
  stride = 16	  
  for count = 1 to MAXSIZE/stride step *sqrt(2) do
    measure comm_pt2pt : Shmem_Iput_Round(count, stride, 5)
  od
end measurement
```

* `Shmem_P_Simple`

Measures the time to perform a `shmem_p` from process 0 to process 1. Process 0 only measures the time. The time measurement step includes a call to shmem_quiet in order to ensure completion of the data transfer. Outside of the data transfer measurement, we are measuring the time to complete an "empty" shmem_quiet and substracting this time from the total time.

```
Shmem_P_Simple( int iterations )
```

* `Shmem_P_Round`

Measures the time to perform a `shmem_p` operation between processes that communicate in a ring fashion. The time measurement step includes a call to shmem_quiet in order to ensure completion of the data transfer. Outside of the data transfer measurement, we are measuring the time to complete an "empty" shmem_quiet and substracting this time from the total time.

```
Shmem_P_Round( int iterations ) 
```


## Non-blocking put

* `Shmem_Put_Nonblocking_Post`

Measures the time to post a non-blocking `shmem_put_nbi`, and only the posting. Completion is ensured with `shmem_quiet`, which happens outside of the timing. Processes communicate in a ring fashion.

```
Shmem_Put_Nonblocking_Post( int count, int iterations )
```

* `Shmem_Put_Nonblocking_Quiet`

Measures the time to perform the `shmem_quiet` call that follows a call to a non-blocking put (happens outside of the timing). Processes communicate in a ring fashion. Since `shmem_quiet` is used to ensure completion of the (non-blocking) put operations issued before, this function is used to measure how long it takes to complete a previously posted non-blocking put operation.

```
Shmem_Put_Nonblocking_Quiet( int count, int iterations )
```

* `Shmem_Put_Nonblocking_Full`

Measures the time to post a non-blocking `shmem_put_nbi` and its completion with `shmem_quiet`. Processes communicate in a ring fashion.

```
Shmem_Put_Nonblocking_Full( int count, int iterations );
```

* `Shmem_Put_Nonblocking_Overlap`

This routine can be used to measure the percentage of overlap achieved. First, it measures the time taken by blocking operations. Then it posts a non-blocking put, and sleeps during 2x the time to perform the blocking operation. Then the time pent in shmem_quiet is measured. It returns he time spent in shemm_quiet. The overlap can be obtained by the ratio between the time spent in posting the operation and waiting for it to complete (returned here) and the time to perform the blocking operation. Processes communicate in a ring fashion. 

```
Shmem_Put_Nonblocking_Overlap( int count, int iterations )

```

## Get

Get-based routines are more straightforward to measure because the routine returns when the communication has completed and the data is available in the destination memory. 

* `Shmem_Get_Simple`

Measures the time to perform a simple get operation between process 0 and process 1: process 0 pulls data from process 1. Process 0 only measures the time.

```
Shmem_Get_Simple( int count, int iterations )
```

* `Shmem_Get_Round`

Measures the time to perform a get operation between processes that communicate in a ring fashion. 

```
Shmem_Get_Round( int count, int iterations )
```

* `Shmem_Iget_Round`

Measures the time to perform a strided get operation between processes that communicate in a ring fashion. 

```
Shmem_Iget_Round( int count, int stride, int iterations )
```

Attention: the size of the buffer used for communications is set by SKaMPI by:
```
set_skampi_buffer( MAXSIZE )
```

Therefore, the *strided* data must fit in this buffer!
```
begin measurement "Iget_Round"
  stride = 16	  
  for count = 1 to MAXSIZE/stride step *sqrt(2) do
    measure comm_pt2pt : Shmem_Iget_Round(count, stride, 5)
  od
end measurement
```

* `Shmem_G_Simple`

Measures the time to perform a `shmem_g` from process 0 to process 1. Process 0 only measures the time. 

```
Shmem_G_Simple( int iterations ) 
```

* `Shmem_G_Round`

Measures the time to perform a `shmem_g` operation between processes that communicate in a ring fashion. 

```
Shmem_G_Round( int iterations ) 
```

## Non-blocking get

* `Shmem_Get_Nonblocking_Post`

Measures the time to post a non-blocking `shmem_get_nbi`, and only the posting. Completion is ensured with `shmem_quiet`, which happens outside of the timing. Processes communicate in a ring fashion.

```
Shmem_Get_Nonblocking_Post( int count, int iterations )
```

* `Shmem_Get_Nonblocking_Full`

Measures the time to post a non-blocking `shmem_get_nbi` and its completion with `shmem_quiet`. Processes communicate in a ring fashion.

```
Shmem_Get_Nonblocking_Full( int count, int iterations )
```

* `Shmem_Get_Nonblocking_Quiet`

Measures the time to perform the `shmem_quiet` call that follows a call to a non-blocking get (happens outside of the timing). Processes communicate in a ring fashion. Since `shmem_quiet` is used to ensure completion of the (non-blocking) get operations issued before, this function is used to measure how long it takes to complete a previously posted non-blocking get operation.

```
Shmem_Get_Nonblocking_Quiet( int count, int iterations )
```

* `Shmem_Get_Nonblocking_Overlap`

This routine can be used to measure the percentage of overlap achieved. First, it measures the time taken by blocking operations. Then it posts a non-blocking get, and sleeps during 2x the time to perform the blocking operation. Then the time pent in shmem_quiet is measured. It returns he time spent in shemm_quiet. The overlap can be obtained by the ratio between the time spent in posting the operation and waiting for it to complete (returned here) and the time to perform the blocking operation. Processes communicate in a ring fashion. 

```
Shmem_Get_Nonblocking_Overlap( int count, int iterations )
```

# Collective communications

Measuring collective communication routines is not trivial. We will document this later.

## Broadcast

* `Shmem_Bcast_All`
 

```
Shmem_Bcast_All( int count, int root )
```

* `Bcast_All_Rounds`

This benchmark method is called the `broadcast rounds` benchmark method: each rank  of the communicator / team initiates a broadcast. There is still a possibillity of pipeling, depending on the order used to cycle over the tasks. 

```
Bcast_All_Rounds( int count, int root )
```

* `Shmem_Bcast_All_SK`

```   
Shmem_Bcast_All_SK( int count, int root )
```

* `Shmem_Bcast_All_Synchro`

This method relies on SKaMPI's internal repetition and synchronization mechanisms. 
```
Shmem_Bcast_All_Synchro( int count , int root )
```

```
measure comm_pt2pt : Shmem_Bcast_All(iterations, count, root)
measure comm_pt2pt : Shmem_Bcast_All_Synchro(count, root)
measure comm_pt2pt : Shmem_Bcast_All_Rounds(iterations, count)
measure comm_pt2pt : Shmem_Bcast_All_SK(iterations, count, root)
```
	
## Reduce
	 
```
measure comm_pt2pt : Shmem_Reduce_And_Consecutive(iterations, count)
measure comm_pt2pt : Shmem_Reduce_And_Barrier(iterations, count)
measure comm_pt2pt : Shmem_Reduce_And_Synchro(count)
```

## Collect

```
measure comm_pt2pt : Shmem_Collect_Consecutive(iterations, count)
measure comm_pt2pt : Shmem_Fcollect_Consecutive(iterations, count)
measure comm_pt2pt : Shmem_Collect_Barrier(iterations, count)
measure comm_pt2pt : Shmem_Fcollect_Barrier(iterations, count)
measure comm_pt2pt : Shmem_Collect_Synchro(count)
measure comm_pt2pt : Shmem_Fcollect_Synchro(count)
```
## All to all

```
measure comm_pt2pt : Shmem_Alltoall_Consecutive( iterations, count )
measure comm_pt2pt : Shmem_Alltoalls_Consecutive( iterations, count )
measure comm_pt2pt : Shmem_Alltoall_Barrier( iterations, count )
measure comm_pt2pt : Shmem_Alltoalls_Barrier( iterations, count )
measure comm_pt2pt : Shmem_Alltoall_Synchro( count )
measure comm_pt2pt : Shmem_Alltoalls_Synchro( count )
```

## Barrier

```
measure comm_pt2pt : Shmem_Barrier()
measure comm_pt2pt : Shmem_Barrier_Consecutive( iterations )
measure comm_pt2pt : Shmem_Barrier_Half()
measure comm_pt2pt : Shmem_Barrier_Half_Consecutive( iterations )
```

## Sync

```
measure comm_pt2pt : Shmem_Sync()
measure comm_pt2pt : Shmem_Sync_Consecutive( iterations )
measure comm_pt2pt : Shmem_Sync_Half()
measure comm_pt2pt : Shmem_Sync_Half_Consecutive( iterations )
```

# Locks

### Shmem_Lock_Test_Busy

Process 1 takes the lock, process 0 tests it. In order to make sure that process 1 has taken the lock before process 1 tests it, we are performing a barrier after the call to `shmem_set_lock` on process 1 and before the call to `shmem_test_lock` on process 0.

Using this version, only process 0 will measure time. It also depends on the topology used by the underlying collective operation.

```
measure comm_pt2pt : Shmem_Lock_Test_Busy()
```


### Shmem_Lock_Test_Busy_All

To address this last issue, a given process (the last one) is taking the lock and all the other ones are testing it. They can test it in turns or concurrently. In this function, they are *all* testing the lock concurrently.

Potential for pipelining?

```
measure comm_pt2pt : Shmem_Lock_Test_Busy_All()
```


### Shmem_Lock_Test_Busy_Turns

Same but in turns instead of concurrently.

```
measure comm_pt2pt : Shmem_Lock_Test_Busy_Turns()
```


### Shmem_Lock_Test_Busy_Round

Since the time might depend on the topology and concurrent tests might flood the PEs' access to their symmetric heap,

- Each process becomes the locker, in turns
- For each turn, the locker locks
- All the other processes, in turns, test the lock
- The function returns the average.
    Barriers ensure that no two processes are testing the lock concurrently.

```
measure comm_pt2pt : Shmem_Lock_Test_Busy_Round()
```

### Other lock-related routines

```
measure comm_pt2pt : Shmem_Clear_Lock()
measure comm_pt2pt : Shmem_Set_Lock()
```


# Memory management routines

Memory management routines take the number of repetitions and, when relevant, the size of the buffer to allocate. `Shmem_Align` aligns the buffer on 2. `Shmem_Realloc` allocates half of the requested size using `shmem_malloc`, and reallocates to the requested size using `shmem_realloc`. The initial allocation is not counted in the returned time.

```
measure comm_pt2pt : Shmem_Malloc( count, size )
measure comm_pt2pt : Shmem_Free( count )
measure comm_pt2pt : Shmem_Realloc( count, size )
measure comm_pt2pt : Shmem_Align( count, size )
measure comm_pt2pt : Shmem_Calloc( 10, 64, 4 )
```

# Communication management routines

# Contexts

## Create a communication context

Contexts can be created with three options: `SHMEM_CTX_SERIALIZED`, `SHMEM_CTX_PRIVATE` and `SHMEM_CTX_NOSTORE`. A measurement function exists for each of these possibilities:

* `Shmem_Ctx_Create_Serialized`
* `Shmem_Ctx_Create_Private`
* `Shmem_Ctx_Create_Nostore`

For example:
```
begin measurement "Shmem_Context_Create_serialized"
      measure comm_pt2pt : Shmem_Ctx_Create_Serialized()
end measurement
```

We recommend to call creation and destruction context measurement routines of the same type in order rather than interleaved, otherwise the execution may crash during the finalization of the OpenSHMEM library.

```
measure comm_pt2pt : Shmem_Ctx_Create_Serialized()
measure comm_pt2pt : Shmem_Ctx_Destroy_Serialized()
measure comm_pt2pt : Shmem_Ctx_Create_Private()
measure comm_pt2pt : Shmem_Ctx_Destroy_Private()
measure comm_pt2pt : Shmem_Ctx_Create_Nostore()
measure comm_pt2pt : Shmem_Ctx_Destroy_Nostore()
```

## Destroy a communication contex

In a similar way, three measurement functions exist for context destruction:

* `Shmem_Ctx_Destroy_Serialized`
* `Shmem_Ctx_Destroy_Private`
* `Shmem_Ctx_Destroy_Nostore`

For example:
```
begin measurement "Shmem_Context_Destroy_private"
      measure comm_pt2pt : Shmem_Ctx_Destroy_Private()
end measurement
```

# Memory ordering routines

We provide measurement routines for memory ordering functions.

```
measure comm_pt2pt : Shmem_Quiet( count )
measure comm_pt2pt : Shmem_Fence( count )
measure comm_pt2pt : Shmem_Wait_Until( count )
measure comm_pt2pt : Shmem_Test( count )
```

`shmem_fence` does not guarantee delivery of the previously posted communications but it guarantees they will be delivered in  order. It might have a different cost  when posted after a communication (such as a put). Hence, we are providing `Shmem_Fence_Put` that measures the cost of a `shmem_fence` following a `shmem_put`. The time to post `shmem_put` is not included in the measured time. These routines take the number of iterations and the number of bytes to send in she put.

```
measure comm_pt2pt : Shmem_Quiet_Put( count, nb )
measure comm_pt2pt : Shmem_Fence_Put( count, nb )
```

# Distributed locking routines

## Take a lock

Measure the time to take a lock: `Shmem_Set_Lock`.

```
begin measurement "Shmem_Set_Lock"
      measure comm_pt2pt : Shmem_Set_Lock()
end measurement
```

## Release a lock

Measure the time to release a lock: `Shmem_Clear_Lock`. The lock is taken outside of the measured part.

```
begin measurement "Shmem_Clear_Lock"
      measure comm_pt2pt : Shmem_Clear_Lock()
end measurement
```

## Test a lock

* Only process 0 measures time: `Shmem_Lock_Test_Busy`

Process 1 takes the lock, holds it for a while (10 times the time to perform a barrier), meanwhile process 0 tests it. As a consequence, process 0 tests a lock which is held by another process.

```
begin measurement "Shmem_Lock_Test_Busy"
      measure comm_pt2pt : Shmem_Lock_Test_Busy()
end measurement
```

* All the processes measure time: `Shmem_Lock_Test_Busy_All`

A process takes the lock during the initialization (the last one). Then all the processes test the lock, without any ordering (therefore, in a concurrent way).

```
begin measurement "Shmem_Lock_Test_Busy_All"
      measure comm_pt2pt : Shmem_Lock_Test_Busy_All()
end measurement
```

* All the process measure time, try to take the lock in turns: `Shmem_Lock_Test_Busy_Turns`

The lock is held by the last process. All the processes are trying to get the lock, in turns.

```
begin measurement "Shmem_Lock_Test_Busy_Turns"
      measure comm_pt2pt : Shmem_Lock_Test_Busy_Turns()
end measurement
```

* A neighbor has the lock: `Shmem_Lock_Test_Busy_Round`

Using this function, each process tests the lock in turns. A process holds the lock and another ont tests it.

```
begin measurement "Shmem_Lock_Test_Busy_Round"
      measure comm_pt2pt : Shmem_Lock_Test_Busy_Round()
end measurement
```

