mypython: mypython.cpp parser.cpp parser.h lexical.cpp lexical.h
	g++ -std=c++11 mypython.cpp parser.cpp parser.h lexical.cpp lexical.h -o mypython

#make andrun file=<file.py>
andrun: mypython
	./mypython $(file)

clean:
	rm mypython
