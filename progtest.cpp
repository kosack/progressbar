#include <iostream>
#include <signal.h>
#include "ProgressBar.h"


using namespace std;


int 
main(int argc, char **argv) {

    ProgressBar prog(100,"test");
    ProgressBarHandler *ph = ProgressBarHandler::instance();
    ph->setProg( prog );
    
    srand(100);
    int k;

    ph->start();
    for (int i=0; i<100; i++) {

	ph->setValue(i);
	k = ((double)rand())/RAND_MAX*10000;

	// pause for random amount
	for (int j=0; j<k; j++) {
	    for (int l=0;l<10000; l++); 
	}

    }
    ph->stop();
    
    prog.printClear();


}


