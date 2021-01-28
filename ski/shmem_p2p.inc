## One-sided communications

begin measurement "Put_Simple"
  for count = 1 to MAXSIZE step *sqrt(2) do
    measure comm_pt2pt : Shmem_Put_Simple(count, 5)
  od
end measurement

begin measurement "Pingpong_Put_Put"
  for count = 1 to MAXSIZE step *sqrt(2) do
    measure comm_pt2pt : Shmem_Pingpong_Put_Put(count, 5)
  od
end measurement

begin measurement "Put_Round"
  for count = 1 to MAXSIZE step *sqrt(2) do
    measure comm_pt2pt : Shmem_Put_Round(count, 5)
  od
end measurement

begin measurement "Put_Full"
  for count = 1 to MAXSIZE step *sqrt(2) do
    measure comm_pt2pt : Shmem_Put_Full(count, 5)
  od
end measurement

begin measurement "P_Simple"
  measure comm_pt2pt : Shmem_P_Simple( 5)
end measurement

begin measurement "P_Round"
  measure comm_pt2pt : Shmem_P_Round( 5)
end measurement

begin measurement "Iput_Round"
  stride = 16	  
  for count = 1 to MAXSIZE/stride step *sqrt(2) do
    measure comm_pt2pt : Shmem_Iput_Round(count, stride, 5)
  od
end measurement

begin measurement "Put_Nonblocking_Post"
  for count = 1 to MAXSIZE step *sqrt(2) do
    measure comm_pt2pt : Shmem_Put_Nonblocking_Post(count, 5)
  od
end measurement

begin measurement "Put_Nonblocking_Full"
  for count = 1 to MAXSIZE step *sqrt(2) do
    measure comm_pt2pt : Shmem_Put_Nonblocking_Full(count, 5)
  od
end measurement

begin measurement "Put_Nonblocking_Quiet"
  for count = 1 to MAXSIZE step *sqrt(2) do
    measure comm_pt2pt : Shmem_Put_Nonblocking_Quiet(count, 5)
  od
end measurement

begin measurement "Put_Nonblocking_Overlap"
  for count = 1 to MAXSIZE step *sqrt(2) do
    measure comm_pt2pt : Shmem_Put_Nonblocking_Overlap(count, 5)
  od
end measurement

######################################################################

begin measurement "Get_Simple"
  for count = 1 to MAXSIZE step *sqrt(2) do
    measure comm_pt2pt : Shmem_Get_Simple(count, 5)
  od
end measurement

begin measurement "Get_Round"
  for count = 1 to MAXSIZE step *sqrt(2) do
    measure comm_pt2pt : Shmem_Get_Round(count, 5)
  od
end measurement

begin measurement "Iget_Round"
  stride = 16
  for count = 1 to MAXSIZE/stride step *sqrt(2) do
    measure comm_pt2pt : Shmem_Iget_Round(count, stride, 5)
  od
end measurement

begin measurement "Get_Nonblocking_Post"
  for count = 1 to MAXSIZE step *sqrt(2) do
    measure comm_pt2pt : Shmem_Get_Nonblocking_Post(count, 5)
  od
end measurement

begin measurement "Get_Nonblocking_Full"
  for count = 1 to MAXSIZE step *sqrt(2) do
    measure comm_pt2pt : Shmem_Get_Nonblocking_Full(count, 5)
  od
end measurement

begin measurement "Get_Nonblocking_Quiet"
  for count = 1 to MAXSIZE step *sqrt(2) do
    measure comm_pt2pt : Shmem_Get_Nonblocking_Quiet(count, 5)
  od
end measurement

begin measurement "Get_Nonblocking_Overlap"
  for count = 1 to MAXSIZE step *sqrt(2) do
    measure comm_pt2pt : Shmem_Get_Nonblocking_Overlap(count, 5)
  od
end measurement

begin measurement "G_Round"
  measure comm_pt2pt : Shmem_G_Round( 5)
end measurement

begin measurement "G_Simple"
  measure comm_pt2pt : Shmem_G_Simple( 5)
end measurement

