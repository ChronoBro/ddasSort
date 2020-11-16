#include "ionCorrelator.h"

ionCorrelator::ionCorrelator(double corrWindow0, double TGate0, double Ethreshold0, double stripTolerance0,  Event implantFront0, Event implantBack0, histo *Histo0){
  implantFront = implantFront0;
  implantBack = implantBack0;
  corrWindow = corrWindow0;
  Histo = Histo0;
  Ethreshold = Ethreshold0;
  stripTolerance = stripTolerance0;
  frontImplantStrip = 40 - (implantFront.channel - 103);
  backImplantStrip = 40 - (implantBack.channel - 183);
  TCutoff = TGate0;

  //traceFilter = new gatePar("Gates/realTrace.cut");

  //traceFilter.open("Gates/realTrace.cut");
  //traceFilter.open("Gates/testTrace.cut");
  traceFilter.open("Gates/testTrace3.cut");
  gammaTimeFilter.open("Gates/gammaTimeCut.cut");

  segaDelay[0] = 0.;
  segaDelay[1] = 0.;
  segaDelay[2] = 0.;
  segaDelay[3] = 0.;
  segaDelay[4] = 0.;
  segaDelay[5] = 0.;
  segaDelay[6] = 0.;
  segaDelay[7] = 0.;
  segaDelay[8] = 0.;
  segaDelay[9] = 0.;
  segaDelay[10] = 0.;
  segaDelay[11] = 0.;
  segaDelay[12] = 0.;
  segaDelay[13] = 0.;
  segaDelay[14] = 0.;
  segaDelay[15] = 0.;


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
      test2.SetMSPS(100.); //so that time is correct on traces
      Histo->trace_vs_signal->Fill(test2.GetQDC(), frontEvent.signal);

      // std::ostringstream nameMe;
      // nameMe << "";
      // if(test2.GetQDC() < 0 && frontEvent.signal > 20000. && frontEvent.signal < 25000){ 
      // 	nameMe << "R1_trace-Energy_" << frontEvent.energy;
      // }
      // else if(test2.GetQDC() < 0 && frontEvent.signal > 27000.){
      // 	nameMe << "R2_trace-Energy_" << frontEvent.energy;
      // }
      // else if(frontEvent.signal > 10000 && frontEvent.signal > 15000 && test2.GetQDC() > 300000. && test2.GetQDC() < 400000.){
      // 	nameMe << "R3_trace-Energy_" << frontEvent.energy;
      // 	// for(auto & gamma : segaEvents){
      // 	//        Histo->hGammaEnergy_R3events->Fill(gamma.energy);
      // 	// }
    
      // }
	
      // if(nTraces < maxNTraces && !nameMe.str().empty()){
      // 	Histo->traceHistos.push_back((TH1D*)test2.GetTraceHisto()->Clone(nameMe.str().c_str())); //need to create an object for it to point to, so at least need a clone
      // 	//Histo->traceHistos.push_back(test2.GetTraceHisto());
      // 	//delete trace;
      // 	nTraces++;
      // }

      /********************************
      //remember to reinitiate this Dan!
      *********************************/
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


	double decayTime = frontEvent.time-implantTime;	

	// if(decayTime <= 0){
	//   std::cout << "difference = " << decayTime << std::endl;
	//   std::cout << "implantTime = " << implantTime << std::endl;
	//   std::cout << "decayTime = " << frontEvent.time << std::endl;
	// }

	Histo->hDecayTime->Fill(frontEvent.time-implantTime);
	Histo->hDecayTimeLogAll->Fill(frontEvent.time-implantTime);
	if(frontEvent.energy > 3000 && frontEvent.energy < 3800){
	  Histo->hDecayTimeEx->Fill(frontEvent.time-implantTime);
	}
	if(frontEvent.energy > 3800 && frontEvent.energy < 4600){
	  Histo->hDecayTimeGS->Fill(frontEvent.time-implantTime);
	}

	//7/13/2020, there are a large number of "underflow" time events (<0) that are polluting the spectra
	//My first step is to see the effect, then I will try to diagnose where they are coming from. 
	if(decayTime>0)Histo->hDecayEnergy->Fill(frontEvent.energy);
	//Histo->hDecayEnergyTot->Fill(frontDecayAddBack.energy);
	//counterList.count("decays");
	counter++;
    
	//double TCutoff = 20E9;//1E9;//20E9; //20s

	if(decayTime < TCutoff && decayTime>0){
	  //Histo->hDecayEnergyTot_TGate->Fill(frontDecayAddBack.energy);
	  Histo->hDecayEnergyTGate->Fill(frontEvent.energy);
	  for(auto & segaEvent : segaEvents){
	    Histo->hGammaTvsDecayT->Fill((segaEvent.time-frontEvent.time),segaEvent.energy);
	    if( !gammaTimeFilter.isInside((segaEvent.time-frontEvent.time),segaEvent.energy) ) continue; //only consider gamma-rays within filter
	    //Histo->hGammaEnergy->Fill(segaEvent.energy);
	    Histo->hGammaVsDecayTGated->Fill(frontEvent.energy,segaEvent.energy);
	    //if(frontDecayAddBack.energy > 3000 && frontDecayAddBack.energy < 3800){
	    Histo->hGammaEnergyG->Fill(segaEvent.energy);
	    Histo->hGammaTvsDet->Fill(segaEvent.time-frontEvent.time,segaEvent.channel-16.);//to get offset to 0
	    //}
	    for(auto & segaEvent2 : segaEvents){
		if(segaEvent.energy == segaEvent2.energy)continue;
		if( abs(segaEvent.time - segaEvent2.time) > 200 ) continue;
		Histo->hGammaEvsGammaE->Fill(segaEvent.energy, segaEvent2.energy);
		//Histo->hGammaEvsGammaE->Fill(segaEvent2.energy, segaEvent.energy);
	    }
	  }
	}

	if(decayTime > 600 && decayTime < 6000){
	  Histo->hDecayEnergyTGateStrict->Fill(frontEvent.energy);
	}

	for(auto & segaEvent : segaEvents){
	  Histo->hGammaEnergy->Fill(segaEvent.energy);
	  Histo->hGammaVsDecay->Fill(frontEvent.energy,segaEvent.energy);
	}

	if(decayTime < 0){
	  Histo->hDecayEnergyBackgroundUnderflow->Fill(frontEvent.energy);
	}

	    
	if(decayTime > TCutoff){
	  //Histo->hDecayEnergyTotBackground->Fill(frontDecayAddBack.energy);
	  Histo->hDecayEnergyBackground->Fill(frontEvent.energy);
	  Histo->hDecayEnergyBackgroundScaled->Fill(frontEvent.energy);
	  for(auto & segaEvent : segaEvents){
	    Histo->hGammaVsDecayBackground->Fill(frontEvent.energy, segaEvent.energy);
	    if( !gammaTimeFilter.isInside((segaEvent.time-frontEvent.time),segaEvent.energy) ) continue; //only consider gamma-rays within filter
	    for(auto & segaEvent2 : segaEvents){
		if(segaEvent.energy == segaEvent2.energy)continue;
		if( abs(segaEvent.time - segaEvent2.time) > 200 ) continue;
		Histo->hGammaEvsGammaE_back->Fill(segaEvent.energy, segaEvent2.energy);
	    }
	  }
	}

	if(secondEvent){
	  second = frontEvent;
	  if(decayTime < TCutoff && decayTime > 0){ 
	    Histo->hDecayEnergy_secondEventTGate->Fill(frontEvent.energy);
	  }
	  Histo->hDecayTime_first_second->Fill(second.time-first.time);
	  Histo->hDecayTime2LogVsDecayE2->Fill(frontEvent.energy,frontEvent.time-implantTime);
	  // std::cout << std::endl << "First Event Time = " << first.time << std::endl;
	  // std::cout << std::endl << "Second Event Time = " << second.time << std::endl;
	  // std::cout << std::endl << "Time difference between second and first is: " << second.time - first.time << std::endl;
	}

	if(firstEvent){
	  Histo->hDecayEnergy_firstEvent->Fill(frontEvent.energy);
	  if(decayTime < TCutoff && decayTime > 0) Histo->hDecayEnergy_firstEventTGate->Fill(frontEvent.energy);
	  if(decayTime > 1E9) Histo->hDecayEnergy_firstEventBackground->Fill(frontEvent.energy);
	  Histo->hDecayTimeLog->Fill(frontEvent.time-implantTime);
	  Histo->hDecayTime1LogVsDecayE1->Fill(frontEvent.energy,frontEvent.time-implantTime);
	  firstEvent = false;
	  secondEvent = true;
	  first = frontEvent;
	}

	//only should really leave below on for beta-delayed proton emitters with no beta-gamma branch
	//and for a small number of counts
	// if(frontEvent.energy > 2900 && frontEvent.energy <3100 && nTraces < maxNTraces && decayTime < TCutoff){ 
	//   //if(decayTime < 3E3 && nTraces < maxNTraces){
	//   //std::cout << "found a low decayTime event... must be a buffer issue" << std::endl;
	//   std::ostringstream nameMe;
	//   nameMe << "Energy-" << frontEvent.energy << "_fStrip-" << frontStrip << "_bStrip-" << backStrip<<"_base-" << test2.GetBaseline();
	//   //TH1D * trace= new TH1D; //need to create new block of memory for trace to be saved
	//   test2.SetMSPS(100.); //so that time is correct on traces
	//   //trace = (TH1D*)test2.GetTraceHisto()->Clone(nameMe.str().c_str());
	//   //Histo->traceHistos.push_back(trace);
	//   //test2.GetTraceHisto()->SetName(nameMe.str().c_str());
	//   Histo->traceHistos.push_back((TH1D*)test2.GetTraceHisto()->Clone(nameMe.str().c_str())); //need to create an object for it to point to, so at least need a clone
	//   //Histo->traceHistos.push_back(test2.GetTraceHisto());
	//   //delete trace;
	//   nTraces++;
	// }
	    
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

