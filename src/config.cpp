#include "config.h"

using namespace ctoml;

//shoutout to PherricOxide on StackOverflow for a quick test if a file exists
inline bool config::exists_test (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}



void config::config(string configName, int runStart, int runEnd){

  if(exists_test(configName)){

    //parsing configuration file
    auto config = cpptoml::parse_file(name);

    auto dataDir = config->get_as<string>("dataDir");



    for(int iFile=runStart;iFile<=runEnd;iFile++){
 
      for(int subRun = 0;subRun <= maxSubRun;subRun++){

	ostringstream infile;      

	if(iFile>= 100){
	  infile << dataDir <<  "run-0" << iFile << "-0" << subRun << "_NoBuild.root";
	}
	else{
	  infile << dataDir << "run-00" << iFile << "-0" << subRun << "_NoBuild.root";
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

    for (const auto& table : *tarr)
      {
	// *table is a cpptoml::table
	auto name = table->get_as<std::string>("name");
      
      }

  }
  else{
    std::cout << "Couldn't find configuration file, aborting..." << std::endl;
    abort();
  }

}
