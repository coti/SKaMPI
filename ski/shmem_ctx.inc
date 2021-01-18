## Context management

begin measurement "Shmem_Context_Create_serialized"
      measure comm_pt2pt : Shmem_Ctx_Create_Serialized()
end measurement

begin measurement "Shmem_Context_Destroy_serialized"
      measure comm_pt2pt : Shmem_Ctx_Destroy_Serialized()
end measurement

begin measurement "Shmem_Context_Create_private"
      measure comm_pt2pt : Shmem_Ctx_Create_Private()
end measurement

begin measurement "Shmem_Context_Destroy_private"
      measure comm_pt2pt : Shmem_Ctx_Destroy_Private()
end measurement

begin measurement "Shmem_Context_Create_nostore"
      measure comm_pt2pt : Shmem_Ctx_Create_Nostore()
end measurement

begin measurement "Shmem_Context_Destroy_nostore"
      measure comm_pt2pt : Shmem_Ctx_Destroy_Nostore()
end measurement

