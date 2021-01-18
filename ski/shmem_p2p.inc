## One-sided communications

begin measurement "Pingpong_Put_Put_fence"
  for count = 1 to MAXSIZE step *sqrt(2) do
    measure comm_pt2pt : Shmem_Pingpong_Put_Put(count, 5)
  od
end measurement

