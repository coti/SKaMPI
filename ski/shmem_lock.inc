begin measurement "Shmem_Lock_Test_Busy"
      measure comm_pt2pt : Shmem_Lock_Test_Busy()
end measurement

begin measurement "Shmem_Lock_Test_Busy_All"
      measure comm_pt2pt : Shmem_Lock_Test_Busy_All()
end measurement

begin measurement "Shmem_Lock_Test_Busy_Turns"
      measure comm_pt2pt : Shmem_Lock_Test_Busy_Turns()
end measurement

begin measurement "Shmem_Lock_Test_Busy_Round"
      measure comm_pt2pt : Shmem_Lock_Test_Busy_Round()
end measurement

begin measurement "Shmem_Clear_Lock"
      measure comm_pt2pt : Shmem_Clear_Lock()
end measurement

begin measurement "Shmem_Set_Lock"
      measure comm_pt2pt : Shmem_Set_Lock()
end measurement

