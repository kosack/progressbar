all: progtest

progtest: progtest.cpp ProgressBar.cpp Progressbar.h
	g++ -o progtest progtest.cpp ProgressBar.cpp

clean:
	$(RM) progtest
