SRCDIR = src/
INCDIR = $(PWD)/include
OBJDIR = objs/


#OBJECTS = objs/det.o objs/detector.o objs/histo.o objs/caen.o  objs/elist.o  objs/solution.o objs/einstein.o objs/newton.o objs/correl.o
OBJECTS = objs/histo.o objs/ddasDet.o objs/ddasArray.o objs/ddasDSSD.o objs/diagnostics.o objs/RBDDTrace.o objs/RBDDASChannel.o objs/RBDDChannel.o objs/RBDDTriggeredEvent.o objs/RBDDdet.o objs/RBDDarray.o objs/DDASEvent.o objs/ddaschannel.o objs/DDASHit.o objs/DDASHitUnpacker.o objs/DDASdict.o
#ALLOBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
ALLOBJECTS := $(patsubst $(SRCDIR)%.cpp,$(OBJDIR)%.o,$(wildcard $(SRCDIR)*.cpp))

CFLAGS= -c -O2 -W -I$(shell root-config --incdir) -g -I$(INCDIR) #-std=c++11
COMPILER= g++
LINKOPTION = $(shell root-config --libs) -L./lib #-lddaschannel -Wl,-soname,libddaschannel.so,-rpath=/lib/libddaschannel.so

all: src/DDASdict.cpp sort4 #sort3 sort3_v2 sort3_v3 sort4 #sortDev sort sort2 

sort: objs/sort.o $(OBJECTS)
	$(COMPILER) -o sort objs/sort.o $(OBJECTS) $(LINKOPTION)

sortDev: objs/sortDev.o $(OBJECTS)
	$(COMPILER) -o sortDev objs/sortDev.o $(OBJECTS) $(LINKOPTION)

sort2: objs/sort2.o $(OBJECTS)
	$(COMPILER) -o sort2 objs/sort2.o $(OBJECTS) $(LINKOPTION)

# sort3: objs/sort3.o $(OBJECTS)
# 	$(COMPILER) -o sort3 objs/sort3.o $(OBJECTS) $(LINKOPTION)

sort3_v2: objs/sort3_v2.o $(OBJECTS)
	$(COMPILER) -o sort3_v2 objs/sort3_v2.o $(OBJECTS) $(LINKOPTION)

sort3_v3: objs/sort3_v3.o $(OBJECTS)
	$(COMPILER) -o sort3_v3 objs/sort3_v3.o $(OBJECTS) $(LINKOPTION)

sort4: objs/sort4.o $(OBJECTS)
	$(COMPILER) -o sort4 objs/sort4.o $(OBJECTS) $(LINKOPTION)

andyBuild: objs/andyBuild.o $(OBJECTS)
	$(COMPILER) -o andyBuild objs/andyBuild.o $(OBJECTS) $(LINKOPTION)

src/DDASdict.cpp: $(INCDIR)/DDASEvent.h $(INCDIR)/ddaschannel.h LinkDef.h
	rootcint -f src/DDASdict.cpp $(INCDIR)/DDASEvent.h $(INCDIR)/ddaschannel.h LinkDef.h

objs/DDASdict.o: src/DDASdict.cpp

$(ALLOBJECTS): $(OBJDIR)%.o : $(SRCDIR)%.cpp
	$(COMPILER) $(CFLAGS) $< -o $@


clean:
	rm -f $(OBJDIR)*.o src/*dict*

