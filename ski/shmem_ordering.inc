
begin measurement "Quiet"
    measure comm_pt2pt : Shmem_Quiet( 15 )
end measurement

begin measurement "Quiet Put"
    measure comm_pt2pt : Shmem_Quiet_Put( 256, 15 )
end measurement

begin measurement "Fence"
    measure comm_pt2pt : Shmem_Fence( 15 )
end measurement

begin measurement "Fence Put"
    measure comm_pt2pt : Shmem_Fence_Put( 256, 15 )
end measurement

begin measurement "Wait_Until"
    measure comm_pt2pt : Shmem_Wait_Until( 15 )
end measurement

begin measurement "Test"
    measure comm_pt2pt : Shmem_Test( 15 )
end measurement
