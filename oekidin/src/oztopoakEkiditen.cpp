#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <libplayerc++/playerc++.h>
#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <chrono>

std::ofstream datuFitx;
std::string fileName = "outData.txt";

void
closeAll(int signum)
{
  std::cout << "Interrupt signal (" << signum << ") received\n";
  //datuFitx.close();
  exit(signum);
}

float pathRecValue(int minIndex, float actualValue){
  float newValue = actualValue;
  if (minIndex > 89 && actualValue<=0){
    newValue = 0.5;
  }
  else if (minIndex < 90 && actualValue >= 0){
    newValue = -0.5;
  }
  return newValue;
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
    float alphaLS = 0.0;
    float alphaAS = 0.0;
    float lspeed = 0.5;
    float aspeed = 0.3;
    float norm_distance, reg_i, left_alpha, right_alpha;

    std::mt19937_64 rng;
    // Initialize the random number generator with time-dependent seed
    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
    rng.seed(ss);
    // Initialize a uniform distribution between 0 and 1
    std::uniform_real_distribution<double> unif(0, 1);
  
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
      }
      
      //float x = robota.GetXPos();
      //float y = robota.GetYPos();
      //datuFitx  << x << " " << y  << "\n";

      // Angle:
      // left: 179-90
      // right: 0-89

      alphaLS = 0.0;
      alphaAS = 0.0;

      right_alpha = 0.0;
      left_alpha = 0.0;

      for (i = 0; i < count/2; i++)
      {
          // 0->90 LESS-MOST priority
          // Positions: [0-1] 0: least important, 1 most important
          reg_i = i/90;
          // Smaller the distance, greater the numerator => greater alpha
          norm_distance = 1-laserra.GetRange(i)/8.1;
          // 
          right_alpha = right_alpha + (reg_i+norm_distance)/2;
      }
      alphaAS = right_alpha/(count/2);
      for (i = count/2+1; i < count; i++){
          // 90->179 MOST-LEAST priority
          // positions: [0-1] 0: least important, 1 most important  
          reg_i = (i - 90)/90;
          // Smaller the distance, greater the numerator => greater alpha
          norm_distance = 1-laserra.GetRange(i)/8.1;
          left_alpha = left_alpha + (reg_i+norm_distance)/2;
      }
      alphaAS = alphaAS - left_alpha/(count/2);
      std::cout << 'alphaAS: '<< alphaAS << '\n' << std::endl;
      //////////////////////////////////////////////////////
      ////////// Calculate alphaLS and alphaAS /////////////
      //////////////////////////////////////////////////////
      
      // alphaLS: alpha for linear speed
      // alphaAS: alpha for angular speed

      // if (laserra.GetRange(40) < laserra.GetRange(140)){
      //   alphaLS = 1.0;
      //   alphaAS = 0.3;
      // } else {
      //   alphaLS = 1.0;
      //   alphaAS = -0.3;
      // }



      //////////////////////////////////////////////////////
      ////// Apply low pass filter and set speeds //////////
      //////////////////////////////////////////////////////
      // Low pass filter: actual_value = (1-change_value)*previous_value + change_value*random(0,1)
      
      // Linear speed with low pass filter
      lspeed = (1-alphaLS)*lspeed + alphaLS*unif(rng);
      // Angular speed with low pass filter
      if (alphaAS < 0)
      {
        if (aspeed < 0){
          aspeed = (1+alphaAS)*aspeed + alphaAS*unif(rng);
        }
        else{
          aspeed = -(1+alphaAS)*aspeed + alphaAS*unif(rng);
        }
      } else{
        aspeed = (1-alphaAS)*aspeed + alphaAS*unif(rng);
      }
      // Set robot's speed
      robota.SetSpeed(lspeed, aspeed);
      
      //////////////////////////////////////////////////////
      //////////////// Print info //////////////////////////
      //////////////////////////////////////////////////////
      //std::cout << "Max Reading: " <<  max << " Index:" << maxind << std::endl;
      //std::cout << "Min Reading: " <<  min << " Index:" << minind << std::endl;
      //std::cout << "-----------------------------------" << std::endl;
      
      }
  }
  catch (PlayerCc::PlayerError & e)
    {
      std::cerr << e << std::endl;
      return -1;
    }
    
}




