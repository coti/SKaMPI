begin measurement "Shmem_Malloc"
      measure comm_pt2pt : Shmem_Malloc( 256 )
end measurement

begin measurement "Shmem_Free"
      measure comm_pt2pt : Shmem_Free()
end measurement

