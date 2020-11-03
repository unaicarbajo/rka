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
std::string fileName = "out.csv";

void
closeAll(int signum)
{
  std::cout << "Interrupt signal (" << signum << ") received\n";
  datuFitx.close();
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
    datuFitx.open(fileName, std::ios::out);

    int count = laserra.GetCount();
    
    std::cout << "Scan count: "  <<  count << std::endl;    
    std::cout << "Scan resolution: "  <<  laserra.GetScanRes() << std::endl;
    std::cout << "Range resolution: " <<  laserra.GetRangeRes() << std::endl;
    std::cout << "Max Range: " <<  laserra.GetMaxRange() << std::endl;
    std::cout << "Max angle: " <<  laserra.GetMaxAngle() << std::endl;
    std::cout << "Min angle: " <<  laserra.GetMinAngle() << std::endl;

    float alphaLS = 0.0;
    float alphaAS = 0.0;
    float lspeed = 0.5;
    float aspeed = 0.3;
    float norm_distance, reg_i, left_alpha, right_alpha, scaled_lspeed, scaled_aspeed, y, x;

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
      x = robota.GetXPos();
	    y = robota.GetYPos();
      datuFitx  << x << ";" << y  << "\n";

      //////////////////////////////////////////////////////
      ////////// Calculate alphaLS and alphaAS /////////////
      //////////////////////////////////////////////////////

      alphaLS = 0.0;
      alphaAS = 0.0;

      right_alpha = 0.0;
      left_alpha = 0.0;

      // Right side alpha
      for (i = 0; i < count/2; i++)
      {
          // 0->90 LESS-MOST priority
          // Positions: [0-1] 0: least important, 1 most important
          reg_i = i/90;
          // Smaller the distance, greater the numerator => greater alpha
          norm_distance = 1-laserra.GetRange(i)/8;
          right_alpha = right_alpha + (reg_i+norm_distance)/2;
      }
      right_alpha = right_alpha/(count/2);
      
      // Left side alpha
      for (i = count/2+1; i < count; i++)
      {
          // 90->179 MOST-LEAST priority
          // positions: [0-1] 0: least important, 1 most important  
          reg_i = (180-i)/90; //(i - 90)/90;
          // Smaller the distance, greater the numerator => greater alpha
          norm_distance = 1-laserra.GetRange(i)/8;
          left_alpha = left_alpha + (reg_i+norm_distance)/2;
      }
      left_alpha = left_alpha/(count/2);
    
      alphaAS = right_alpha - left_alpha;
      (left_alpha < right_alpha) ? alphaLS=right_alpha : alphaLS=left_alpha;
      
      // PRINT
      //std::cout << "Right_alpha: " << right_alpha << "\t" << "Left_alpha: " << -left_alpha << "\t" << "AlphaAS: " << alphaAS << std::endl;
      //std::cout << "% RIGHT: " << (right_alpha/(right_alpha + left_alpha))*100 << "%\t" << "% LEFT: " << left_alpha/(right_alpha + left_alpha)*100 << "%\n" << std::endl;

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
          if (aspeed < 0){
            aspeed = -(1-alphaAS)*aspeed + alphaAS*unif(rng);
          }
          else{
            aspeed = (1-alphaAS)*aspeed + alphaAS*unif(rng);
          }
      }

      /////////////////////////////////////////////////////////
      ///////////////////// SCALING ///////////////////////////
      /////////////////////////////////////////////////////////

      // scaled_lspeed [0, 0.3]
      scaled_lspeed = lspeed * 0.3;
      // scaled_aspeed [-0.4,0.4]
      scaled_aspeed = aspeed * 0.4;


      /////////////////////////////////////////////////////////
      ///////////////////// SET SPEED /////////////////////////
      /////////////////////////////////////////////////////////

      // FULL SPEED
      std::cout << "LSPEED: " << lspeed << "\t" << "ASPEED" << aspeed << std::endl;
      robota.SetSpeed(lspeed, aspeed);

      // SCALED SPEED
      //std::cout << "LSPEED [scaled]: " << scaled_lspeed << "\t" << "ASPEED [scaled]" << scaled_aspeed << std::endl;
      //robota.SetSpeed(scaled_lspeed, scaled_aspeed);
      
      }
  }
  catch (PlayerCc::PlayerError & e)
    {
      std::cerr << e << std::endl;
      return -1;
    }
    
}




