SRCDIR = src/
INCDIR = ./include
OBJDIR = objs/


#OBJECTS = objs/det.o objs/detector.o objs/histo.o objs/caen.o  objs/elist.o  objs/solution.o objs/einstein.o objs/newton.o objs/correl.o
OBJECTS = objs/histo.o objs/ddasDet.o
#ALLOBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
ALLOBJECTS := $(patsubst $(SRCDIR)%.cpp,$(OBJDIR)%.o,$(wildcard $(SRCDIR)*.cpp))

CFLAGS= -c -O2 -std=c++11 -W -I$(shell root-config --incdir) -g -I$(INCDIR)
COMPILER= g++
LINKOPTION = $(shell root-config --libs) -L./lib -lddaschannel -Wl,-soname,libddaschannel.so,-rpath=/lib/libddaschannel.so

all: sort andyBuild

sort: objs/sort.o $(OBJECTS)
	$(COMPILER) -o sort objs/sort.o $(OBJECTS) $(LINKOPTION)

andyBuild: objs/andyBuild.o $(OBJECTS)
	$(COMPILER) -o andyBuild objs/andyBuild.o $(OBJECTS) $(LINKOPTION)


$(ALLOBJECTS): $(OBJDIR)%.o : $(SRCDIR)%.cpp
	$(COMPILER) $(CFLAGS) $< -o $@


clean:
	rm -f $(OBJDIR)*.o

