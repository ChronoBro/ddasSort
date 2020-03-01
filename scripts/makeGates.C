void makeGatesPID(string dataName0){

  ostringstream dataName;
  ostringstream directory;
  directory << "../root-files/";

  dataName << directory.str() << dataName0;


  TFile * data = new TFile(dataName.str().c_str());


  TH1D * decayTime = (TH1D*)data->Get("Histos/hDecayTime")->Clone("Decay Time");

}
