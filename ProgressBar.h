// ProgressBar
// Karl Kosack <kosack@hbar.wustl.edu>

#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <cstdio>
#include <sys/time.h>
#include <string>

/**
 * Displays a text-based progress bar with estimated time to finish
 * for a particular task.
 */ 
class ProgressBar {

 public:
    
    ProgressBar( int total, std::string name="Progress" );
    ~ProgressBar();
    void setName( std::string name ){ _name=name; }
    void print( int );
    void printClear();
    double getTime();

    void start();
    void stop();

 private:
    
    std::string _name;
    int _last;
    char _spinner[4];
    int _total;
    int _spin;
    double _now;
    double _before;
    double _avgpersec;
    double _is_ansi_term;

};

class ProgressBarHandler {
    
 public:

    static ProgressBarHandler *instance();
    bool isRunning() { return _running; };
    int getValue() { return _curval; }
    void print() { if (_prog) _prog->print(_curval); }
    void setValue(double val) { _curval = val; }
    void setProg( ProgressBar &prog ){ _prog = &prog; }
    void start() { _running=true; setitimer(ITIMER_REAL,&_timer,NULL);  }
    void stop()  { 
	_running=false;
	setitimer(ITIMER_REAL,NULL,NULL); 
	_prog->printClear();
    }


    
 protected: 
    ProgressBarHandler();


 private:

    static ProgressBarHandler *pinstance;
    ProgressBar *_prog;
    int _curval;
    struct itimerval _timer;
    bool _running;

};

void progressbar_signal_handler( int n );


#endif
