# cxxconf

CXXDEPS := g++ -MM
LDFLAGS :=  -ledit
CXXFLAGS := -Wall -DUSE_HASH_MAP
CXX := g++
TAGS := exctags
PROGRAM := ucsh

.PHONY : all clean
all : $(PROGRAM)
clean :
	rm -f $(PROGRAM) builtin.o command.o main.o parser.o shell.o ucsh.o tags

tags : *.h builtin.cc command.cc main.cc parser.cc shell.cc ucsh.cc
	$(TAGS) *.h builtin.cc command.cc main.cc parser.cc shell.cc ucsh.cc

$(PROGRAM) : builtin.o command.o main.o parser.o shell.o ucsh.o
	$(CXX) $(LDFLAGS) -o ucsh builtin.o command.o main.o parser.o shell.o ucsh.o
builtin.o: builtin.cc builtin.h
command.o: command.cc command.h shell.h builtin.h
main.o: main.cc ucsh.h parser.h command.h shell.h builtin.h
parser.o: parser.cc parser.h command.h shell.h builtin.h
shell.o: shell.cc shell.h command.h builtin.h
ucsh.o: ucsh.cc ucsh.h parser.h command.h shell.h builtin.h
