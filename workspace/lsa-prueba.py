#!/usr/bin/env python3
from ev3dev2.sensor import Sensor
from time import sleep

def printLSA(lsa):
    for i in range(0,8):
        print("i=%d value=%d"%(i, lsa.value(i)))
        print("--------------------------------")
        
def main():
    lsa = Sensor()
    while True:
        printLSA(lsa)
        sleep(0.1)
        
if __name__ == "__main__":
    main()