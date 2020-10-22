#!/usr/bin/env python3
from ev3dev2.motor import OUTPUT_C, OUTPUT_B, MoveTank
from ev3dev2.sensor import Sensor
from time import sleep
from ev3dev2.sensor import INPUT_2

def main():
    robot = MoveTank(OUTPUT_C,OUTPUT_B)
    lsa = Sensor()

    vl = 25
    vr = 25

    umbral = 20

    while True:
        s0 = lsa.value(0)
        s1 = lsa.value(1)
        s2 = lsa.value(2)
        s3 = lsa.value(3)
        s4 = lsa.value(4)
        s5 = lsa.value(5)
        s6 = lsa.value(6)
        s7 = lsa.value(7)

        print("{},{},{},{},{},{},{},{}".format(s0,s1,s2,s3,s4,s5,s6,s7))
        left_per = 1
        right_per = 1

        if not(s3 < umbral and s4 < umbral):
            if s0 < umbral and s1 < umbral:
                left_per = 0.5
            elif s1 < umbral and s2 < umbral:
                left_per = 0.7
            elif s2 < umbral and s3 < umbral:
                left_per = 0.90
            elif s7 < umbral and s6 < umbral:
                right_per = 0.5
            elif s6 < umbral and s5 < umbral:
                right_per = 0.7
            elif s4 < umbral and s5 < umbral:
                right_per = 0.90
            elif s0 < umbral:
                left_per = 0.15
            elif s7 < umbral:
                right_per = 0.15
            else:
                left_per = 0
                right_per = 0


        robot.on(vl*left_per, vr*right_per)


if __name__ == "__main__":
    main()   