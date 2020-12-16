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
            if (sem_flag == 1):
                self.robot.on(self.vl*left_per*0.5, self.vr*right_per*1.2)
            elif (sem_flag == 0):
                self.robot.on(self.vl*left_per*1.1, self.vr*right_per*0.9)
            else:
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
        global sem_flag
        sem_flag = 2
        while self.running:
            rgb = self.sensor.rgb
            kolore_lur = self.sensor.color
            
            koloremax = max(rgb)
            kolore = rgb.index(koloremax) if (kolore_lur == 5 or kolore_lur == 2 or kolore_lur == 3) else -1

            # Berdearen eta urdinaren diskriminazioa

            if (kolore == 1 and (abs(koloremax - rgb[2]) < 50)): # berdea
                kolore = 2
            
            if (kolore != -1 and kol_n == 0):
                sem[kol_n] = kolore
                kol_n += 1
            elif (kolore != -1 and sem[kol_n-1] != kolore):
                sem[kol_n] = kolore
                kol_n += 1
                if (kol_n == 3):
                    kol_n = 0
                    sem_flag = 1 if (sem==self.kode) else 0
                    print(sem)
                    print("#############")
                    sem = [-1,-1,-1]
                    sleep(6)
                    sem_flag = 2
            # elif (kolore == -1):
            #    sem = [-1,-1,-1]
            #    kol_n = 0
            sleep(0.01)
                
            
            


if __name__ == '__main__':
    t = lerroJarraitzailea(1, 25, 25)
    s = semaforoIrakurlea(2, 2, 0, 1)
    t.setDaemon = True
    s.setDaemon = True
    t.start()
    s.start()
    while True:
        sleep(0.01)
