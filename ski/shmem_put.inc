set_min_repetitions(8)
set_max_repetitions(30)
set_max_relative_standard_error(0.03)

num_procs = get_comm_size(MPI_COMM_WORLD)
comm_pt2pt = MPI_COMM_WORLD
# set_skampi_buffer_mpi_alloc_mem(4mb,MPI_INFO_NULL)

MAXSIZE = 65536

set_skampi_buffer( MAXSIZE )

begin measurement "Pingpong_Put_Put_fence"
  for count = 1 to MAXSIZE step *sqrt(2) do
    measure comm_pt2pt : Shmem_Pingpong_Put_Put(count, 5)
  od
end measurement

begin measurement "Shmem_Bcast_All-procs-short"
      measure comm_pt2pt : Shmem_Bcast_All(256, 0)
end measurement
