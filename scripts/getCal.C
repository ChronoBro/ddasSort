void getCal(){

  char name[500];
  int SeGAchannel = 16;
  TFile *fSeGACalFile = new TFile("../Calibrations/SeGACalibrations.root","READ");

  ofstream output("../Calibrations/SeGACalib.dat");

  for(Int_t i=0; i<16; i++){
    sprintf(name,"f_cal_SeGA_%02i",i);
    TF1 *funG= (TF1*)fSeGACalFile->FindObjectAny(name);
    
    vector<double> params;
    params.push_back(funG->GetParameter(0));
    params.push_back(funG->GetParameter(1));
    
    output << SeGAchannel << "\t" << funG->GetParameter(0) << "\t" << funG->GetParameter(1) << endl;

    SeGAchannel++;

    delete funG;
  }
  fSeGACalFile->Close();

  ofstream output1("../Calibrations/DSSDloFrontChannels.dat");
  ofstream output2("../Calibrations/DSSDloBackChannels.dat");


  for(int i=0;i<40;i++){
    
    output1 << (104 + i) << endl;
    output2 << (144 + i) << endl;

  }
    


}
