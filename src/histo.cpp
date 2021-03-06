
#include "math.h"
#include "histo.h"


histo::histo(){
  name = "root-files/sort.root";
  //default constructor
  file = new TFile(name.c_str(),"RECREATE"); 
  histos();
}

//overloaded constructor
histo::histo(std::string name0)
{
  std::ostringstream outFilename;
  name = name0;
  outFilename << "root-files";

  if(mkdir(outFilename.str().c_str(),0777))
    {
      if(errno == 17)
	{
	  //printf("%s\n\n", "Directory 'root-files' already made");
	}
      else
	{
	  printf("%s %d\n", "ERROR creating new 'root-files' directory:", errno);
	}
    }

  outFilename << "/" << name;

  //create root file for storing stuff
  //file = new TFile ("root-files/sort.root","RECREATE");
  file = new TFile (outFilename.str().c_str(),"RECREATE");
  histos();
 
}
//*********************************************

void histo::histos(){
  file->cd();

  //define histograms here

  dirHistos = new TDirectoryFile("Histos","Histos");
  dirHistos->cd();
  channels = dirHistos->mkdir("channels","channels");
  dirTraces = dirHistos->mkdir("traces","traces");

  //dirCsIRaw = dirCsI->mkdir("CsIRaw","raw");
  
  energyCheck = new TH1I("energyCheck","energyCheck",30000,0,30000);
  
  h_raw_summary           = new TH2I("h_raw_summary","h_raw_summary",250,0,250,(int)pow(2,12),0,(int)pow(2,15));
  h_PromptGamma_summary   = new TH2I("h_PromptGamma_summary","h_PromptGamma_summary",16,15.5,31.5,10000,0,10000);

  
  // channels->cd();
  // for(Int_t i=0; i<80; i++){
  //   char name1[500], title[500];
  //   sprintf(name1,"h_raw_DSSD_%02i",i);
  //   sprintf(title,"RAW DSSD energy %02i",i);
  //   h_raw_DSSD[i] = new TH1I(name1,title,(Int_t)pow(2,15),0,pow(2,15));
  //   sprintf(name1,"h_raw_DSSD_M1_%02i",i);
  //   sprintf(title,"RAW DSSD energy with one front and one back strip %02i",i);
  //   h_raw_DSSD_M1[i] = new TH1I(name1,title,(Int_t)pow(2,15),0,pow(2,15));
  // }
  
  
  dirHistos->cd();
  h_mult   = new TH1I("h_mult",  "DSSD Multiplicity",      40,0,40);
  h_mult_F = new TH1I("h_mult_F","DSSD Front Multiplicity",40,0,40);
  h_mult_B = new TH1I("h_mult_B","DSSD Back Multiplicity", 40,0,40);
  h_mult_F_implant = new TH1I("h_mult_F_implant","DSSD front implant multiplicity",100,-0.5,99.5);
  h_mult_B_implant = new TH1I("h_mult_B_implant","DSSD back implant multiplicity",100,-0.5,99.5);
  h_raw_DSSD_hits    = new TH2I("h_raw_DSSD_hits",   "DSSD hit pattern (strips)",       40,0,40,40,0,40);
  h_raw_DSSD_hitsMap = new TH2I("h_raw_DSSD_hitsMap","DSSD hit pattern (strips mapped)",40,0,40,40,0,40);
  h_raw_DSSD_hitsXY  = new TH2I("h_raw_DSSD_hitsXY", "DSSD hit pattern (xy position)",  40,-20,20,40,-20,20);
  h_raw_DSSD_hitsXY->GetXaxis()->SetTitle("x (mm)");
  h_raw_DSSD_hitsXY->GetYaxis()->SetTitle("y (mm)");
  hTriggerTest = new TH1D("h_triggerTest", "triggerTest",2000,-10000,10000);
  
  h_PID        = new TH2I("h_PID","PID PIN1 Energy vs PIN2-XFP TAC", 1250,0,25000,(int)pow(2,10),0,pow(2,15));
  h_PID_gated  = new TH2I("h_PID_gated","PID PIN1 Energy vs PIN2-XFP TAC", 1250,0,25000,(int)pow(2,10),0,pow(2,15));
  h_PIN1vsPIN2 = new TH2I("h_PIN1vsPIN2","PID PIN1 Energy vs PIN2 Energy", (int)pow(2,10),0,(int)pow(2,15),(int)pow(2,10),0,pow(2,15));
  hDecayTime   = new TH1D("hDecayTime","Implant-decay correlation time",10000,0,10E9); // ms
  hDecayTime_first_second   = new TH1D("hDecayTime_first_second","Implant-decay correlation time",10000,0,10E9); // ms
  hDecayTimeGS   = new TH1D("hDecayTimeGS","Implant-decay correlation time",2000,0,2E9); // ms
  hDecayTimeEx   = new TH1D("hDecayTimeEx","Implant-decay correlation time",2000,0,2E9); // ms

  trace_vs_signal = new TH2D("trace_vs_signal","trace_vs_signal",2E4,-1E6,1E6,1000,-1E4,1E5);

  int Nbins = 500;
  double maxT = 100E9; //100 seconds
  double minT = 4;//ns, smallest adc channel
  double dx = log(maxT/minT)/double(Nbins);
  Double_t edges[Nbins+1];
  edges[0] = 0;
  //edges[Nbins+1] = maxT;

  //double dx = maxT/((double)Nbins+1.);

  for(int i=0;i<Nbins+1;i++){
    //edges[i] = log(((double)i+1.)*maxT/((double)Nbins+1.));
    //std::cout << edges[Nbins+2-i] << std::endl;
    //edges[Nbins+1-i] = edges[Nbins+2-i]/pow(minT,i);
    edges[i+1] = exp(log(minT)+(double)i*dx);
    //std::cout << edges[i+1] << std::endl;
    //std::cout << (edges[i+1] - edges[i])/edges[i+1] << std::endl;
  }
  //edges[0]=0;

  hDecayTimeLog   = new TH1D("hDecayTimeLog","Implant-decay correlation time log bins",Nbins,edges); // ms
  hDecayTimeLogAll   = new TH1D("hDecayTimeLogAll","Implant-decay correlation time log bins",Nbins,edges); // ms
  hDecayTime1LogVsDecayE1 = new TH2D("hDecayTime1LogVsDecayE1","time correlations with decay energy",10000,0,10000,Nbins,edges);
  hDecayTime2LogVsDecayE2 = new TH2D("hDecayTime2LogVsDecayE2","time correlations with decay energy",10000,0,10000,Nbins,edges);

  hDecayTimeEgate   = new TH1D("hDecayTimeEgate","Implant-decay correlation time energy gated",2000,0,2E9); // ms
  hDecayEnergy = new TH1D("hDecayEnergy","Decay energy (DSSD)",10000,0,10000);
  hDecayEnergyTot = new TH1D("hDecayEnergyTot","Decay energy (DSSD)",10000,0,10000);
  hDecayEnergyAll = new TH1D("hDecayEnergyAll","Decay energy (DSSD)",10000,0,10000);
  hDecayEnergyTGate = new TH1D("hDecayEnergyTGate","Decay energy (DSSD) time Gated",10000,0,10000);
  hDecayEnergyTGateStrict = new TH1D("hDecayEnergyTGateStrict","Decay energy (DSSD) time Gated",10000,0,10000);
  hDecayEnergyTot_TGate = new TH1D("hDecayEnergyTot_TGate","Decay energy (DSSD) time Gated",10000,0,10000);
  hDecayEnergyAvg_TGate = new TH1D("hDecayEnergyAvg_TGate","Decay energy (DSSD) time Gated",10000,0,10000);
  hDecayEnergyTot_firstEvent = new TH1D("hDecayEnergyTot_firstEvent","Decay energy (DSSD) first Event",10000,0,10000);
  hDecayEnergy_firstEvent = new TH1D("hDecayEnergy_firstEvent","Decay energy (DSSD) first Event",10000,0,10000);
  hDecayEnergy_secondEventTGate = new TH1D("hDecayEnergy_secondEventTGate","Decay energy (DSSD) first Event",10000,0,10000);
  hDecayEnergy_firstEventTGate = new TH1D("hDecayEnergy_firstEventTGate","Decay energy (DSSD) first Event",10000,0,10000);
  hDecayEnergy_firstEventBackground = new TH1D("hDecayEnergy_firstEventBackground","Decay energy (DSSD) first Event",10000,0,10000);
  hDecayEnergyAmp = new TH1D("hDecayEnergyAmp","Decay energy (DSSD Amplitude calc.)",5000,0,5000);
  hDecayEnergyBackground = new TH1D("hDecayEnergyBackground","Decay energy (DSSD)",10000,0,10000);
  hDecayEnergyBackgroundUnderflow = new TH1D("hDecayEnergyBackgroundUnderflow","Decay energy (DSSD)",10000,0,10000);
  hDecayEnergyBackgroundScaled = new TH1D("hDecayEnergyBackgroundScaled","Decay energy (DSSD)",10000,0,10000);
  hDecayEnergyTotBackground = new TH1D("hDecayEnergyTotBackground","Decay energy (DSSD)",10000,0,10000);

  hGammaEnergy = new TH1D("hGammaEnergy","SeGA gamma energy",10000,0,10000);
  hGammaEnergy_R3events = new TH1D("hGammaEnergy_R3events","SeGA gamma energy",10000,0,10000);
  hPromptGammaEnergy = new TH1D("hPromptGammaEnergy","SeGA gamma energy",10000,0,10000);
  hGammaVsDecay = new TH2D("hGammaVsDecay","Gamma E vs Decay E",1500,0,15000,10000,0,10000);
  hGammaVsDecayEtot = new TH2D("hGammaVsDecayEtot","Gamma E vs Decay E",1500,0,15000,10000,0,10000);
  hGammaVsDecayTGated = new TH2D("hGammaVsDecayTGated","Gamma E vs Decay E",1500,0,15000,10000,0,10000);
  hGammaVsDecayBackground = new TH2D("hGammaVsDecayBackground","Gamma E vs Decay E",1500,0,15000,10000,0,10000);
  hGammaVsDecayAll = new TH2D("hGammaVsDecayAll","Gamma E vs Decay E",1500,0,15000,10000,0,10000);
  hGammaEnergyG= new TH1D("hGammaEnergyG","SeGA gamma energy Gated",10000,0,10000);
  hGammaTvsDecayT = new TH2D("hGammaTdecayTvsGammaE","gamma decayT vs gammaE",500,-2500,2500,10000,0,10000);
  hGammaEvsImplantT = new TH2D("hGammaTimplantTvsGammaE","gamma implantT vs gammaE",1000,-5000,5000,10000,0,10000);
  hGammaTvsDet = new TH2D("hGammaTvsDet","gamma decay T vs det",500,-2500,2500,16,-0.5,15.5);
  hGammaEvsGammaE = new TH2D("hGammaEvsGammaE","gammaE vs gammaE",10000,0,10000,10000,0,10000);
  hGammaEvsGammaE_back = new TH2D("hGammaEvsGammaE_back","gammaE vs gammaE back",10000,0,10000,10000,0,10000);
  
  //Is this matrix that much of a memory hog? memory usage more than doubles with this histogram, weird
  //gg_prompt = new TH2D("gamma-gamma prompt","gamma-gamma prompt",10000,0,10000,10000,0,10000);

  hSeGAEnergy  = new TH2D("hSeGAEnergy","SeGA gamma energies",16,0,16,5000,0,10000);
  hImplantXY   = new TH2I("hImplantXY", "DSSD hit pattern (xy position)",  40,-20,20,40,-20,20);
  hImplantXY->GetXaxis()->SetTitle("x (mm)");
  hImplantXY->GetYaxis()->SetTitle("y (mm)");
  hDecayXY   = new TH2I("hDecayXY", "DSSD hit pattern (xy position)",  40,-20,20,40,-20,20);
  hDecayXY->GetXaxis()->SetTitle("x (mm)");
  hDecayXY->GetYaxis()->SetTitle("y (mm)");
  rawSSDhiGain = new TH1I("rawSSDhiGain_all", "SSD raw All strips",(int)pow(2,10),0,pow(2,15));
  calibratedSSDhiGain = new TH1D("calibratedSSDhiGainAll","SSD energy",10000,0,10000);
  rawScint = new TH1I("rawScint", "Raw Scintillator",(int)pow(2,10),0,pow(2,15));

  hGammaVsTime = new TH2D("gammaVsTime","gammaVsTime",5000,0,5E9,10000,0,10000);
}


void histo::write()
{
  dirTraces->cd();
  for(auto & trace : graphTraces){
    trace->Write();
  }

  for(auto & trace : traceHistos){
    trace->Write();
  }


  file->Write();
  std::cerr << " DONE! --->histos written to file root-files/" << name << "                                           " << std::endl << std::endl;
  //file->Close();
  /*
    for (int i=0;i<Ntele;i++)
    {
    delete red[i];
    }
    delete [] red;
  */
}
