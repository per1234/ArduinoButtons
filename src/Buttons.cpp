
/*


*/


#include "Buttons.h"

Buttons::Buttons() : 
  prevState(0), timeLastChange(0), timeLastKeyDown(0), timeLastKeyRepeat(0), 
  timeDebounce(20), repeatDelay(100), repeatWait(200), timeLongWait(3000),
  onKeyPress(NULL), onKeyDown(NULL), onKeyUp(NULL),  
  onKeyLongDown(NULL), onKeyLongUp(NULL), onStateChange(NULL),
  longFired(0)
{

}

void Buttons::update() {
  this->check();
}



uint8_t Buttons::check() {
  uint8_t newState = this->getState();
  long time = millis();
  if (newState == 0) this->longFired = 0; //reset the long press
  if (newState != this->prevState) { //state change
    if (time - this->timeLastChange > this->timeDebounce) { //valid change
			if ((this->prevState == 0x00) && (newState != 0x00)) { //key has been pressed
				this->timeLastKeyDown = time;
				this->timeLastKeyRepeat = time;
			}
      if (this->onStateChange != NULL) this->onStateChange(newState); 
      uint8_t mask = 0x01;
      for (uint8_t i = 0; i < 8; i++) {
        if (!(this->prevState & mask) && (newState & mask)) { //change from 0 to 1
          if (this->onKeyDown != NULL) this->onKeyDown(newState & mask); 
          if (this->onKeyPress != NULL) this->onKeyPress(newState & mask);
        } else if ((this->prevState & mask) && !(newState & mask)) { //change from 1 to 0
          if (this->onKeyUp != NULL) this->onKeyUp(this->prevState & mask);
        } else {
          //no change for this bit
        }
        mask <<= 1;
      }
      this->prevState = newState;
      this->timeLastChange = time;
      return 1;
    } 
  } else if ((this->onKeyPress != NULL) && (newState != 0x00)) { //no state change, but at least one button pressed and we have autorepeat handler attached
		//check for autorepeat
		if (time - this->timeLastKeyDown > this->repeatWait) { //waited enough for autorepeat
			if (time - this->timeLastKeyRepeat > this->repeatDelay) { //time to autorepeat again
				uint8_t mask = 0x01;
				for (uint8_t i = 0; i < 8; i++) {
					if (newState & mask) this->onKeyPress(newState & mask);
					mask <<= 1;
				}
				this->timeLastKeyRepeat = time;
			}
		}
  } else if ((this->onKeyLongDown != NULL) && (newState != 0x00) && (this->longFired == 0)) { //no state change, but at least one button pressed and we have long press handler attached
    //check if it has been pressed long enough
		if (time - this->timeLastKeyDown > this->timeLongWait) { //waited enough for long press
      this->longFired = 1; //so we don't repeat the event, resets when all buttons up
      uint8_t mask = 0x01;
      for (uint8_t i = 0; i < 8; i++) {
        if (newState & mask) this->onKeyLongDown(newState & mask);
        mask <<= 1;
      }
		}
  }
  return 0;
  
}


uint8_t Buttons::getState() {
  return 0;
}



//