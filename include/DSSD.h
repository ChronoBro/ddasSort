//not sure this will be useful unless I can abstract to use for other cases...

#include <vector>
#include <iostream>
#include <ddasDet>


class DSSD{
 public:
  DSSD();
  DSSD(int nStripsFront0, int nStripsBack0);
  ~DSSD();
  bool calibrate(); //will return true if successful, false if not
  bool match(vector<Event> backEvents, vector<Event> frontEvents); //match front and back events with each other, false if no matches, true if >=1 matches
  bool setGeometry(); //will read in map of x-y strip to a point in the lab frame, true if successful, false if not


  vector<siEvent> matchedEvents;

 private:
  int nStripsX;
  int nStripsY;

};
