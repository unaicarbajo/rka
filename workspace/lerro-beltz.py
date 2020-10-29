#!/usr/bin/env python3
from ev3dev2.motor import OUTPUT_C, OUTPUT_B, MoveTank
from ev3dev2.sensor import Sensor
from time import sleep
from ev3dev2.sensor import INPUT_2
import random

def main():
    robot = MoveTank(OUTPUT_C,OUTPUT_B)
    lsa = Sensor()

    vl = 25
    vr = 25

    left_per = 0
    right_per = 0


    umbral = 20

    while True:
        # Sentzorea biratuta
        s0 = lsa.value(0)
        s1 = lsa.value(1)
        s2 = lsa.value(2)
        s3 = lsa.value(3)
        s4 = lsa.value(4)
        s5 = lsa.value(5)
        s6 = lsa.value(6)
        s7 = lsa.value(7)

        # print("{},{},{},{},{},{},{},{}".format(s0,s1,s2,s3,s4,s5,s6,s7))
        
        if not(s3 < umbral and s4 < umbral):
            if s0 < umbral and s1 < umbral:
                print("[   ][   ][   ][   ][   ][   ][XXX][XXX]")
                left_per = 0.5
                out = False
            elif s1 < umbral and s2 < umbral:
                print("[   ][   ][   ][   ][   ][XXX][XXX][   ]")
                left_per = 0.7
                out = False
            elif s2 < umbral and s3 < umbral:
                print("[   ][   ][   ][   ][XXX][XXX][   ][   ]")
                left_per = 0.90
                out = False
            elif s7 < umbral and s6 < umbral:
                print("[XXX][XXX][   ][   ][   ][   ][   ][   ]")
                right_per = 0.5
                out = False
            elif s6 < umbral and s5 < umbral:
                print("[   ][XXX][XXX][   ][   ][   ][   ][   ]")
                right_per = 0.7
                out = False
            elif s4 < umbral and s5 < umbral:
                print("[   ][   ][XXX][XXX][   ][   ][   ][   ]")
                right_per = 0.90
                out = False
            elif s0 < umbral:
                print("[   ][   ][   ][   ][   ][   ][   ][XXX]")
                left_per = 0.25
                out = False
            elif s7 < umbral:
                print("[XXX][   ][   ][   ][   ][   ][   ][   ]")
                right_per = 0.25
                out = False
            else:
                print("[   ][   ][   ][   ][   ][   ][   ][   ]")
                if not out:
                    if (left_per == right_per):
                        left_per = random.random()
                        right_per = random.random()
                    elif (left_per > right_per):
                        left_per*=1.1
                    else:
                        right_per*=1.1
                    out =  True

        
        # s3 eta s4 beltz
        else:
            print("[   ][   ][   ][XXX][XXX][   ][   ][   ]")
            left_per = 1
            right_per = 1
            out = False

        robot.on(vl*left_per, vr*right_per)


if __name__ == "__main__":
    main()   