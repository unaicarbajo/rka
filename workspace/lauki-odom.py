#!/usr/bin/env python3
from ev3dev2.motor import OUTPUT_B, OUTPUT_C, MoveDifferential, SpeedRPM, MoveSteering
from ev3dev2.wheel import EV3Tire

def main():
    STUD_MM = 8
    mdiff = MoveDifferential(OUTPUT_B, OUTPUT_C, EV3Tire, 16 * STUD_MM)
    #robot = MoveSteering(OUTPUT_B, OUTPUT_C)
    mdiff.odometry_start(theta_degrees_start=0.0)
    mdiff.on_to_coordinates(SpeedRPM(40), 0, 1000)
    mdiff.turn_to_angle(SpeedRPM(40), 90)
    mdiff.on_to_coordinates(SpeedRPM(40), 1000, 1000)
    mdiff.turn_to_angle(SpeedRPM(40), 90)
    mdiff.on_to_coordinates(SpeedRPM(40), 1000, 0)
    mdiff.turn_to_angle(SpeedRPM(40), 90)
    mdiff.on_to_coordinates(SpeedRPM(40), 0, 0)
    mdiff.turn_to_angle(SpeedRPM(40), 90)
    print("Square finished")
    
if __name__=="__main__":
    main()