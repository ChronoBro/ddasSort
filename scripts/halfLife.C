
TCanvas CdecayTime("CdecayTime","CdecayTime",800,600);
TCanvas CdecayEnergy("CdecayEnergy","CdecayEnergy", 800,600);

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
  dataName << directory.str() << "sort3all73Sr_1pixelGate_AllCorrelationEvents.root";
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
  
  decayTime->GetYaxis()->SetTitle("Counts");
  decayTime->GetXaxis()->SetTitle("ms");

  decayTime->Rebin(10);
  decayTime->Sumw2();

  decayTime->GetXaxis()->SetRangeUser(0,fitRange);
  decayTime->SetMarkerStyle(21);
  decayTime->Draw("PE1");



  TF1 * exp = new TF1("exp","expo(0) + pol0(2)",0,fitRange); //1E9 is true end of range
  //TF1 * exp = new TF1("exp","expo(0)",0,fitRange); //1E9 is true end of range
  decayTime->Fit(exp,"EMRLL"); //P for pearson test, and uses weight as sqrt(binentries) which is exactly what I want it to do. 
  
  int nParFit = 3;
  double half_life = -1*log(2)/exp->GetParameter(1)*1E-6; //ms
  double hError = exp->GetParError(1);
  double chisq = exp->GetChisquare();
  


  // hError = pow(hError,0.5); //hError now sigma of value
  // cout << "hError = " << hError << endl;  
  // hError = 5*hError; //have error out to 5 sigma
  // cout << "hError = " << hError << endl;  

  double half_lifeErr = half_life*(-1*hError/exp->GetParameter(1));

  double lowLimit = log(2)/(-1*exp->GetParameter(1)+hError)*1E-6;
  double upLimit = log(2)/(-1*exp->GetParameter(1)-hError)*1E-6;

  double ndf = 0;
  int nBins = decayTime->GetNbinsX();
  for(int i=1;i<=nBins;i++){
    if(decayTime->GetBinCenter(i) > fitRange){break;}
    if(decayTime->GetBinContent(i)!=0){ndf++;}
  }

  if(ndf==0){abort();}

  double redChisq = chisq/(double)(ndf - nParFit);
  //cout << "Nbins = " << decayTime->GetNbinsX() << endl;

  cout << endl << "half-life = " << half_life << " ms" << endl;
  cout << "half-life error = " << half_lifeErr << " ms" << endl;
  cout << "lower limit = " << lowLimit << " ms" << endl;
  cout << "upper limit = " << upLimit << " ms" << endl;

  cout << endl <<  "Chi^2 = " << chisq << endl;
  cout << "reduced Chi^2 = " << redChisq << endl;

  CdecayEnergy.cd();
  TH1D * decayEnergy = (TH1D*)data->Get("Histos/hDecayEnergyTot_TGate")->Clone("Decay Energy");
  TH1D * decayEnergyBackground = (TH1D*)data->Get("Histos/hDecayEnergyTotBackground")->Clone("Background");

  TH1D* decayEnergy71Kr = (TH1D*)data2->Get("Histos/hDecayEnergyTot_TGate")->Clone("Decay 71Kr");
  

  int binLo=0;
  int binHi=0;

  int binLo2=0;
  int binHi2=0;
  for(int i=1;i<=decayEnergy->GetNbinsX();i++){
    if(decayEnergy->GetBinCenter(i)<3000){binLo = i;}
    if(decayEnergy->GetBinCenter(i)<3800){binHi = i;}
    if(decayEnergy->GetBinCenter(i)<3800){binLo2 = i;}
    if(decayEnergy->GetBinCenter(i)<4500){binHi2 = i;}
  }

  // cout << binLo2 << endl;
  // cout << binHi2 << endl;
  
  cout << endl;
  cout << "Number of counts in 3200 peak = " << decayEnergy->Integral(binLo,binHi) << endl;
  cout << endl;

  cout << endl;
  cout << "Number of counts in 3900 peak = " << decayEnergy->Integral(binLo2,binHi2) << endl;
  cout << endl;
  
  
  decayEnergy->GetYaxis()->SetTitle("Counts");
  decayEnergy->GetXaxis()->SetTitle("keV");

  int rebinFactor = 50;

  decayEnergy->Rebin(rebinFactor);
  decayEnergyBackground->Rebin(rebinFactor);
  decayEnergy71Kr->Rebin(rebinFactor);

  decayEnergy71Kr->Scale(0.005);

  decayEnergy->Add(decayEnergyBackground,-0.2); //The TGate is 200 ms, background is 1 ms so amount of background should be 1/5 of measured amount
  //decayEnergy->Sumw2();
  //decayEnergy->Add(decayEnergy71Kr,-1);
  decayEnergy->Draw();
  //decayEnergy71Kr->Draw("same");



}
