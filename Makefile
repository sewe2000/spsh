CFLAGS = -O2 -Wall -pedantic
LIBS = -lreadline 
spsh: src/spsh.c
	gcc src/spsh.c ${CFLAGS} ${LIBS} -o spsh
clean:
	rm -f spsh
install:
	mkdir -p /usr/bin
	cp spsh /usr/bin


