void makeGatesPID(string dataName0, string outFile0, string gateName){

  TCanvas * c1 = new TCanvas("PID","PID",800,600);
  
  ostringstream dataName;
  ostringstream directory;
  ostringstream outfile;
  directory << "../root-files/";
  dataName << directory.str() << dataName0;


  TFile * data = new TFile(dataName.str().c_str());

  if(data->IsOpen()){
    
    TH1D * PID = (TH1D*)data->Get("Histos/h_PID")->Clone("PID2");
    PID->Draw("colz");
   
    //data->Close();
  


    TCutG * pidGate;

    pidGate = (TCutG*)c1->WaitPrimitive("CUTG");
    pidGate->SetName(gateName.c_str());

    outfile << directory.str() << gateName;
  
    TFile * outFile = new TFile(outFile.str().c_str(),"UPDATE");

    pidGate->Write();

    outFile->Close();

  }
  else{
    cout << "Couldn't open data file" << endl;
  }

}
