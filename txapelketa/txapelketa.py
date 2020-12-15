#!/usr/bin/env python3
from ev3dev2.motor import OUTPUT_C, OUTPUT_B, MoveTank
from ev3dev2.sensor import Sensor
from ev3dev2.sensor.lego import ColorSensor
from ev3dev2.sensor import INPUT_1
from ev3dev2.sensor import INPUT_4
import random
from time import sleep
from threading import Thread


class lerroJarraitzailea(Thread):
    def __init__(self, threadID, vl, vr):
        Thread.__init__(self)
        self.threadID = threadID
        self.running = True
        self.lsa = Sensor(INPUT_1)
        self.vl = vl
        self.vr = vr
        self.robot =  MoveTank(OUTPUT_C,OUTPUT_B)
        
    def run(self):
        out = True
        left_per = 0
        right_per = 0
        umbral = 20
        while self.running:
            # Sentzorea biratuta
            s0 = self.lsa.value(0)
            s1 = self.lsa.value(1)
            s2 = self.lsa.value(2)
            s3 = self.lsa.value(3)
            s4 = self.lsa.value(4)
            s5 = self.lsa.value(5)
            s6 = self.lsa.value(6)
            s7 = self.lsa.value(7)

            # print("{},{},{},{},{},{},{},{}".format(s0,s1,s2,s3,s4,s5,s6,s7))
            if not(s3 < umbral and s4 < umbral):
                if s0 < umbral and s1 < umbral:
                    #print("[   ][   ][   ][   ][   ][   ][XXX][XXX]")
                    left_per = 0.5
                    out = False
                elif s1 < umbral and s2 < umbral:
                    #print("[XXX][XXX][   ][   ][   ][   ][   ][   ]")
                    right_per = 0.5
                    out = False
                elif s6 < umbral and s5 < umbral:
                    #print("[   ][XXX][XXX][   ][   ][   ][   ][   ]")
                    right_per = 0.7
                    out = False
                elif s4 < umbral and s5 < umbral:
                    #print("[   ][   ][XXX][XXX][   ][   ][   ][   ]")
                    right_per = 0.90
                    out = False
                elif s0 < umbral:
                    #print("[   ][   ][   ][   ][   ][   ][   ][XXX]")
                    left_per = 0.25
                    out = False
                elif s7 < umbral:
                    #print("[XXX][   ][   ][   ][   ][   ][   ][   ]")
                    right_per = 0.25
                    out = False
                else:
                    #print("[   ][   ][   ][   ][   ][   ][   ][   ]")
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
                #print("[   ][   ][   ][XXX][XXX][   ][   ][   ]")
                left_per = 1
                right_per = 1
                out = False
            self.robot.on(self.vl*left_per, self.vr*right_per)


class semaforoIrakurlea(Thread):
    def __init__(self, threadID, kode0, kode1, kode2):
        Thread.__init__(self)
        self.threadID = threadID
        self.kode = [kode0, kode1, kode2]
        self.running = True
        self.sensor = ColorSensor(INPUT_4)


    # COLOR_RED = 0
    # COLOR_GREEN = 1
    # COLOR_BLUE = 2

    def run(self):
        sem = [-1, -1, -1]
        kol_n = 0
        global sem_zuzena
        sem_zuzena = False
        while self.running:
            rgb = self.sensor.rgb
            koloremax = max(rgb)
            kolore = rgb.index(koloremax) if (koloremax > 150) else -1
            
            if (kolore == 1): # berdea
                kolore_urdin = rgb[2]
                if (abs(koloremax - kolore_urdin) < 50):
                    kolore = 2

            if (kolore != -1 and kol_n == 0):
                sem[kol_n] = kolore
                kol_n += 1
            elif (kolore != -1 and sem[kol_n-1] != kolore):
                sem[kol_n] = kolore
                kol_n += 1
                if (kol_n == 3):
                    kol_n = 0
                    sem_zuzena = True if (sem==self.kode) else False
                    print(sem)
                    sem = [-1,-1,-1]
            else:
                sem = [-1, -1, -1]
            sleep(0.01)
                
            
            


if __name__ == '__main__':
    t = lerroJarraitzailea(1, 25, 25)
    s = semaforoIrakurlea(2,-1,-1,-1)
    t.setDaemon = True
    s.setDaemon = True
    t.start()
    s.start()
    while True:
        sleep(0.01)
