// ProgressBar
// Karl Kosack <kosack@hbar.wustl.edu>

#include <iostream>
#include <iomanip>
#include <ctime>
#include <cstdio>
#include <sys/time.h>
#include <signal.h>

#include "ProgressBar.h"

using std::cout;
using std::cerr;
using std::endl;




ProgressBar::ProgressBar( int total, std::string name ) 
    : _total(total),_last(0),_spin(0),_name(name),_avgpersec(0),
      _is_ansi_term(true)
{

    const char ESC = 0x1b;


    _spinner[0] = '|';
    _spinner[1] = '/';
    _spinner[2] = '-';
    _spinner[3] = '\\';

    std::string term = getenv("TERM");
    if (   term.find("xterm") == std::string::npos 
	&& term.find("vt100") == std::string::npos 
	&& term.find("vt220") == std::string::npos ) {
	_is_ansi_term = false;
    }

    if (_is_ansi_term) {
	cerr << ESC << "[?25l";  // disable cursor
    }

}


ProgressBar::~ProgressBar() {
    const char ESC = 0x1b;

    if (_is_ansi_term) {
	cerr << ESC << "[?25h";  // enable cursor
	cerr << "Cursor reenabled." << endl;
    }
}


/**
 * Displays the progress bar, at position n
 */
void
ProgressBar::print( int n ) {

    int i;
    float percent;
    int secleft, minleft;
    double evtpersec;
    int prog;
    double delta = n - _last;
    const char ESC = 0x1b;

    if (n<=0) return;

    _spin++; if (_spin>=4) _spin=0;
    percent = (float)n/_total;
    prog = (int)(percent * 30); 
    cerr << _name<<": "<< _spinner[_spin] << " [" ;
    if (_total == 0) {
	cerr << "please wait...]                       ";
	cerr <<"  \r" << std::flush;
	return;
    }

    if (_is_ansi_term) {

	for (i=0; i<=prog; i++)   cerr<<ESC<<"[01;44m "<<ESC<<"[0m";//"#";
	for (i=prog+1; i<30; i++) cerr<<"-"; //cerr << "_";
    }
    else {
	for (i=0; i<=prog; i++)   cerr<<"#";
	for (i=prog+1; i<30; i++) cerr<<"_";
    }

    _now = getTime();

    if (delta > 0) {
	evtpersec = (_now-_before > 0)? (delta/((_now-_before))):-1;
	_before = _now;
	_last = n;
    }


    _avgpersec = (evtpersec + _avgpersec)/2.0;
    secleft = (_avgpersec>0)?(int)((_total-n)/_avgpersec):0;
    minleft = secleft/60;
    secleft = secleft % 60;
    cerr << ESC << "[0m"
	 << "] " 
	 << std::setw(2) << (int)(percent*100) << "% ";


    if ( evtpersec > 1e8 || evtpersec < 0) 
	cerr << "??? per sec, ";
    else 
	cerr << std::setw(6) <<std::setprecision(4)<<evtpersec<<" per sec, ";
	
    if ( minleft < 60 && secleft > 0) {
	cerr << std::setw(2) << minleft <<":" 
	     << std::setw(2) << std::setfill('0')<< secleft << " left"
	     << std::setfill(' ');
    }
    else {
	cerr << "           ";
    }
	

    cerr <<"  \r" << std::flush;

    
}


/**
 * Returns the current system time in microseconds
 */
double
ProgressBar::getTime() {

    struct timeval tv;
    struct timezone tz;

    gettimeofday( &tv,&tz);

    return (tv.tv_sec + (double)tv.tv_usec/1.0e6);
    
}

/**
 * Print a string of spaces over the progress bar area.
 */
void
ProgressBar::printClear() {

    const char ESC = 0x1b;
   
    cerr << "                                              "
	"                                \r"
	 <<std::flush;

    if (_is_ansi_term) {
	cerr << ESC << "[?25h";  // enable cursor
    }

}



ProgressBarHandler* ProgressBarHandler::pinstance = NULL;

void progress_signal_handler( int n) {

    if (ProgressBarHandler::instance()->isRunning() ) 
	ProgressBarHandler::instance()->print();

}



ProgressBarHandler*
ProgressBarHandler::instance() {
    if (pinstance==NULL) 
	pinstance = new ProgressBarHandler();
    return pinstance;
}


ProgressBarHandler::ProgressBarHandler() {

    _timer.it_interval.tv_sec = 0;
    _timer.it_interval.tv_usec = 500000;
    _timer.it_value.tv_sec = 0;
    _timer.it_value.tv_usec = 40000;

    struct sigaction sa;
    
    sigfillset( &sa.sa_mask );
    sa.sa_flags = 0;
    sa.sa_handler = progress_signal_handler;
    if (sigaction(SIGALRM, &sa, NULL) < 0) {
        perror( "sigaction SIGALRM" );
        exit(1);
    }


}
