
#ifndef ____ionCorrelator__
#define ____ionCorrelator__

//#include <cmath>
//#include <stdio.h>
#include <vector>
//
//#include <TBits.h>
//#include <TCanvas.h>
//#include <TFile.h>
//#include <TGraph.h>
#include <TH1D.h>
#include <TNamed.h>
#include <TObject.h>
#include "histo.h"
#include "RBDDarray.h"
#include "RBDDTrace.h"
//#include <TTree.h>
//#include <TList.h>


class ionCorrelator : public TNamed
{
protected:
  Event implantFront;
  Event implantBack;
  Event decayFront;
  Event decayBack;
  double corrWindow=0.;
  double stripTolerance = 3.;
  double Ethreshold = 100.;
  int frontImplantStrip = -100;
  int backImplantStrip = -100;
  int counter = 0.;
  bool firstEvent = true;

public:
  ionCorrelator(double corrWindow0, double Ethreshold0, double stripTolerance0,  Event implantFront0, Event implantBack0);
  void analyze(std::vector<Event> frontEvents, std::vector<Event> backEvents, std::vector<Event> segaEvents, histo *Histo);

  bool shouldDelete(){return abs(decayFront.time - implantFront.time) > corrWindow;}

  int getCounts(){return counter;}
  Event getFrontImplant(){return implantFront;}
  Event getBackImplant(){return implantBack;}

  int getImplantFrontStrip(){return frontImplantStrip;}
  int getImplantBackStrip(){return backImplantStrip;}

  bool implantOverlap(ionCorrelator secondImplant){
    if( abs(secondImplant.getImplantFrontStrip() - frontImplantStrip) < stripTolerance){
      
      if( abs(secondImplant.getImplantBackStrip() - backImplantStrip) < stripTolerance){
	return true;
      }

    }

    return false;
    ;}

  //ClassDef(RBDDTrace,1); //comment this if I want to compile normally, otherwise I have to do some root trickery in the compilation
};

#endif /* defined(____RBDDTrace__) */
