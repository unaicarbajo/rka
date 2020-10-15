#!/usr/bin/env python3
from ev3dev2.sensor.lego import ColorSensor
from time import sleep
from ev3dev2.sensor import INPUT_1

def main():
    eye = ColorSensor(INPUT_1)
    for i in range(0, 20):
        print(eye.color_name, eye.color)
        sleep(0.5)

if __name__ == "__main__":
    main()   