## Broadcast

begin measurement "Shmem_Bcast_All"
  for count = 1 to MAXSIZE step *sqrt(2) do
#      measure comm_pt2pt : Shmem_Bcast_All(count, 0, 10)
  od
end measurement

begin measurement "Shmem_Bcast_All_Synchro"
  for count = 1 to MAXSIZE step *sqrt(2) do
#      measure comm_pt2pt : Shmem_Bcast_All_Synchro(count, 0)
  od
end measurement

begin measurement "Shmem_Bcast_All_Rounds"
  for count = 1 to MAXSIZE step *sqrt(2) do
#      measure comm_pt2pt : Shmem_Bcast_All_Rounds(count, 10)
  od
end measurement

begin measurement "Shmem_Bcast_All_SK"
  for count = 4 to MAXSIZE step *sqrt(2) do
#      measure comm_pt2pt : Shmem_Bcast_All_SK(count, 0, 15)
  od
end measurement

## Reduce

begin measurement "Shmem_Reduce_And_All-Consecutive"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Reduce_And_Consecutive(count, 10)
  od
end measurement

begin measurement "Shmem_Reduce_And_All-Barrier"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Reduce_And_Barrier(count, 10)
  od
end measurement

begin measurement "Shmem_Reduce_And_All-Synchro"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Reduce_And_Synchro(count)
  od
end measurement


## Collect

begin measurement "Shmem_Reduce_Collect_All-Consecutive"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Collect_Consecutive(count, 10)
  od
end measurement

begin measurement "Shmem_Reduce_Fcollect_All-Consecutive"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Fcollect_Consecutive(count, 10)
  od
end measurement

begin measurement "Shmem_Reduce_Collect_All-Barrier"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Collect_Barrier(count, 10)
  od
end measurement

begin measurement "Shmem_Reduce_Fcollect_All-Barrier"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Fcollect_Barrier(count, 10)
  od
end measurement

begin measurement "Shmem_Reduce_Collect_All-Synchro"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Collect_Synchro(count)
  od
end measurement

begin measurement "Shmem_Reduce_Fcollect_All-Synchro"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Fcollect_Synchro(count)
  od
end measurement

## All to all

begin measurement "Shmem_Reduce_Alltoall_All-Consecutive"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Alltoall_Consecutive(count, 10)
  od
end measurement

begin measurement "Shmem_Reduce_Alltoalls_All-Consecutive"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Alltoalls_Consecutive(count, 10)
  od
end measurement

begin measurement "Shmem_Reduce_Alltoall_All-Barrier"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Alltoall_Barrier(count, 10)
  od
end measurement

begin measurement "Shmem_Reduce_Alltoalls_All-Barrier"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Alltoalls_Barrier(count, 10)
  od
end measurement

begin measurement "Shmem_Reduce_Alltoall_All-Synchro"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Alltoall_Synchro(count)
  od
end measurement

begin measurement "Shmem_Reduce_Alltoalls_All-Synchro"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Alltoalls_Synchro(count)
  od
end measurement

## Barrier

begin measurement "Shmem_Barrier_all"
  measure comm_pt2pt : Shmem_Barrier()
end measurement
