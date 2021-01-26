## One-sided communications

begin measurement "Pingpong_Put_Put_fence"
  for count = 1 to MAXSIZE step *sqrt(2) do
    measure comm_pt2pt : Shmem_Pingpong_Put_Put(count, 5)
  od
end measurement

begin measurement "Put_Fence_Bidirectional"
  for count = 1 to MAXSIZE step *sqrt(2) do
    measure comm_pt2pt : Shmem_Put_Fence_Bidirectional(count, 5)
  od
end measurement

begin measurement "Put_Simple"
  for count = 1 to MAXSIZE step *sqrt(2) do
    measure comm_pt2pt : Shmem_Put_Simple(count, 5)
  od
end measurement

begin measurement "Quiet_Put"
  for count = 1 to MAXSIZE step *sqrt(2) do
    measure comm_pt2pt : Shmem_Quiet_Put(count, 5)
  od
end measurement

begin measurement "Put_Full"
  for count = 1 to MAXSIZE step *sqrt(2) do
    measure comm_pt2pt : Shmem_Put_Full(count, 5)
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

