#include "TMinuit.h"

TCanvas CdecayTime("CdecayTime","CdecayTime",800,600);
TCanvas CdecayEnergy("CdecayEnergy","CdecayEnergy", 800,600);
TCanvas CdecayTimeError("CdecayTimeError","CdecayTimeError", 800,600);


Double_t logFit(double* x, double* par){

  double result = 0;
  double f = TMath::Exp(log(x[0])+log(par[1]));
  double f2 = TMath::Exp(log(x[0])+log(par[3]));
  //result = par[0]*f*TMath::Exp( -1*f  );
  result = par[0]*f*TMath::Exp( -1*f  ) + par[2]*f2*TMath::Exp(-1*f2);
  return result;

}

void halfLife(){
  gStyle->SetOptStat(1); //turn off stat box on histogram
  //gStyle->SetOptFit(1111);
  
  ostringstream dataName;
  ostringstream directory;
  directory << "../root-files/";
  //dataName << directory.str() << "sort3all73Sr_honingIn20.root";
  //dataName << directory.str() << "sort3all73Sr_noGatesWFilter.root";
  //dataName << directory.str() << "sort3all73Sr_noGatesWFilter4.root";
  //dataName << directory.str() << "sort3all73Sr_noGatesWFilter5.root";
  //dataName << directory.str() << "sort3all73Sr_pixelGate_AllCorrelationEvents.root";
  //dataName << directory.str() << "sort3all73Sr_1pixelGate_AllCorrelationEvents.root";
  //dataName << directory.str() << "sort3all72Rb.root";
  //dataName << directory.str() << "sort3all73Sr_3pixelsTolerance.root";
  //dataName << directory.str() << "sort3all73Sr_2stripTolerance_10sCorrWindow.root";
  //dataName << directory.str() << "sort3all73Sr_2stripTolerance_10sCorrWindow_traces.root";
  //dataName << directory.str() << "sort3all73Sr_2stripTolerance_fixedStripMisMatch.root";
  //dataName << directory.str() << "sort3all73Sr_1stripTolerance_noAddback.root";
  //dataName << directory.str() << "sort3all73Sr_2stripTolerance_badStripRemoval.root";
  //dataName << directory.str() << "sort3all73Sr_2stripTolerance_bad+edgeStripRemoval.root";
  //dataName << directory.str() << "sort3all73Sr_3stripTolerance_badStripRemoval.root";
  //dataName << directory.str() << "sort3all73Sr_3stripTolerance_badStripRemoval_TGate300ms_5sCorrWindow.root";
  //dataName << directory.str() << "sort3all73Sr_3stripTolerance_badStripRemoval_5sCorrWindow.root";
  //dataName << directory.str() << "sort3all73Sr_2stripTolerance_badStripRemoval_5sCorrWindow_firstEventPlot.root";
  //dataName << directory.str() << "sort3all73Sr_3stripTolerance_allBadStripRemoval_5sCorrWindow_EdiffThreshold.root";
  //dataName << directory.str() << "sort3all73Sr_2stripTolerance_bad+edgeStripRemoval_TGate100_5scorrWindow.root";
  //dataName << directory.str() << "secondImplantTest.root";
  dataName << directory.str() << "run_95-110_processed.root";
  //dataName << directory.str() << "sort3all73Sr_2stripTolerance_badStripRemoval_5sCorrWindow.root";
  //dataName << directory.str() << "test71Kr.root";
  //dataName << directory.str() << "test101.root";
  //dataName << directory.str() << "test222.root";

  double fitRange = 3E8;
  //fitRange = 6E8; // 1E9=1s
  fitRange = 2E9;

  TFile * data = new TFile(dataName.str().c_str());
  dataName.str("");
  dataName.clear();
  dataName << directory.str() << "sort3all71Kr_2.root";
  TFile* data2 = new TFile(dataName.str().c_str());
  

  CdecayTime.cd();
  CdecayTime.SetLogy();
  TH1D * decayTime = (TH1D*)data->Get("Histos/hDecayTime")->Clone("Decay Time");
  TH1D * decayTimeLog = (TH1D*)data->Get("Histos/hDecayTimeLog")->Clone("Decay Time Log");

  decayTime->GetYaxis()->SetTitle("Counts");
  decayTime->GetXaxis()->SetTitle("ms");

  decayTime->Rebin(1);
  decayTime->Sumw2();

  decayTime->GetXaxis()->SetRangeUser(0,fitRange);
  decayTime->SetMarkerStyle(21);
  decayTime->Draw("PE1");




  TF1 * exp = new TF1("exp","expo(0) + pol0(2)",0,fitRange); //1E9 is true end of range
  //TF1 * exp = new TF1("exp","expo(0)",0,fitRange); //1E9 is true end of range
  decayTime->Fit(exp,"EMRLL"); //P for pearson test, and uses weight as sqrt(binentries) which is exactly what I want it to do. 

  //TF1* func = new TF1("logFit",logFit,10E5,10E9,2);
  TF1* func = new TF1("logFit",logFit,10E5,10E10,4);
  func->SetParameter(0,300);
  func->SetParameter(1,-1*exp->GetParameter(1));
  func->SetParameter(2,20);
  func->SetParameter(3,3E-10); //for background peak
  //func->SetParameter(3,2E-7);
  decayTimeLog->Draw("PE1");
  decayTimeLog->Rebin(5);
  decayTimeLog->Fit("logFit", "ML");

  double half_lifeLog = log(2)/func->GetParameter(1)*1E-6;
  
  int nParFit = 3;
  double half_life = -1*log(2)/exp->GetParameter(1)*1E-6; //ms
  double hError = exp->GetParError(1);
  double chisq = exp->GetChisquare();
  
  double chisqLog = func->GetChisquare();

  // hError = pow(hError,0.5); //hError now sigma of value
  // cout << "hError = " << hError << endl;  
  // hError = 5*hError; //have error out to 5 sigma
  // cout << "hError = " << hError << endl;  

  double half_lifeErr = half_life*(-1*hError/exp->GetParameter(1));
  double half_lifeErrLog = half_lifeLog*(func->GetParError(1)/func->GetParameter(1));

  double lowLimit = log(2)/(-1*exp->GetParameter(1)+hError)*1E-6;
  double upLimit = log(2)/(-1*exp->GetParameter(1)-hError)*1E-6;

  double ndf = 0;
  int nBins = decayTime->GetNbinsX();
  for(int i=1;i<=nBins;i++){
    if(decayTime->GetBinCenter(i) > fitRange){break;}
    if(decayTime->GetBinContent(i)!=0){ndf++;}
  }

  double ndfLog = 0;
  nBins = decayTimeLog->GetNbinsX();
  for(int i=1;i<=nBins;i++){
    if(decayTimeLog->GetBinCenter(i) > 10E10){break;}
    if(decayTimeLog->GetBinContent(i)!=0){ndfLog++;}
  }


  if(ndf==0){abort();}

  double redChisq = chisq/(double)(ndf - nParFit);
  //cout << "Nbins = " << decayTime->GetNbinsX() << endl;

  cout << endl << "half-life = " << half_life << " ms" << endl;
  cout << "half-life error = " << half_lifeErr << " ms" << endl;
  cout << "half-life logFit = " << half_lifeLog << " ms" << endl;
  cout << "half-life error logFit = " << half_lifeErrLog << " ms" << endl;
  cout << "lower limit = " << lowLimit << " ms" << endl;
  cout << "upper limit = " << upLimit << " ms" << endl;

  cout << endl <<  "Chi^2 = " << chisq << endl;
  cout << "reduced Chi^2 = " << redChisq << endl;

  cout << endl <<  "Chi^2 Log = " << chisqLog << endl;
  cout << "reduced Chi^2 Log = " << chisqLog/ndfLog << endl;


  CdecayEnergy.cd();
  TH1D * decayEnergy = (TH1D*)data->Get("Histos/hDecayEnergyTot_TGate")->Clone("Decay Energy");
  //TH1D * decayEnergy = (TH1D*)data->Get("Histos/hDecayEnergyTot")->Clone("Decay Energy"); //going to try directly subtracting background cuz why not
  TH1D * decayEnergyBackground = (TH1D*)data->Get("Histos/hDecayEnergyTotBackground")->Clone("Background");

  TH1D* decayEnergy71Kr = (TH1D*)data2->Get("Histos/hDecayEnergyTot_TGate")->Clone("Decay 71Kr");
  
  
  
  decayEnergy->GetYaxis()->SetTitle("Counts");
  decayEnergy->GetXaxis()->SetTitle("keV");

  int rebinFactor = 100;

  decayEnergy->Rebin(rebinFactor);
  decayEnergyBackground->Rebin(rebinFactor);
  decayEnergy71Kr->Rebin(rebinFactor);

  decayEnergy71Kr->Scale(0.005);

  //background depends on Run, I should make this standard

  //decayEnergy->Add(decayEnergyBackground,-0.2); //The TGate is 200 ms, background is 1 s so amount of background should be 1/5 of measured amount
  //true for sorts in mid April to 5-9-2019
  //decayEnergy->Add(decayEnergyBackground,-0.2/9); //The TGate is 200 ms, background is 9 s so amount of background should be 1/5/9 of measured amount

  //true for sorts 5-9-201
  decayEnergy->Add(decayEnergyBackground,-0.2/4); //The TGate is 200 ms, background is 4 s so amount of background should be 1/5/4 of measured amount

  //for TGate=100ms I need to modify background subtraction
  //decayEnergy->Add(decayEnergyBackground,-0.1/4); //The TGate is 200 ms, background is 4 s so amount of background should be 1/5/4 of measured amount

  //for TGate=300ms run
  //decayEnergy->Add(decayEnergyBackground,-0.3/4); //The TGate is 200 ms, background is 4 s so amount of background should be 1/5/4 of measured amount


  //if looking at whole spectrum (not time gated) then background needs to be scaled for the whole time window
  //decayEnergy->Add(decayEnergyBackground,-10/9.8); //The TGate is 200 ms, background is 9 s so amount of background should be 1/5/9 of measured amount

  //decayEnergy->Add(decayEnergyBackground,-1.02); //The TGate is 200 ms, background is 9 s so amount of background should be 1/5/9 of measured amount



  int binLo=0;
  int binHi=0;

  double p1min = 3000;
  double p1max = 3700;
  double p2min = 3800;
  double p2max = 4400;


  int binLo2=0;
  int binHi2=0;
  for(int i=1;i<=decayEnergy->GetNbinsX();i++){
    if(decayEnergy->GetBinCenter(i)<p1min){binLo = i;}
    if(decayEnergy->GetBinCenter(i)<p1max){binHi = i;}
    if(decayEnergy->GetBinCenter(i)<p2min){binLo2 = i;}
    if(decayEnergy->GetBinCenter(i)<p2max){binHi2 = i;}
  }

  // cout << binLo2 << endl;
  // cout << binHi2 << endl;
  
  cout << endl;
  cout << "Number of counts in 3200 peak = " << decayEnergy->Integral(binLo,binHi) << endl;
  cout << endl;

  cout << endl;
  cout << "Number of counts in 3900 peak = " << decayEnergy->Integral(binLo2,binHi2) << endl;
  cout << endl;



  //decayEnergy->Sumw2();
  //decayEnergy->Add(decayEnergy71Kr,-1);
  decayEnergy->Draw();
  decayEnergyBackground->Scale(0.2/9.);
  decayEnergyBackground->Draw("same");
  //decayEnergy71Kr->Draw("same");


  //I want to get the below working!
  // TMinuit *gMinuit = new TMinuit(2);  //initialize TMinuit with a maximum of 5 params
  // //gMinuit->SetFCN(exp2);

  CdecayTimeError.cd();
  gMinuit->SetErrorDef(1);
  TGraph *gr0 = (TGraph *)gMinuit->Contour(80,2,1);
  gr0->SetLineColor(kRed);
  gr0->Draw("alp");
  gr0->GetXaxis()->SetTitle("parameter 0 (constant)");
  gr0->GetYaxis()->SetTitle("parameter 1 (decayValue)");
  gr0->SetTitle("1-sigma uncertainties on fit parameters");



}
