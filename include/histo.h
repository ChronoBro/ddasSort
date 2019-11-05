#ifndef histo_
#define histo_
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include "TGraph.h"
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

 public:
  TDirectoryFile * dirHistos; //!< directory for the CsI info
  TDirectory * channels;
  TDirectory * dirTraces;


  histo();                  //!< constructor
  ~histo(){write();delete file;}
  histo(std::string name0);
  void write(); //!< write the root spectra to file
  void histos();

  //TGraph * traces[50]; //bad idea to hard code this... should come up with better method to fill traces

  std::vector<TGraph*> graphTraces;
  std::vector<TH1D*> traceHistos;

  TH1I * energyCheck;
  TH2I * h_raw_summary;
  TH2I * h_PromptGamma_summary;
  TH1I  *h_raw_DSSD[80];
  TH1I  *h_raw_DSSD_M1[80];
  TH1I  *h_mult_F_implant;
  TH1I  *h_mult_B_implant;
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
  TH2I  *h_PID_gated;
  TH2I  *h_PIN1vsPIN2;
  TH2I  *hImplantXY;
  TH2I  *hDecayXY;
  TH1D  *hDecayTime;
  TH1D  *hDecayTimeGS;
  TH1D  *hDecayTimeEx;
  TH1D  *hDecayTimeLog;
  TH2D  *hDecayTimeLogVsDecayEtot;
  TH1D  *hDecayTimeEgate;
  TH1D  *hDecayEnergyTGate;
  TH1D  *hDecayEnergyTot_TGate;
  TH1D  *hDecayEnergyTot_firstEvent;
  TH1D  *hDecayEnergy;
  TH1D  *hDecayEnergyBackground;
  TH1D  *hDecayEnergyTotBackground;
  TH1D  *hDecayEnergyTot;
  TH1D  *hDecayEnergyAll;
  TH1D  *hDecayEnergyAmp;
  TH1D  *hGammaEnergy;
  TH2D  *hGammaVsDecayTGated;
  TH1D  *hPromptGammaEnergy;
  TH2D  *hGammaVsDecay;
  TH2D  *hGammaVsDecayEtot;
  TH2D  *hGammaVsDecayAll;
  TH2D  *gg_prompt;
  TH1D  *hGammaEnergyG;
  TH2D  *hSeGAEnergy;
  TH1I  *rawSSDhiGain;
  TH1D  *calibratedSSDhiGain;
  TH1I  *rawScint;

  std::string name;
  
};
#endif
