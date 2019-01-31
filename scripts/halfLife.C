
TCanvas CdecayTime("CdecayTime","CdecayTime",800,600);
TCanvas CdecayEnergy("CdecayEnergy","CdecayEnergy", 800,600);

void halfLife(){
  gStyle->SetOptStat(0); //turn off stat box on histogram
  //gStyle->SetOptFit(1111);
  
  ostringstream dataName;
  ostringstream directory;
  directory << "../root-files/";
  dataName << directory.str() << "allRunsDanCal.root";

  double fitRange = 3E8;

  TFile * data = new TFile(dataName.str().c_str());
  
  CdecayTime.cd();
  CdecayTime.SetLogy();
  TH1D * decayTime = (TH1D*)data->Get("Histos/hDecayTime")->Clone("Decay Time");
  
  decayTime->GetYaxis()->SetTitle("Counts");
  decayTime->GetXaxis()->SetTitle("ms");

  decayTime->Rebin(25);
  decayTime->Sumw2();

  decayTime->GetXaxis()->SetRangeUser(0,fitRange);
  decayTime->SetMarkerStyle(21);
  decayTime->Draw("PE1");



  TF1 * exp = new TF1("exp","expo(0) + pol0(2)",0,fitRange); //1E9 is true end of range
  decayTime->Fit(exp,"EMR"); //P for pearson test, and uses weight as sqrt(binentries) which is exactly what I want it to do. 
  
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
  TH1D * decayEnergy = (TH1D*)data->Get("Histos/hDecayEnergy")->Clone("Decay Energy");
  
  decayEnergy->GetYaxis()->SetTitle("Counts");
  decayEnergy->GetXaxis()->SetTitle("keV");

  decayEnergy->Rebin(10);
  //decayEnergy->Sumw2();
  decayEnergy->Draw();



}
