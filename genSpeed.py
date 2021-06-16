#!/usr/bin/env python3

import matplotlib.pyplot as plt
import math


def main():

    f1 = open('values.txt', 'r')
    lines = f1.readlines()
    f1.close()


    nodeId = 42 # this is the id of the node to plot the location for
    speeds = []

    locX = []
    locY = []
    times = set()


    for line in lines:

        vals = line.strip().split(' ')

        time = int(int(vals[0])/1000000000)
        n = int(vals[1])
        x = float(vals[2])
        y = float(vals[3])
        vx = float(vals[4])
        vy = float(vals[5])

        v = math.sqrt(vx**2 + vy**2)

        if n == nodeId:
            locX.append(x)
            locY.append(y)

        times.add(time)

        # add the speed to the list
        if len(speeds) == time+1:
            speeds[time] += v
        else:
            speeds.append(v)

    numNodes = 500
    speeds = list(map(lambda x: x/numNodes, speeds))
    avgSpeed = sum(speeds)/len(speeds)

    times = list(times)

    plt.title(f'Average speed of {numNodes} nodes over 4 hours of simulated time in a\n1km x 1km area using the Random Waypoint Mobility Model')
    plt.xlabel("time (s)")
    plt.ylabel("average speed (m/s)")
    plt.plot(times, speeds)
    plt.hlines(avgSpeed, 0, 14400, color='red', label=str(f'average velocity {avgSpeed} m/s'))
    plt.savefig("speeds.png", dpi=600)

    plt.cla()
    plt.clf()

    plt.title(f'Trajectory of node {nodeId} over a 4 hour simulation in a 1km x 1km area\n using the Random Waypoint Mobility Model')
    plt.xlabel("x location (m)")
    plt.ylabel("y location (m)")
    plt.plot(locX, locY)
    plt.plot(locX[0], locY[0], 'om')
    plt.plot(locX[-1], locY[-1], 'vk')
    plt.axes([0,1000,0,1000])
    plt.savefig("trajectory.png", dpi=600)



if __name__ == "__main__":
    main()
