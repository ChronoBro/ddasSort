R__LOAD_LIBRARY(../lib/libddaschannel.so)//have to update this to the libddaschannel.so that you built

#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <TSystem.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>

#include "../include/DDASEvent.h"
#include "../include/ddaschannel.h"

// Take built DDAS events and create trees with only single channel entries.
Int_t UnbuildData(const char* iFileName, const char* oFileName)
{
  //
  TFile *iFile     = new TFile(iFileName,"READ");
  
  if(iFile->IsOpen()) cout << "--> Unbuiding file " << iFileName << "  . . . " << endl;
  else               {cerr <<"ERROR: File could not be opened." << endl; return 0;}

  TTree *iTree     = (TTree*)iFile->FindObjectAny("dchan");
  DDASEvent *iev   = new DDASEvent();
  iTree->SetBranchAddress("ddasevent",&iev); //Need to set the DDASEvent pointer to the same as tree
  
  TFile *oFile     = new TFile(oFileName,"RECREATE");

  TTree     *oTree = new TTree("dchan","A time-ordered list of DDAS channel events");
  DDASEvent *oev   = new DDASEvent();
  std::vector<ddaschannel*> pChannels;
  oTree->Branch("ddasevent",&oev);
  oFile->cd();
  
  for(Long64_t entry=0; entry<=iTree->GetEntries(); entry++){
    // Progress . . .
    if(entry%10000==0) {cout << entry << "\r" << std::flush;}
    
    iTree->GetEntry(entry);
    pChannels = iev->GetData();
    
    for(auto &ch : pChannels){
      ddaschannel *och = ch;
      oev->AddChannelData(och);
      oTree->Fill();
      oev->GetData().clear();
    }
  }
  
  oTree->FlushBaskets();
  oTree->Write();
  oFile->Close();
  
  iFile->Close();
  
  return 1;
}

void UnbuildDataFiles(const char* path, Int_t first, Int_t last){
  //
  Char_t name[500];
  TSystemDirectory sd("sd",path);
  TList *fileList = sd.GetListOfFiles();
  fileList->Sort();

  for(Int_t i=first; i<last+1; i++){
    TIter nextFile(fileList);
    while(TSystemFile *file = (TSystemFile*)nextFile()){
      for(Int_t j=0; j<100; j++){
        sprintf(name,"run-%04i-%02i.root",i,j);
        Char_t filePath[500], outFilePath[500];
        sprintf(filePath,"%s/%s",path,name);
	//path += '../unbuilt';
        sprintf(outFilePath,"%s/../unbuilt/run-%04i-%02i_NoBuild.root",path,i,j);
        if(strcmp(name,file->GetName())==0) UnbuildData(filePath,outFilePath);
      }
    }
  }
}
