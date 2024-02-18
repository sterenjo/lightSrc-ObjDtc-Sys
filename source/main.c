#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer

enum FSMstate state;
enum SYSmode lpm_mode;

void main(void){
  state = state0;  // start in idle state on RESET
  lpm_mode = mode0;     // start in idle state on RESET
  unsigned int x = 5500;
  unsigned int delay = 650;
  //unsigned char str [45] = "AVVDSVVDSVDSVSDVSDTBDFVSD";
  sysConfig();

  while(1){
	switch(state){
	  case state0:
	      enableUartReceive();
	      enterLPM(lpm_mode);
	      break;
		 
	  case state1:
	      ObjectsDtcSys();
	      break;
		 
	  case state2:
	      telemeter();
		break;
	  case state3:
	      lightSrcDtcSys(DIST_LIGHT_ARR);
	      break;
	  case state4:
	      scanAllObjects();
	      break;
	  case state5:
	      saveScript();

	      //flashWrite('D', str);
	      //script();
	      //ObjectsDtcSys();
	      //state = state5;
	      break;

	  case state6:
	      scriptEx();
	      //Fla
	      break;

	  case state7:
	      calibration();
	      break;
	}
  }
}
  
  
  
  
  

