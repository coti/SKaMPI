begin measurement "Shmem_Malloc"
      measure comm_pt2pt : Shmem_Malloc( 10, 256 )
end measurement

begin measurement "Shmem_Free"
      measure comm_pt2pt : Shmem_Free( 10 )
end measurement

begin measurement "Shmem_Realloc"
      measure comm_pt2pt : Shmem_Realloc( 10, 256 )
end measurement

begin measurement "Shmem_Align"
      measure comm_pt2pt : Shmem_Align( 10, 256 )
end measurement

