#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <libplayerc++/playerc++.h>
#include <iostream>
#include <fstream>
#include <string>

std::ofstream datuFitx;
std::string fileName = "outData.txt";

void
closeAll(int signum)
{
  std::cout << "Interrupt signal (" << signum << ") received\n";
  //datuFitx.close();
  exit(signum);
}

int
main(int argc, const char **argv)
{
  int i;  
  signal(SIGINT, closeAll);

  try
  {
    using namespace PlayerCc;

    // Sortu bezeroa eta konektatu zerbitzariarekin.
    PlayerClient bezeroa("localhost", 6665);
    // Sortu laser interfazea eta harpidetu bezeroarekin
    LaserProxy laserra(&bezeroa, 0);
    // Sortu position2d interfazea eta harpidetu bezeroarekin
    Position2dProxy robota(&bezeroa, 0);
    
    std::cout << bezeroa << std::endl;
    
    robota.SetMotorEnable (true);

    for (i = 0; i < 10; i++)
      bezeroa.Read();

    // Fitxategia prestatu 
    // datuFitx.open(fileName, ios::out);
    // datuFitx  << "# Robotaren (rx ry) koordenatuak \n";

    int count = laserra.GetCount();
    
    std::cout << "Scan count: "  <<  count << std::endl;    
    std::cout << "Scan resolution: "  <<  laserra.GetScanRes() << std::endl;
    std::cout << "Range resolution: " <<  laserra.GetRangeRes() << std::endl;
    std::cout << "Max Range: " <<  laserra.GetMaxRange() << std::endl;
    std::cout << "Max angle: " <<  laserra.GetMaxAngle() << std::endl;
    std::cout << "Min angle: " <<  laserra.GetMinAngle() << std::endl;


    float max = -1.0;
    float min = laserra.GetMaxRange();
    int maxind = -1;
    int minind = -1;
    
    while (1)
      { 
      bezeroa.Read();
      
      /* Laserraren irakurketak kudeatu */
      for (i = 0; i < count; i++)
      {
        if (laserra.GetRange(i) > max && laserra.GetRange(i) != laserra.GetMaxRange())
          {
          max = laserra.GetRange(i);
          maxind = i;
          }
          if (laserra.GetRange(i) < min)
          {
          min = laserra.GetRange(i);
          minind = i;
          }
      }	/* Robotaren abiadurak finkatu */
      float x = robota.GetXPos();
      float y = robota.GetYPos();
      //datuFitx  << x << " " << y  << "\n";


      // Angle:
      // left: 179-90
      // right: 0-89

      robota.SetSpeed(0.2, 0);
      if (minind < 90){
        robota.SetSpeed(0.2,0.3);
      }
      else{
        robota.SetSpeed(0.2,-0.3);

      }

      // Print info
      
      std::cout << "Max Reading: " <<  max << " Index:" << maxind << std::endl;
      std::cout << "Min Reading: " <<  min << " Index:" << minind << std::endl;
      std::cout << "-----------------------------------" << std::endl;
      
      }
  }
  catch (PlayerCc::PlayerError & e)
    {
      std::cerr << e << std::endl;
      return -1;
    }
    
}




