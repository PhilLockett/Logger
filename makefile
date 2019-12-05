#ODB.
objects  = test.o
objects += mod.o
objects += Logger_c.o
objects += Log_c.o

headers  = Logger_c.h
headers += Log_c.h

options = 

test:	$(objects)	$(headers)
	g++ $(options) -o test $(objects)

%.o:	%.cpp	$(headers)
	g++ $(options) -c -o $@ $<

clean:
	rm -f *.exe *.o
