## Broadcast

iterations = 10
root = 0

begin measurement "Shmem_Bcast_All"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Bcast_All(iterations, count, root)
  od
end measurement

begin measurement "Shmem_Bcast_All_Synchro"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Bcast_All_Synchro(count, root)
  od
end measurement

begin measurement "Shmem_Bcast_All_Rounds"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Bcast_All_Rounds(iterations, count)
  od
end measurement

begin measurement "Shmem_Bcast_All_SK"
  for count = 4 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Bcast_All_SK(iterations, count, root)
  od
end measurement

## Reduce

begin measurement "Shmem_Reduce_And_All-Consecutive"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Reduce_And_Consecutive(iterations, count)
  od
end measurement

begin measurement "Shmem_Reduce_And_All-Barrier"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Reduce_And_Barrier(iterations, count)
  od
end measurement

begin measurement "Shmem_Reduce_And_All-Synchro"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Reduce_And_Synchro(count)
  od
end measurement


## Collect

begin measurement "Shmem_Collect_All-Consecutive"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Collect_Consecutive(iterations, count)
  od
end measurement

begin measurement "Shmem_Fcollect_All-Consecutive"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Fcollect_Consecutive(iterations, count)
  od
end measurement

begin measurement "Shmem_Collect_All-Barrier"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Collect_Barrier(iterations, count)
  od
end measurement

begin measurement "Shmem_Fcollect_All-Barrier"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Fcollect_Barrier(iterations, count)
  od
end measurement

begin measurement "Shmem_Collect_All-Synchro"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Collect_Synchro(count)
  od
end measurement

begin measurement "Shmem_Fcollect_All-Synchro"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Fcollect_Synchro(count)
  od
end measurement

## All to all

begin measurement "Shmem_Alltoall_All-Consecutive"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Alltoall_Consecutive(iterations, count)
  od
end measurement

begin measurement "Shmem_Alltoalls_All-Consecutive"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Alltoalls_Consecutive(iterations, count)
  od
end measurement

begin measurement "Shmem_Alltoall_All-Barrier"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Alltoall_Barrier(iterations, count)
  od
end measurement

begin measurement "Shmem_Alltoalls_All-Barrier"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Alltoalls_Barrier(iterations, count)
  od
end measurement

begin measurement "Shmem_Alltoall_All-Synchro"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Alltoall_Synchro(count)
  od
end measurement

begin measurement "Shmem_Alltoalls_All-Synchro"
  for count = 1 to MAXSIZE step *sqrt(2) do
      measure comm_pt2pt : Shmem_Alltoalls_Synchro(count)
  od
end measurement

## Barrier

begin measurement "Shmem_Barrier_all"
  measure comm_pt2pt : Shmem_Barrier()
end measurement

begin measurement "Shmem_Barrier_all-Consecutive"
  measure comm_pt2pt : Shmem_Barrier_Consecutive( iterations )
end measurement

begin measurement "Shmem_Barrier_Half"
  measure comm_pt2pt : Shmem_Barrier_Half()
end measurement

begin measurement "Shmem_Barrier_Half-Consecutive"
  measure comm_pt2pt : Shmem_Barrier_Half_Consecutive( iterations )
end measurement

## Sync

begin measurement "Shmem_Sync_all"
  measure comm_pt2pt : Shmem_Sync()
end measurement

begin measurement "Shmem_Sync_all-Consecutive"
  measure comm_pt2pt : Shmem_Sync_Consecutive( iterations )
end measurement

begin measurement "Shmem_Sync_Half"
  measure comm_pt2pt : Shmem_Sync_Half()
end measurement

begin measurement "Shmem_Sync_Half-Consecutive"
  measure comm_pt2pt : Shmem_Sync_Half_Consecutive( iterations )
end measurement
