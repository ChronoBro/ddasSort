#include "diagnostics.h"

diagnostics::diagnostics(){

}


diagnostics::diagnostics(std::vector<std::string> counts){

  for(auto & nCounter: counts){
    namedCounter fillerCounter;
    fillerCounter.name = nCounter;

    list.push_back(fillerCounter);
  }
}

diagnostics::~diagnostics(){

}

void diagnostics::add(std::string counterName){

  namedCounter fillerCounter;
  fillerCounter.name = counterName;

  list.push_back(fillerCounter);

}

void diagnostics::count(std::string counterName){

  for(auto & nCounter : list){
    if( !nCounter.name.compare(counterName) ){ //returns 0 if true, so I want NOT of returned value
	nCounter.n++;
      }	
  }
  
  //std::cout<< std::endl << "No matching counter found for " << counterName << std::endl;

}

void diagnostics::reset(std::string counterName){

  for(auto & nCounter : list){
    if( !nCounter.name.compare(counterName) ){ //returns 0 if true, so I want NOT of returned value
	nCounter.n = 0.;
      }	
  }
  
  //std::cout<< std::endl << "No matching counter found for " << counterName << std::endl;

}

int diagnostics::returnValue(std::string counterName){


  for(auto & nCounter : list){
    if( !nCounter.name.compare(counterName) ){ //returns 0 if true, so I want NOT of returned value
      return nCounter.n;
      }	
  }

  std::cout<< std::endl << "No matching counter found for " << counterName << std::endl;
  return 0;

}
