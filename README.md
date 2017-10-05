# ai_intersection
Project implementation for AI course at KTH, simulating and scheduling for autonomous car intersection

## Pipe structure

### Spawner
> initial_velocity max_velocity max_acceleration lane_length time_step timestamp_1 lane_1_1 lane_1_2 timestamp_2 lane_2_1 ...

lane_i_j is a letter N, S, E, W

all other variables should be doubles

### Visualization
> length timestamp 1 0 1 0 N pos, pos, pos, ... E pos, ... S pos, ... W pos, ...

1 0 1 0 refers to the occupied squares on the intersection
