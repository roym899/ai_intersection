To run, first compile using the following

> javac *.java

Then to create the text file / pipe, use the following

> java Spawner < init.dat > spawn.dat

The format of init.dat must be as such:

<initVelocity: double>

<maxVelocity: double>

<maxAcceleration: double>

<numCars: int>

<totalTime: int>

<timestep: double>

<laneLength: int>

There will only be one car generated for a certain timestamp. For a lot of cars to spawn, give a high number of cars
with a small timestep. If printing to the terminal, it'll give a nicely formatted output with a combination of stdout
and stderr. So when using a pipe, it is normal to get a few newlines + error messages in the console if a car couldn't
be spawned.
