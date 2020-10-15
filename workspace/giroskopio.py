#!/usr/bin/env python3
from ev3dev2.sensor.lego import GyroSensor
from ev3dev2.sensor import INPUT_2
from time import sleep
def main():
    # Lego Gyroscope sensor mode:
    gyro = GyroSensor(INPUT_2)
    #gyro.mode = ’GYRO-G&A’  (Default)
    print("Gyro mode: %s"%(gyro.mode))
    #gyro.reset()
    while True:
        angle, rate = gyro.angle_and_rate
        print("Gyro angle = %d rate = %d"%(angle, rate))
        sleep(0.1)
        
if __name__ == "__main__":
    main()