#include <iostream>
#include <vector>
#include "math.h"
#include <string>
#include <sstream>

struct namedCounter{
  int n = 0;
  std::string name = "";

};

class diagnostics{
 public:
  diagnostics();
  ~diagnostics();
  diagnostics(std::vector<std::string> counts);
  
  void add(std::string counterName);
  void count(std::string counterName);
  int returnValue(std::string counterName);
  void reset(std::string counterName);

 private:
  std::vector<namedCounter> list;
  

};
