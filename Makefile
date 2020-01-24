SRCDIR = src/
INCDIR = $(PWD)/include
OBJDIR = objs/


#OBJECTS = objs/det.o objs/detector.o objs/histo.o objs/caen.o  objs/elist.o  objs/solution.o objs/einstein.o objs/newton.o objs/correl.o
OBJECTS = objs/histo.o objs/ddasDet.o objs/ddasArray.o objs/diagnostics.o objs/RBDDTrace.o objs/RBDDASChannel.o objs/RBDDChannel.o objs/RBDDTriggeredEvent.o objs/RBDDdet.o objs/RBDDarray.o objs/DDASEvent.o objs/ddaschannel.o objs/DDASHit.o objs/DDASHitUnpacker.o objs/DDASdict.o objs/ionCorrelator.o objs/config.o
#ALLOBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
ALLOBJECTS := $(patsubst $(SRCDIR)%.cpp,$(OBJDIR)%.o,$(wildcard $(SRCDIR)*.cpp))

CFLAGS= -c -fPIC -W -I$(shell root-config --incdir) -g -I$(INCDIR) #-std=c++11
COMPILER= g++
LINKOPTION = $(shell root-config --libs) -L./lib #-lddaschannel -Wl,-soname,libddaschannel.so,-rpath=/lib/libddaschannel.so

all: sort5 #sort4_v3 sort4_v2 sort4 sort3_v2 sort3_v3 #sort3 sort3_v2 sort3_v3 sort4 #sortDev sort sort2 

root:  objs src/DDASdict.cpp

objs: 
	mkdir -p objs	

src/DDASdict.cpp: $(INCDIR)/DDASEvent.h $(INCDIR)/ddaschannel.h $(INCDIR)/LinkDef.h
	rootcint -f src/DDASdict.cpp $(INCDIR)/DDASEvent.h $(INCDIR)/ddaschannel.h $(INCDIR)/LinkDef.h

objs/DDASdict.o: objs src/DDASdict.cpp

$(ALLOBJECTS): $(OBJDIR)%.o : $(SRCDIR)%.cpp
	$(COMPILER) $(CFLAGS) $< -o $@

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

sort4: objs/sort4.o $(OBJECTS) src/DDASdict.cpp
	$(COMPILER) -o sort4 objs/sort4.o $(OBJECTS) $(LINKOPTION)

sort4_v2: objs/sort4_v2.o $(OBJECTS) src/DDASdict.cpp
	$(COMPILER) -o sort4_v2 objs/sort4_v2.o $(OBJECTS) $(LINKOPTION)

sort4_v3: objs/sort4_v3.o $(OBJECTS) src/DDASdict.cpp
	$(COMPILER) -o sort4_v3 objs/sort4_v3.o $(OBJECTS) $(LINKOPTION)

sort5: objs/sort5.o $(OBJECTS) src/DDASdict.cpp
	$(COMPILER) -o sort5 objs/sort5.o $(OBJECTS) $(LINKOPTION)

andyBuild: objs/andyBuild.o $(OBJECTS)
	$(COMPILER) -o andyBuild objs/andyBuild.o $(OBJECTS) $(LINKOPTION)


clean:
	rm -f $(OBJDIR)*.o src/*dict*

