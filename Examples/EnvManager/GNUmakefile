O = ./GNU/mc68000/
CXXFLAGS =
CPPFLAGS = -I ../oui/include/
OBJS= $(O)envman.o $(O)enventry.o $(O)envwin.o $(O)envopt.o $(O)stringreq.o

envman: $(OBJS)
	g++ -noixemul -o envman $(OBJS) -loui -ldebug

$(O)envman.o: envman.cc envdef.h enventry.h envwin.h envopt.h

$(O)enventry.o: enventry.cc envdef.h enventry.h

$(O)envwin.o: envwin.cc envdef.h envwin.h enventry.h envopt.h

$(O)envopt.o: envopt.cc envopt.h

$(O)stringreq.o: stringreq.cc stringreq.h

install:
	@copy envman Devel:c
	@copy catalogs to locale:catalogs all
	@echo envman installed




