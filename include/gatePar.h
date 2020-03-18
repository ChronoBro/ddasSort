#ifndef _gatePar
#define _gatePar
#include "TMath.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

/**
 *!\briefly store (in memory) a 2D Histogram gate
 */

class gatePar
{
 public:

  gatePar(std::string name0);
  gatePar(){};
  ~gatePar();
  bool open(std::string name);
  bool isInside(double xx, double yy);
  bool good = true;

  int n; //!<number of points
  //float *x; //!<pointer to x array
  //float *y; //!<pointer to y array
  std::vector<double> x;
  std::vector<double> y;
};
#endif
