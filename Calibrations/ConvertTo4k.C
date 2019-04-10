{
  TFile f("root-files/SeGACalib_run-0035.root");
  TFile fOut("SeGACalib_run-0035-4k.root","RECREATE");
  Char_t name[100];
  for(Int_t i=0; i<16; i++){
    sprintf(name,"h_SeGA%02i",i);
    TH1I *h = f.FindObjectAny(name);
    if(h){
     h->Rebin(8);
     h->Write("");
    }
  }
  fOut->Close();
  f.Close();

}
