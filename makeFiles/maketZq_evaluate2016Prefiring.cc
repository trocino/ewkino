CC=g++ -Wall -Wno-reorder -Wextra -O3
CFLAGS= -Wl,--no-as-needed,-lTMVA
LDFLAGS=`root-config --glibs --cflags`
SOURCES= src/treeReader.cc src/tZq_evaluate2016Prefiring.cc src/analysisTools.cc src/eventSelection.cc src/trilepTools.cc src/tZqTools.cc bTagSFCode/BTagCalibrationStandalone.cc src/Reweighter.cc src/eventWeights.cc plotting/plotCode.cc plotting/tdrStyle.cc src/kinematicTools.cc src/objectSelection.cc src/treeReaderErrors.cc src/BDTReader.cc plotting/drawLumi.cc src/Sample.cc src/tZqVariables.cc src/systemTools.cc src/stringTools.cc src/PostFitScaler.cc
OBJECTS=$(SOURCES:.cc=.o)
EXECUTABLE= tZq_evaluate2016Prefiring

all: 
	$(CC) $(CFLAGS) $(SOURCES) $(LDFLAGS) -o $(EXECUTABLE)
	
clean:
	rm -rf *o $(EXECUTABLE)
