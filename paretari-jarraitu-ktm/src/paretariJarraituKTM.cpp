#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <libplayerc++/playerc++.h>
#include <iostream>
#include <gsl/gsl_fit.h>

/* PARAMETROAK ZEHAZTU */

#define LASER_IZPI_KOP 45

void
closeAll(int signum)
{
  std::cout << "Interrupt signal (" << signum << ") received\n";
  // datuFitx.close();
  exit(signum); 
}

int
main(int argc, const char **argv)
{
  int i, ind;
  float diff;
  float v,w;
  float dist;
  // string fileName = "outData.txt";
  float Kp;
  float angelua;

  signal(SIGINT, closeAll);
  if (argc < 2)
    {
      std::cout << "Robotak paretari jarraituko dio " << std::endl;
      std::cout << "angeluaren gaineko erregresio lineala erabiliz." << std::endl;
      std::cout << "Erabilera: " << argv[0] <<" <Kp> [<outFitx>]" << std::endl;
      return -1;
    }
  Kp = atof(argv[1]);
  if (argc == 3)
    // fileName = argv[2];
  // datuFitx.open(fileName, ios::out);

  try
  {
    double c0, c1, cov00, cov01, cov11, batura;
    double x[LASER_IZPI_KOP], y[LASER_IZPI_KOP];
    double theta;
    int j = 0;

    using namespace PlayerCc;

    // Sortu bezeroa eta konektatu zerbitzariarekin.
    PlayerClient bezeroa("localhost", 6665);
    // Sortu laser interfazea eta harpidetu bezeroarekin
    LaserProxy laserra(&bezeroa, 0);
    // Sortu position2d interfazea eta harpidetu bezeroarekin
    Position2dProxy robota(&bezeroa, 0);
    
    for (i = 0; i<10; i++)
    	bezeroa.Read();

    while (1)
      {
	diff = 0;
	bezeroa.Read();
	/* Kalkulatu irakurketa "motzen" proiekzioak */

	j = 0;
	for (i = 0; i < LASER_IZPI_KOP; i++)
	  {
	    /* Hemen laser izpiak proiektuatu behar dira */
	    x[i] = laserra.GetRange(i)*sin(laserra.GetBearing(i));
	    y[i] = laserra.GetRange(i)*cos(laserra.GetBearing(i));;
	    j++;
	  }

	/* regresio lineala: karratu txikienen metodoa */
	/* KONTUZ!! j indizeak regresioa kalkulatzeko erabiliko den */
	/* puntu kopurua adierazten du!!  */
	gsl_fit_linear (x, 1, y, 1, j, 
			&c0, &c1, &cov00, &cov01, &cov11, 
			&batura);

	/* Kalkulatu robota eta paretaren arteko angelua */
	angelua = M_PI/2 - abs(atan(c1));
	
	/* Abiadurak finkatu */
	w = angelua * Kp;
	// w = 0;
	
	robota.SetSpeed(0.2, w);
	
      }
    
  }
  catch (PlayerCc::PlayerError & e)
    {
      std::cerr << e << std::endl;
      return -1;
    }
  
  return 0;
}
