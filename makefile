# Makefile for Logger unit tests.
objects  = test.o
objects += test2.o
objects += Log_c.o

headers  = Log_c.h

options = 

test:	$(objects)	$(headers)
	g++ $(options) -o test $(objects)

%.o:	%.cpp	$(headers)
	g++ $(options) -c -o $@ $<

clean:
	rm -f *.exe *.o
