#ifndef histo_
#define histo_
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "TH1I.h"
#include "TH2I.h"
#include "TH3I.h"
#include "TFile.h"
#include "TDirectory.h"
#include <sys/stat.h>
#include "math.h"

//using namespace std;

class histo
{
 protected:

  TFile * file; //!< output root file


  //CsI
  TDirectoryFile * dirHistos; //!< directory for the CsI info
  TDirectory * channels;


 public:
  histo();                  //!< constructor
  ~histo(){write();delete file;}
  histo(std::string name0);
  void write(); //!< write the root spectra to file
  void histos();
  
  TH1I * energyCheck;
  TH2I * h_raw_summary;
  TH1I  *h_raw_DSSD[80];
  TH1I  *h_raw_DSSD_M1[80];
  TH1I  *h_mult;
  TH1I  *h_mult_F;
  TH1I  *h_mult_B;
  TH2I  *h_raw_DSSD_hits;
  TH2I  *h_raw_DSSD_hitsMap;
  TH2I  *h_raw_DSSD_hitsXY;

// Energy calculated from trace amplitude
//  TH2I  *h_raw_summary_A;
//  TH1I  *h_raw_DSSD_A[80];
//  TH1I  *h_mult_A;
//  TH1I  *h_mult_F_A;
//  TH1I  *h_mult_B_A;
//  TH2I  *h_DSSD_hits_A;

  TH2D  *h_cal_summary;
  TH1D  *h_cal_DSSD[80];
  TH2D  *h_EFEB;
  TH2I  *h_cal_DSSD_hits;

  TH2I  *h_PID;
  TH2I  *hImplantXY;
  TH2I  *hDecayXY;
  TH1D  *hDecayTime;
  TH1D  *hDecayEnergyTGate;
  TH1D  *hDecayEnergy;
  TH1D  *hDecayEnergyAll;
  TH1D  *hDecayEnergyAmp;
  TH1D  *hGammaEnergy;
  TH1D  *hPromptGammaEnergy;
  TH2D  *hGammaVsDecay;
  TH2D  *hGammaVsDecayAll;
  TH1D  *hGammaEnergyG;
  TH2D  *hSeGAEnergy;
  TH1I  *rawSSDhiGain;
  TH1I  *rawScint;

  std::string name;
  
};
#endif
