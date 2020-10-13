#!/usr/bin/env python3
from ev3dev2.motor import OUTPUT_B, OUTPUT_C, MoveSteering
from time import sleep

def forward(rob, s, v, tsec):
    rob.on_for_seconds(s, v, tsec)
    
def turnLeft(rob, s, v, tsec):
    # turn left on the spot
    rob.on_for_seconds(s, v, tsec)
    
def main():
    robot = MoveSteering(OUTPUT_C, OUTPUT_B)
    for _ in range(0, 4):
        forward(robot, 0, 50, 4)
        turnLeft(robot, 50, 21, 2)
        
    print("Square finished")

if __name__=="__main__":
    main()