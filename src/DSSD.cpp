#include "DSSD.h"



DSSD::DSSD(){ //default constructor
  nStripsFront = 1;
  nStripsBack = 1;
}

DSSD::DSSD(int nStripsFront0, int nStripsBack0){ //overloaded constructor
  nStripsFront = nStripsFront0;
  nStripsBack = nStripsBack0;

} 

DSSD::~DSSD(){ //destructor
}
