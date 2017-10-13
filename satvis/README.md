Here are some simulations already computed called "simulationX.txt"
If you have Processing, you can open satvis.pde and launch the simulation by only tweaking two parameters:
1) On void setup(), you can find on the first line "frameRate(x)" where x is the speed you wish for the simulation (by default 1).
2) on void setup(), you can find on the third line "String[] lines = loadStrings("simulationX.txt");" where you should choose x between 1 and 7.
