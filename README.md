# ai_intersection
Project implementation for AI course at KTH, simulating and scheduling for autonomous car intersection

NOTE: Every directory has a specific README which explains how to use the respective implementation.
Here you can find a summary of the content for each directory.

## centralized
In this directory, the CSP solver can be found.

## satvis
In this directroy, the visualization for the SAT solver can be found.
It uses Processing.

## scala
In this directory, the SAT solver can be found.

## Spawner
In this directory, the spawner for the continuous time case (aka the CSP solver) takes a text file in the following format:
> initial_velocity max_velocity max_acceleration lane_length time_step timestamp_1 lane_1_1 goal_1_1 lane_1_2 goal_1_2 timestamp_2 lane_2_1 ...

lane_i_j is a letter N, S, E, W

all other variables should be doubles


## Visualizer
In this directoy, the visualization for the CSP solver can be found. The input file has the following structure:
> length timestamp 1 0 1 0 N pos, pos, pos, ... E pos, ... S pos, ... W pos, ...

1 0 1 0 refers to the occupied squares on the intersection
