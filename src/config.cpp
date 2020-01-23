#include "config.h"


//shoutout to PherricOxide on StackOverflow for a quick test if a file exists
inline bool config::exists_test (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}


config::config(std::string configName, int runStart, int runEnd){

  dataChainObject.SetName("dchan");

  if(exists_test(configName)){

    //parsing configuration file
    auto config = cpptoml::parse_file(configName);

    auto dataDir = config->get_as<std::string>("dataDir");


    int maxSubRun = 9;
    for(int iFile=runStart;iFile<=runEnd;iFile++){
 
      for(int subRun = 0;subRun <= maxSubRun;subRun++){

	std::stringstream infile;      

	if(iFile>= 100){
	  infile << *dataDir <<  "run-0" << iFile << "-0" << subRun << "_NoBuild.root";
	}
	else{
	  infile << *dataDir << "run-00" << iFile << "-0" << subRun << "_NoBuild.root";
	}   

	if( exists_test(infile.str()) ){

      
	  dataChainObject.AddFile(infile.str().c_str());
	  std::cout << "added " << infile.str() << " to TChain                           " << std::endl; //have to use carriage return for flush

	}
	else{
	  ////std::cout << "Couldn't find file " << infile.str() << "                        " << std::endl; 
	  break;
	}
      } 

    }

    ////std::cout << "adding " << infile.str() << " to TChain..." << std::endl;
  
    std::cout << std::endl;


    auto Arrays = config->get_table_array("Arrays");

    int iArray=0;
    for (const auto& table : *Arrays){
      // *table is a cpptoml::table
      auto name = table->get_as<std::string>("name");
      auto cal = table->get_as<std::string>("calibrationFile");
      auto map = table->get_as<std::string>("channelListFile");

      int chan = 0.;
      double slope;
      double offset;
    
      arrayObjects[iArray].setName(*name);


      if(cal){
	std::ifstream calibFile(*cal);
	while(!calibFile.eof()){
	  calibFile >> chan >> slope >> offset;
	  
	  std::vector<double> params;
	  params.push_back(offset);
	  params.push_back(slope);
	  
	  detectorObjects[chan].setAssignedChannel(chan);
	  detectorObjects[chan].setCalibration(params);
	  arrayObjects[iArray].addDet(detectorObjects[chan]);
	}
      }
      else if(map){
	std::ifstream channelFile(*map);
	channelFile >> chan;
	detectorObjects[chan].setAssignedChannel(chan);
	arrayObjects[iArray].addDet(detectorObjects[chan]);
      }
      else{
	std::cout << "Need to have channel map file specified in config file, aborting..." << std::endl;
	abort();
      }

      iArray++;
    }
    
    auto Dets = config->get_table_array("Detectors");
    
    for (const auto& table : *Dets){
      auto name = table->get_as<std::string>("name");
      auto channel = table->get_as<int>("channel");
      detectorObjects[*channel].setAssignedChannel(*channel);
      detectorObjects[*channel].setName(*name);
    }
    
  }
  else{
    std::cout << "Couldn't find configuration file, aborting..." << std::endl;
    abort();
  }

}

RBDDarray config::getArray(std::string arrayName){

  for(int i=0;i<maxArrays;i++){
    if(arrayName.compare(arrayObjects[i].getName()) == 0){
      return arrayObjects[i];
    }
  }

  std::cout << "There is no array named " << arrayName << ", aborting..." << std::endl;
  abort();

}

RBDDdet config::getDet(std::string detName){

  for(int i=0;i<maxChannels;i++){
    if(detName.compare(detectorObjects[i].getName()) == 0){
      return detectorObjects[i];
    }
  }

  std::cout << "There is no detector named " << detName << ", aborting..." << std::endl;
  abort();


}