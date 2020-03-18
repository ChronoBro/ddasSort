void makeFilter(string dataName0, string outFile0, string gateName){

  TCanvas * c1 = new TCanvas("PID","PID",800,600);
  
  ostringstream dataName;
  ostringstream directory;
  ostringstream outfile;
  directory << "../root-files/";
  dataName << directory.str() << dataName0;


  TFile * data = new TFile(dataName.str().c_str());

  if(data->IsOpen()){
    
    TH1D * traceCheck = (TH1D*)data->Get("Histos/trace_vs_signal")->Clone("traceDiagnostics");
    traceCheck->Draw("colz");
   
    //data->Close();
  


    TCutG * filter;

    filter = (TCutG*)c1->WaitPrimitive("CUTG");
    filter->SetName(gateName.c_str());

    outfile << directory.str() << gateName;
  
    TFile * outFile = new TFile(outfile.str().c_str(),"UPDATE");

    ostringstream gateName0;
    gateName0 << "../Gates/" << gateName << ".cut";

    ofstream txtVersion(gateName0.str());

    txtVersion << filter->GetN() << endl;

    for(int i=0;i<filter->GetN();i++){
      txtVersion << filter->GetX()[i] << "\t" << filter->GetY()[i] << endl;
    }

    txtVersion.close();
    filter->Write();
    outFile->Close();

  }
  else{
    cout << "Couldn't open data file" << endl;
  }

}
