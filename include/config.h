#include "cpptoml.h"
#include <string>
#include "RBDDarray.h"
#include "TFile.h"
#include "TChain.h"

class config{
 public:

  void config(string configName, int runStart, int runEnd);
  TChain getDataChain(){return dataChainObject};

  RBDDarray getArray(string arrayName);
  RBDDdet getDet(string detName);

 private:


  inline bool exists_test(const std::string& name);
  //Will store objects in configuration file, as this will set all the relevant variables
  RBDDdet detectorObjects[1024]; //This should be enough objects for 4 DDAS crates (XIA Pixie-16 digitizer crates)
  RRBDDarray arrayObjects[10]; //Should never really have more than this...

  TChain dataChainObject;

};
