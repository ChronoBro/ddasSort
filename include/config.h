#include "cpptoml.h"
#include <string>
#include "RBDDarray.h"
#include "TFile.h"
#include "TChain.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

class config{
 public:

  config(std::string configName, int runStart, int runEnd, TChain &dataChainObject);
  //TChain* dataChainObject; //I really don't want this public but can't seem to figure out why it
  // doesn't like being private and have a function to grab it

  //TChain getDataChain(){return *dataChainObject;}

  RBDDarray getArray(std::string arrayName);
  RBDDdet getDet(std::string detName);

  double getCoinWindow(){return corrWindow;}
  double getCorrWindow(){return coinWindow;}
  double getStripTolerance(){return stripTolerance;}
 
 private:

  static const int maxChannels = 1024; //remember to sync this with RBDDTriggeredEvent...
  static const int maxArrays = 10;

  inline bool exists_test(const std::string& name);
  //Will store objects in configuration file, as this will set all the relevant variables
  RBDDdet detectorObjects[maxChannels]; //This should be enough objects for 4 DDAS crates (XIA Pixie-16 digitizer crates)
  RBDDarray arrayObjects[maxArrays]; //Should never really have more than this...

  double corrWindow = 0.;
  double coinWindow = 0.;
  double stripTolerance = 0.;

};
