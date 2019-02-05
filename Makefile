SRCDIR = src/
INCDIR = ./include
OBJDIR = objs/


#OBJECTS = objs/det.o objs/detector.o objs/histo.o objs/caen.o  objs/elist.o  objs/solution.o objs/einstein.o objs/newton.o objs/correl.o
OBJECTS = objs/histo.o objs/ddasDet.o objs/ddasArray.o objs/ddasDSSD.o
#ALLOBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
ALLOBJECTS := $(patsubst $(SRCDIR)%.cpp,$(OBJDIR)%.o,$(wildcard $(SRCDIR)*.cpp))

CFLAGS= -c -O2 -std=c++11 -W -I$(shell root-config --incdir) -g -I$(INCDIR)
COMPILER= g++
LINKOPTION = $(shell root-config --libs) -L./lib -lddaschannel -Wl,-soname,libddaschannel.so,-rpath=/lib/libddaschannel.so

all: sort sort2 sort3 #sortDev

sort: objs/sort.o $(OBJECTS)
	$(COMPILER) -o sort objs/sort.o $(OBJECTS) $(LINKOPTION)

sortDev: objs/sortDev.o $(OBJECTS)
	$(COMPILER) -o sortDev objs/sortDev.o $(OBJECTS) $(LINKOPTION)

sort2: objs/sort2.o $(OBJECTS)
	$(COMPILER) -o sort2 objs/sort2.o $(OBJECTS) $(LINKOPTION)

sort3: objs/sort3.o $(OBJECTS)
	$(COMPILER) -o sort3 objs/sort3.o $(OBJECTS) $(LINKOPTION)

andyBuild: objs/andyBuild.o $(OBJECTS)
	$(COMPILER) -o andyBuild objs/andyBuild.o $(OBJECTS) $(LINKOPTION)


$(ALLOBJECTS): $(OBJDIR)%.o : $(SRCDIR)%.cpp
	$(COMPILER) $(CFLAGS) $< -o $@


clean:
	rm -f $(OBJDIR)*.o

