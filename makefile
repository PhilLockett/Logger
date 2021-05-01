# Makefile for Logger unit tests.
objects  = test.o
objects += test2.o
objects += Log_c.o
objects += unittest.o

headers  = Log_c.h
headers += unittest.h

options = -std=c++20

test:	$(objects)	$(headers)
	g++ $(options) -o test $(objects)

%.o:	%.cpp	$(headers)
	g++ $(options) -c -o $@ $<

format:
	tfc -s -u -r Log_c.cpp
	tfc -s -u -r Log_c.h
	tfc -s -u -r test.cpp
	tfc -s -u -r test2.cpp
	tfc -s -u -r unittest.cpp
	tfc -s -u -r unittest.h

clean:
	rm -f *.exe *.o
