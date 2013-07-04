NAME = lua-datetime
VERSION = 0.1
DIST := $(NAME)-$(VERSION)

CC = gcc
RM = rm -rf

#use ld option --allow-multiple-definition here to avoid modifying too many codes of timelib from PHP's date extension
CFLAGS = -Wall -g -fPIC -I/home/microwish/lua/include -I.
#CFLAGS = -Wall -g -O2 -fPIC -I/home/microwish/lua/include -I.
LFLAGS = -shared -Wl,--allow-multiple-definition -L/home/microwish/lua/lib -llua -L./timelib -ltimelib
INSTALL_PATH = /home/microwish/lua-datetime/lib

all: datetime.so

datetime.so: datetime.o
  $(CC) -o $@ $< $(LFLAGS)

datetime.o: ldatetimelib.c
	$(CC) -o $@ $(CFLAGS) -c $<

install: datetime.so
	install -D -s $< $(INSTALL_PATH)/$<

clean:
	$(RM) *.so *.o

dist:
	if [ -d $(DIST) ]; then $(RM) $(DIST); fi
	mkdir -p $(DIST)
	cp *.c Makefile $(DIST)/
	tar czvf $(DIST).tar.gz $(DIST)
	$(RM) $(DIST)

.PHONY: all clean dist
