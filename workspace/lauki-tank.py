#!/usr/bin/env python3
from ev3dev2.motor import OUTPUT_C, OUTPUT_B, MoveTank
from time import sleep

def forward(tank, v, tsec):
    tank.on_for_seconds(v, v, seconds = tsec)

def turnLeft(tank, vl, vr, tsec):
    # turn left on the spot
    tank.on_for_seconds(vl, vr, seconds = tsec)

def main():
    # The MoveTank class provides the simplest way to drive two motors
    # # left motor D
    # # right motor A
    
    robot = MoveTank(OUTPUT_B, OUTPUT_C)
    for _ in range(0, 4):
        forward(robot, 50, 4)
        turnLeft(robot, 0, 19, 2)
    print("Square finished")
    
if __name__=="__main__":
    main()