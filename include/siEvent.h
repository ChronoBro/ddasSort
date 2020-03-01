//not sure this will be useful unless I can abstract to use for other cases...

#include <vector>
#include <iostream>

class semiconEvent{ //should make this a base class, with calibrate & setGeometry
 public:
  siEvent();
  ~siEvent();
  bool calibrate();
  bool setGeometry();
  double getEnergy() const {return energy;}
  unsigned int getEnergyRaw() const {return energyR;}
  double getX();
  double getY();
  double getZ();

 private:
  double energy = 0;  //high gain MeV
  unsigned int energyR = -1; //Raw signal
  int strip = 0;
  double time = 0;

};
