## Broadcast

begin measurement "Shmem_Bcast_All-procs-short"
      measure comm_pt2pt : Shmem_Bcast_All(256, 0)
end measurement

begin measurement "Shmem_Bcast_All_Rounds-procs-short"
      measure comm_pt2pt : Shmem_Bcast_All_Rounds(256)
end measurement

begin measurement "Shmem_Bcast_All_SK-procs-short"
      measure comm_pt2pt : Shmem_Bcast_All_SK(256, 0)
end measurement

## Reduce

begin measurement "Shmem_Reduce_And_All-procs-short"
      measure comm_pt2pt : Shmem_Reduce_And_All(256, 0)
end measurement

begin measurement "Shmem_Reduce_And_All_Rounds-procs-short"
      measure comm_pt2pt : Shmem_Reduce_And_All_Rounds(256)
end measurement

begin measurement "Shmem_Reduce_And_All_SK-procs-short"
      measure comm_pt2pt : Shmem_Reduce_And_All_SK(256,0)
end measurement

