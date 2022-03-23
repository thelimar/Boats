# Boats
Solving Boats problem using semafors.

This program simulates interaction between several different processes: one 'Capitain' process and N 'Passengers' processes. Processes interact via semafor buffers in a following way: each passenger needs to ride a boat several times; only 2 passengers can board or get off the ship at the same time; Capitain starts the ride when all the seats are occupied and announces boarding to the new ride when all passengers have left the ship. These intaractions are perfectly simulated using semafors, since one operation with semafors works like a transaction: either all actions in the operation are completed or none.
