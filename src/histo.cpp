#include "histo.h"


histo::histo(){
  //default constructor
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
	  printf("%s\n\n", "Directory 'root-files' already made");
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
  file->cd();





  dirHistos = new TDirectoryFile("Histos","Histos");
  dirHistos->cd();
  channels = dirHistos->mkdir("channels","channels");

  //dirCsIRaw = dirCsI->mkdir("CsIRaw","raw");
  
  energyCheck = new TH1I("energyCheck","energyCheck",30000,0,30000);
  
  h_raw_summary           = new TH2I("h_raw_summary",          "h_raw_summary",250,0,250,(int)pow(2,12),0,(int)pow(2,15));

  
  channels->cd();
  for(Int_t i=0; i<80; i++){
    char name1[500], title[500];
    sprintf(name1,"h_raw_DSSD_%02i",i);
    sprintf(title,"RAW DSSD energy %02i",i);
    h_raw_DSSD[i] = new TH1I(name1,title,(Int_t)pow(2,15),0,pow(2,15));
    sprintf(name1,"h_raw_DSSD_M1_%02i",i);
    sprintf(title,"RAW DSSD energy with one front and one back strip %02i",i);
    h_raw_DSSD_M1[i] = new TH1I(name1,title,(Int_t)pow(2,15),0,pow(2,15));
  }
  
  dirHistos->cd();
  h_mult   = new TH1I("h_mult",  "DSSD Multiplicity",      40,0,40);
  h_mult_F = new TH1I("h_mult_F","DSSD Front Multiplicity",40,0,40);
  h_mult_B = new TH1I("h_mult_B","DSSD Back Multiplicity", 40,0,40);
  h_raw_DSSD_hits    = new TH2I("h_raw_DSSD_hits",   "DSSD hit pattern (strips)",       40,0,40,40,0,40);
  h_raw_DSSD_hitsMap = new TH2I("h_raw_DSSD_hitsMap","DSSD hit pattern (strips mapped)",40,0,40,40,0,40);
  h_raw_DSSD_hitsXY  = new TH2I("h_raw_DSSD_hitsXY", "DSSD hit pattern (xy position)",  40,-20,20,40,-20,20);
  h_raw_DSSD_hitsXY->GetXaxis()->SetTitle("x (mm)");
  h_raw_DSSD_hitsXY->GetYaxis()->SetTitle("y (mm)");
  
  h_PID        = new TH2I("h_PID","PID PIN1 Energy vs PIN2-XFP TAC", 1250,0,25000,(int)pow(2,10),0,pow(2,15));
  hDecayTime   = new TH1D("hDecayTime","Implant-decay correlation time",1000,0,1E9); // ms
  hDecayEnergy = new TH1D("hDecayEnergy","Decay energy (DSSD)",2500,0,15000);
  hDecayEnergyAmp = new TH1D("hDecayEnergyAmp","Decay energy (DSSD Amplitude calc.)",5000,0,5000);
  hGammaEnergy = new TH1D("hGammaEnergy","SeGA gamma energy",10000,0,10000);
  hGammaEnergyG= new TH1D("hGammaEnergyG","SeGA gamma energy Gated",10000,0,10000);
  hSeGAEnergy  = new TH2D("hSeGAEnergy","SeGA gamma energies",16,0,16,5000,0,10000);
  hImplantXY   = new TH2I("hImplantXY", "DSSD hit pattern (xy position)",  40,-20,20,40,-20,20);
  hImplantXY->GetXaxis()->SetTitle("x (mm)");
  hImplantXY->GetYaxis()->SetTitle("y (mm)");
  hDecayXY   = new TH2I("hDecayXY", "DSSD hit pattern (xy position)",  40,-20,20,40,-20,20);
  hDecayXY->GetXaxis()->SetTitle("x (mm)");
  hDecayXY->GetYaxis()->SetTitle("y (mm)");




  //ostringstream outstring;

  // ECsI = new TH1I*[NCsI];

  // for(int icsi = 0;icsi <NCsI;icsi++){
  //     outstring.str("");
  //     outstring << "ECsI_" << icsi;
  //     string name2 = outstring.str();
  //     dirCsIRaw->cd();
  //     ECsI[icsi] = new TH1I(name2.c_str(),"",1024,0,4095);
  //   }

  // relDifference = new TH1D("relDif","relDif",200,-1,1);
  // timeDif = new TH1D("timeDif","timeDif",1024,0,4095);
  // relVtime = new TH2D("relVtime","relVtime",200,-1,1,200,-10,10);
  // gainMatch = new TH2D("gainMatch","gainMatch",1024,0,4095,1024,0,4095);
  // timeDifCalibrated = new TH1D("timeDifCalibrated","timeDifCalibrated",200,-10,10);

}
//*********************************************


void histo::write()
{
  file->Write();
  std::cout << " DONE! --->histos written to file root-files/" << name << "                                                            " << std::endl << std::endl;
  /*
    for (int i=0;i<Ntele;i++)
    {
    delete red[i];
    }
    delete [] red;
  */
}
