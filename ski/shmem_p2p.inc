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

