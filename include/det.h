#include <vector>
#include <iostream>

struct siEvent{
  double energy = 0;  //high gain MeV
  unsigned int energyR = -1; //Raw signal
  int strip = 0;
  double time = 0;

};

class DSSD{
 public:
  det(int nStripsX0, int nStripsY0);
  ~det();
  bool calibrate(); //will return true if successful, false if not
  bool match(); //match front and back events with each other, false if no matches, true if at least one match
  bool setGeometry(); //will read in map of x-y strip to a point in the lab frame, true if successful, false if not

  vector<siEvent> matchedEvents;

 private:
  siEvent backEvent;
  siEvent frontEvent;
  int n

};
