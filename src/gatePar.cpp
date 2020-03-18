#include "gatePar.h"

/**
 * constructor reads in a banana gate
\param ifile is an ifstream object of an open file containing gate
*/
gatePar::gatePar(std::string name0)
{
  std::cout << "wtf" << std::endl;
  open(name0);
}
//*****************************************
/**
 * destructor
 */
gatePar::~gatePar()
{
}

bool gatePar::open(std::string name){

  std::ifstream gateFile(name.c_str());

  //std::cout << "wtf2" << std::endl;
  if(gateFile.is_open()){
    gateFile >> n;  // number of points
    //x = new float [n];
    //y = new float [n];
    double x0;
    double y0;
    //std::cout << "WTF3-" << x.size() << std::endl;
    for (int i=0;i<n;i++){
      gateFile >> x0 >> y0;
      //std::cout << x0 << "\t" << y0 << std::endl;
      x.push_back(x0);
      y.push_back(y0);
    }
    gateFile.close();
    return true;
  }
  else{
    std::cout << "Could not open file " << name << std::endl;
    return false;
  }


}
//*******************************************
/**
 * returns true if particle is in 2D Histogram gate
\param xx is energy of particle
\param yy is energy loss of particle
*/
bool gatePar::isInside(double xx, double yy)
{
  //stole this from ROOT's IsInside function

  if(good){
    double xint;
    int inter = 0;
    for(int i=0;i<(int)x.size()-1;i++){
      if(y[i] == y[i+1]) continue;
      if (yy <= y[i] && yy <= y[i+1]) continue;
      if (y[i] < yy && y[i+1] < yy) continue;
      xint = x[i] + (yy-y[i])*(x[i+1]-x[i])/(y[i+1]-y[i]);
      if (xx < xint) inter++;
    }

    if (inter%2) return true;
    else return false;

  }
  else{
    return true;
  }
}
