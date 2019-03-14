R__LOAD_LIBRARY(/home/hoff/Projects/e12024/Analysis/lib/libddaschannel.so) //somehow this just tells ROOT to load this library before the script, no '.L libddashchannel.so' necessary

#include <vector>
#include <sstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <numeric>


//gROOT->ProcessLine(".L ../lib/libddaschannel.so");

TH2F *raw_summary = new TH2F("raw_summary","raw_summary",250,0,250,(int)TMath::Power(2,12),0,TMath::Power(2,15));
//TH2F *calibrated_summary = new TH2F("calibrated_summary","calibrated_summary",250,0,250,(int)TMath::Power(2,12),0,TMath::Power(2,15));
TH2F *calibrated_summary = new TH2F("calibrated_summary","calibrated_summary",250,0,250,10000,0,10000);
TH2I *hEQDC = new TH2I("hEQDC","hEQDC",250,0,250,10000,0,10000);//(Int_t)TMath::Power(2,14));
TH2I *hEPeak = new TH2I("hEPeak","hEPeak",250,0,250,5000,0,5000);
TH1D *raw_channel[250];
TH1D *calibrated_channel[250];
TH1D *raw_channelCheck =  new TH1D("raw_channelCheck","raw_channelCheck",(int)TMath::Power(2,12),0,TMath::Power(2,15));
TH1D *raw_channelGated =  new TH1D("raw_channelGated","raw_channelGated",(int)TMath::Power(2,12),0,TMath::Power(2,15));
TH1D *max_channelCheck =  new TH1D("max_channelCheck","max_channelCheck",6000,-1000,5000);
TH2I *QDCvPeak = new TH2I("QDCvPeak","QDCvPeak",6000,-1000,5000,6000,-1000,5000);
TH2I *reportEvPeak = new TH2I("reportEvPeak","reportEvPeak",(int)TMath::Power(2,12),0,TMath::Power(2,15),6000,-1000,5000);
TH2I *reportEvQDC = new TH2I("reportEvQDC","reportEvQDC",(int)TMath::Power(2,12),0,TMath::Power(2,15),6000,-1000,5000);
TH2I *QDCvPeakChanCheck = new TH2I("QDCvPeakChanCheck","QDCvPeakChanCheck",6000,-1000,5000,6000,-1000,5000);
TH1I *timing = new TH1I("Timing","Timing",1000,-500,500);
TH1I *timingBad = new TH1I("TimingBad","TimingBad",1000,-500,500);
TH2I *qdcBad = new TH2I("invertedQDC","invertedQDC",2000,-1000,1000,2000,-1000,1000);
TH1I *chanBad = new TH1I("badChan","badChan",200,0,200);
TH1D *gammaTot = new TH1D("gammaTot","gammaTot",10000,0,10000);
TH1D *times = new TH1D("time","time",3000,0,300);


// double Integrate(int x1,int x2,double *x,double *y, double baseline1) //Trapezoidal rule
// {
//   double result=0.;
//   for(int i=x1;i<x2;i++)
//     {
//       result = ( (y[i-1]+y[i])/2.-baseline1 )*(x[i]-x[i-1]) + result;
//       //cout << i << endl;
//     }
  
//   return result;
// }

// double findBaseline(int N1, vector<unsigned short>)
// {
//   double baseline=0.;
//   double size = (double)N1*0.05;
//   for(int j=0;j< (int)size;j++)
//     {
//       baseline += y1[j];
//     }

//   baseline = baseline/size;
//   return baseline;

// }

void waveformAnalysis(){

  double startTime = 0;
  double endTime = 0;

  double runTime = 0;

  TFile          *fSeGACalFile; //!
  char name[500],title[500];
  double gains[250],offsets[250],channels[250];

  TList * fSeGACalibFunc = new TList();
  int SeGAchannel = 16;
  fSeGACalFile = new TFile("../Calibrations/SeGACalibrations.root","READ");
  ifstream gammaCalib("../Calibrations/gammaCalib.dat");
  int chanInput=0;
  for(Int_t i=0; i<16; i++){
    sprintf(name,"f_cal_SeGA_%02i",i);
    TF1 *funG= (TF1*)fSeGACalFile->FindObjectAny(name);
    TF1 *fCalG = new TF1(name,"pol1",0,35000);

    fCalG->SetParameter(0, funG->GetParameter(0));
    fCalG->SetParameter(1, funG->GetParameter(1));
    fSeGACalibFunc->Add(fCalG);
    gammaCalib >> chanInput >> offsets[chanInput] >> offsets[chanInput];
    delete funG;
  }
  fSeGACalFile->Close();


  ifstream calib("../Calibrations/alphaCalib.dat");

  if(!calib.is_open()){
    cout << "Couldn't open calibration file" << endl;
    abort();
  }


  TEnv *env = new TEnv("run22_calibration_coefficients.dat");

  for(int i=0;i<250;i++) {
    raw_channel[i] = new TH1D(Form("raw_channel%d",i),Form("raw_channel%d",i),(int)TMath::Power(2,12),0,TMath::Power(2,15));
    //calibrated_channel[i] = new TH1D(Form("calibrated_channel%d",i),Form("calibrated_channel%d",i),(int)TMath::Power(2,12),0,TMath::Power(2,15));
    calibrated_channel[i] = new TH1D(Form("calibrated_channel%d",i),Form("calibrated_channel%d",i),10000,0,10000);
    // gains[i]=env->GetValue(Form("Chan.%d.Gain",i),1.);
    // offsets[i]=env->GetValue(Form("Chan.%d.Offset",i),0.);
  }

  chanInput = 0;
  for(int i=0;!calib.eof();i++){
    calib >> chanInput >> offsets[chanInput] >> gains[chanInput];
    //cout << "still going" << endl;
  }

  ostringstream infile;
  int run = 69;//315;//35;//36;//315;//315;//47;//306; //run 306 = 148Gd source test behind DSSD
               //315 -> mixed 125Sb source
               //52 ->60Co source
               //34 & 35 152Eu source
  int subRun = 0;
  int chanCheck = 100;

  if(run>= 100){
  infile << "../remote/root-files/run-0" << run << "-0" << subRun << ".root";
  }
  else{
  infile << "../remote/root-files/run-00" << run << "-0" << subRun << ".root";
  }

  ostringstream outfile;
  outfile << "../root-files/run_" << run << "-0" << subRun << "_processed.root";

  TFile * file = new TFile(infile.str().c_str());
  TFile * out = new TFile(outfile.str().c_str(), "RECREATE");

  // TFile *file = new TFile("../remote/root-files/run-0042-00.root"); //Gd run
  //TFile *file = new TFile("../remote/root-files/run-0038-00.root"); //Th run

  int del0 = 175;
  int samples = 375;
  int win0 = 200;
  int sampleSize = 20;
  TTree *tr;
  file->GetObject("dchan",tr);
  DDASEvent* anEvent = new DDASEvent;
  tr->SetBranchAddress("ddasevent",&anEvent);
  std::vector<ddaschannel*> channel_data;
  int nentries = tr->GetEntries();
  TDirectoryFile * histos = new TDirectoryFile("histos","histos"); 
  TDirectory * dirTraces;
  dirTraces = histos->mkdir("badTraces","badTraces");
  TDirectory * dirChan;
  dirChan = histos->mkdir("chan","chan");

  double baseCheck;
  double derivativeCheck;
  double maxCheck;
  double QDCcheck;
  double x[samples];
  double y[samples];

  double maxTraces = 200.;
  double traceCounter = 0.;

  double oldTime = 0;
  double curTime = 2E9;
  double oldQDC = 100;



  for(int i=0;i<nentries;i++){
    tr->GetEvent(i);
    channel_data=anEvent->GetData();
    

    for(auto ch: channel_data){
      double reportedE = ch->GetEnergy();
      int chan = ch->GetCrateID()*64+(ch->GetSlotID()-2)*16+ch->GetChannelID();
      raw_summary->Fill(chan,ch->GetEnergy());
      raw_channel[chan]->Fill(ch->GetEnergy());
      //cout << chan << endl;
      curTime = ch->GetCoarseTime();
      times->Fill(curTime/60E9);

      if(i==0 && curTime > 0){startTime=curTime;}
      if(i==(nentries-1)){endTime=curTime;}

      // int gainCheck=0;
      // for(int j=0;j<250;j++){
      // 	//cout << j << endl;
      // 	if(chan == channels[j]){
      // 	  gainCheck = j;
      // 	  break;
      // 	}
      // }

      if(chan>=16 && chan < 32){
      TF1 *fSeGAHighCal = (TF1*)fSeGACalibFunc->At(chan-16);
      //Double_t value =  (TF1*)fSeGACalibFunc->At(chan-16)->Eval(ch->GetEnergy());
      Double_t value = fSeGAHighCal->Eval(ch->GetEnergy());//+fRand->Uniform());
      // //cout << "FOUND Gamma " << value << endl;
      gammaTot->Fill(value);
      calibrated_summary->Fill(chan,value);
      calibrated_channel[chan]->Fill(value);
      }
      else{
      calibrated_summary->Fill(chan,ch->GetEnergy()*gains[chan]+offsets[chan]);
      //cout << ch->GetEnergy()*gains[gainCheck]+offsets[gainCheck] << endl;
      calibrated_channel[chan]->Fill(ch->GetEnergy()*gains[chan]+offsets[chan]);
      }
      // double base = 0;
      // double qdc0 = 0;
      // double max  = 0;

      double base = 0;
      double qdc0 = 0;
      double max  = 0;

      //trigger seems to happen at same sample window (~sample 150) so will tap baseline right before this
      // "negative pulses" (still not sure what cause of these is) are triggering on upward rise, so baseline calculation just before trigger will not help
      
      //run_44-03 makes it look like negative pulse might be "real" (completely mirrors positive side channels) don't know what cause is but 
      //it does look like the reported E from the digitizer is screwed up for these pulses. However, we might be able to recover these signals
      
      for(int iBin=0; iBin<sampleSize; iBin++){ base += (double)ch->trace[iBin];}
      base = base/((double)sampleSize);

      // for(int iBin=130; iBin<151; iBin++){ base += (double)ch->trace[iBin];}
      // base = base/20.;

      double derivativeOld = 200.;
      double derivative = 100.;


      for(int iQDC=0; iQDC<samples; iQDC++) {
	x[iQDC] = iQDC;
	double charge = (double)ch->trace[iQDC] - base;
	qdc0 += charge;// - base;//ch->trace[iQDC] - base;
	//if(max<(ch->trace[iQDC]-base)) max = ch->trace[iQDC];//-base;
	if(iQDC>0 && iQDC<(samples-1)){
	  derivative = ( (double)ch->trace[iQDC+1]-(double)ch->trace[iQDC-1] )/2.; //2 point stencil for approximating first derivative
	}

	if(abs(derivative) < abs(derivativeOld) ){
	  if(max<charge && qdc0>0) max = charge;// - base; //checking if pulse is inverted, if it is then take local minimum
	  else if(max>charge && qdc0<0) max=charge;
	}
	
	// if(max<(charge-base)){ 
	//   max = charge;// - base;
	// }

	derivativeOld = derivative;

      }
      qdc0 = qdc0/samples;
      //qdc0 = integrate(0,samples,ch
      hEQDC->Fill(chan,qdc0);
      hEPeak->Fill(chan,max);
      QDCvPeak->Fill(qdc0,max);
      reportEvPeak->Fill(reportedE,max);
      reportEvQDC->Fill(reportedE,qdc0);
      timing->Fill(curTime-oldTime);
      if(chan==chanCheck) {
	QDCvPeakChanCheck->Fill(qdc0,max);
	raw_channelCheck->Fill(reportedE);
	max_channelCheck->Fill(max);
	if(max < 0){
	  raw_channelGated->Fill(reportedE);
	}
      }



      if(reportedE > 0 && reportedE < 1000 && qdc0 < 0){// && max < 100){
	//cout << "Found bad trace" << endl;
	traceCounter++;
	for(int iBin=0; iBin<samples;iBin++){
	  y[iBin] = (double)ch->trace[iBin] - base;

	  // abort();
	  //cout << iBin << "\t" << y[iBin] << endl;
	}
	  baseCheck = base;
	  maxCheck = max;
	  derivativeCheck = derivative;
	  QDCcheck = qdc0;

	  timingBad->Fill(curTime - oldTime);
	  qdcBad->Fill(qdc0,oldQDC);
	  chanBad->Fill(chan);

	  if(traceCounter < maxTraces){
	    std::ostringstream title;
	    title << "Chan=" << chan << "-Max=" << max;

	    dirTraces->cd();
	    TGraph * badTrace = new TGraph(samples,x,y);
	    badTrace->SetName("badTrace");
	    badTrace->SetTitle(title.str().c_str()); //convert stringstream > string > character *
	    badTrace->Write();

	    std::cout<<"Bad traces: "<< traceCounter << "                     \r" <<  std::flush; //need spaces to flush all characters
	  }


	//abort();
      }
      oldTime = curTime;
      oldQDC = qdc0;

    }
    if(i%1000==0 || i==(nentries-1))
      std::cout<<"Processed "<<i<<"/"<<(nentries-1)<<" events\r"<<std::flush;
  }
  std::cout<<std::endl;
  //h->Draw("colz");
 
  histos->cd();
  // TGraph * badTrace = new TGraph(samples,x,y);
  // badTrace->SetName("badTrace");
  // for(int i=0;i<samples;i++){
  //   cout << x[i] << "\t" << y[i] << endl;
  // }
  // cout << "baseline = " << baseCheck << endl;
  // cout << "QDC = " << QDCcheck << endl;
  // cout << "max = " << maxCheck << endl;
  // cout << "derivative = " << derivativeCheck << endl;

  runTime = endTime-startTime;
  cout << endl;
  cout << "run time = " << runTime/1E9 << " seconds" << endl;
  raw_summary->Write("");
  calibrated_summary->Write("");
  hEQDC->Write("");
  hEPeak->Write("");
  QDCvPeak->Write("");
  reportEvPeak->Write("");
  QDCvPeakChanCheck->Write("");
  raw_channelCheck->Write("");
  raw_channelGated->Write("");
  reportEvQDC->Write("");
  timing->Write("");
  timingBad->Write();
  qdcBad->Write();
  chanBad->Write();
  gammaTot->Write();
  times->Write();
  //badTrace->Write("");
  dirChan->cd();
  for(int i=0;i<250;i++) {
    raw_channel[i]->Write("");
    calibrated_channel[i]->Write("");
  }
  out->Close();
}
//2.04771e+04
//2.96963e+04
