#include "ionCorrelator.h"

ionCorrelator::ionCorrelator(double corrWindow0, double Ethreshold0, double stripTolerance0,  Event implantFront0, Event implantBack0, histo *Histo0){
  implantFront = implantFront0;
  implantBack = implantBack0;
  corrWindow = corrWindow0;
  Histo = Histo0;
  Ethreshold = Ethreshold0;
  stripTolerance = stripTolerance0;
  frontImplantStrip = 40 - (implantFront.channel - 103);
  backImplantStrip = 40 - (implantBack.channel - 183);

  //traceFilter = new gatePar("Gates/realTrace.cut");

  //traceFilter.open("Gates/realTrace.cut");
  traceFilter.open("Gates/testTrace.cut");

  //the below two lines are causing massive memory leak that is grinding processes to a halt...
  //need to figure out a solution for adding contours
  // filterFile = new TFile("root-files/filter.root");
  // filter = new TCutG(*(TCutG*)filterFile->FindObjectAny("CUTG"));

  // std::cout << std::endl;
  // std::cout << "frontImplantStrip : " << frontImplantStrip << std::endl;
  // std::cout << "backImplantStrip : " << backImplantStrip << std::endl;
  // std::cout << std::endl;
}

bool ionCorrelator::analyze(std::vector<Event> frontEvents, std::vector<Event> backEvents, std::vector<Event> segaEvents){

  decayFront = frontEvents.front(); //just to make sure there is some data in there
  decayBack = backEvents.back(); //just to make sure there is some data in there so can check things

  bool foundDecay = false;

  double implantTime = implantFront.time;

  //std::cout << "I'm in ionCorrelator::analyze()" << std::endl;

  double frontAddBackE = 0.;

  for(auto& frontEvent : frontEvents){
    double frontStrip = frontEvent.channel - 64.;

    //if(frontEvent.energy <1500){ //apparently I needed this low energy gate for the filter to only work on things that were actually BAD 9/22/2019
      RBDDTrace test2(frontEvent.trace);

      Histo->trace_vs_signal->Fill(test2.GetQDC(), frontEvent.signal);

      if( !traceFilter.isInside(test2.GetQDC(), frontEvent.signal))
      	{
      	  break;
      	}

      // if(!test2.filter() && frontEvent.energy > 1200){ // filter() returns true if passes filter
      // break;
      // }

      //}

    for(auto& backEvent : backEvents){
      double backStrip = backEvent.channel -144.;

      if( abs(frontStrip - frontImplantStrip) < stripTolerance 
	  && abs(backStrip - backImplantStrip) < stripTolerance //I till think something is screwy with the backs...
	  && frontEvent.energy > Ethreshold
	  && backEvent.energy > Ethreshold
	  ){

	Histo->hDecayTime->Fill(frontEvent.time-implantTime);
	if(frontEvent.energy > 3000 && frontEvent.energy < 3800){
	  Histo->hDecayTimeEx->Fill(frontEvent.time-implantTime);
	}
	if(frontEvent.energy > 3800 && frontEvent.energy < 4600){
	  Histo->hDecayTimeGS->Fill(frontEvent.time-implantTime);
	}


	Histo->hDecayEnergy->Fill(frontEvent.energy);
	//Histo->hDecayEnergyTot->Fill(frontDecayAddBack.energy);
	//counterList.count("decays");
	counter++;
    
	double decayTime = frontEvent.time-implantTime;
	double TCutoff = 2E8; //200ms

	if(decayTime < TCutoff){
	  //Histo->hDecayEnergyTot_TGate->Fill(frontDecayAddBack.energy);
	  Histo->hDecayEnergyTGate->Fill(frontEvent.energy);
	  for(auto & segaEvent : segaEvents){
	    //Histo->hGammaEnergy->Fill(segaEvent.energy);
	    Histo->hGammaVsDecayTGated->Fill(frontEvent.energy,segaEvent.energy);
	    //if(frontDecayAddBack.energy > 3000 && frontDecayAddBack.energy < 3800){
	    Histo->hGammaEnergyG->Fill(segaEvent.energy);
	    //}
	  }

	}

	for(auto & segaEvent : segaEvents){
	  Histo->hGammaEnergy->Fill(segaEvent.energy);
	  Histo->hGammaVsDecay->Fill(frontEvent.energy,segaEvent.energy);
	}


	    
	if(decayTime > 1E9){
	  //Histo->hDecayEnergyTotBackground->Fill(frontDecayAddBack.energy);
	  Histo->hDecayEnergyBackground->Fill(frontEvent.energy);
	}

	if(firstEvent){
	  //Histo->hDecayEnergyTot_firstEvent->Fill(frontDecayAddBack.energy);
	  Histo->hDecayTimeLog->Fill(frontEvent.time-implantTime);
	  firstEvent = false;
	}

	//only should really leave below on for beta-delayed proton emitters with no beta-gamma branch
	//and for a small number of counts
	if(frontEvent.energy < 1400 && nTraces < maxNTraces && decayTime < TCutoff){ 
	  std::ostringstream nameMe;
	  nameMe << "Energy-" << frontEvent.energy << "_fStrip-" << frontStrip << "_bStrip-" << backStrip<<"_base-" << test2.GetBaseline();
	  //TH1D * trace= new TH1D; //need to create new block of memory for trace to be saved
	  test2.SetMSPS(100.); //so that time is correct on traces
	  //trace = (TH1D*)test2.GetTraceHisto()->Clone(nameMe.str().c_str());
	  //Histo->traceHistos.push_back(trace);
	  //test2.GetTraceHisto()->SetName(nameMe.str().c_str());
	  Histo->traceHistos.push_back((TH1D*)test2.GetTraceHisto()->Clone(nameMe.str().c_str())); //need to create an object for it to point to, so at least need a clone
	  //Histo->traceHistos.push_back(test2.GetTraceHisto());
	  //delete trace;
	  nTraces++;
	}
	    
	decayFront = frontEvent;
	decayBack = backEvent;
	foundDecay = true;

	//Histo->trace_vs_signal->Fill(test2.GetQDC(),frontEvent.signal);

	return foundDecay;
	//break;

      }//end valid decay restriction

    } //end of loop on backs

  }//end of loop on fronts

      
  return foundDecay;

}

