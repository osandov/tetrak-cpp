CPP := g++

LIBROOT := Tetrak
BASEDIR := Tetrak/Base
IMPLEMENTDIR:= Tetrak/Implementation
UTILDIR := Utilities

OBJS = main.o \
		$(BASEDIR)/Argument.o \
		$(BASEDIR)/Parameters.o \
		$(BASEDIR)/StaticDefinitions.o \
		$(BASEDIR)/TempoTrackerBase.o \
		$(IMPLEMENTDIR)/Processor.o \
		$(IMPLEMENTDIR)/EventExtractor.o \
		$(IMPLEMENTDIR)/LiveTempoTracker.o \
		$(IMPLEMENTDIR)/TempoFinder.o \
		$(IMPLEMENTDIR)/Input/FileInputter.o \
		$(IMPLEMENTDIR)/Input/LiveInputter.o \
		$(IMPLEMENTDIR)/Output/GnuplotOutputter.o

HEADERS = $(LIBROOT)/*.hpp \
		$(BASEDIR)/*.hpp \
		$(IMPLEMENTDIR)/*.hpp \
		$(IMPLEMENTDIR)/Input/*.hpp \
		$(IMPLEMENTDIR)/Output/*.hpp

tetrak : $(OBJS)
		$(CPP) $(OBJS) -o tetrak $(CXXFLAGS) -lfftw3 -lsndfile -lportaudiocpp -lportaudio -lboost_iostreams

-include $(OBJS:.o=.d)

%.o : %.cpp
		$(CPP) -c $*.cpp -o $*.o -Wall -std=c++11 $(CXXFLAGS)
		@$(CPP) -M $*.cpp -std=c++11 $(CXXFLAGS) > $*.d
		@mv -f $*.d $*.d.tmp
		@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
		@cp -f $*.d $*.d.tmp
		@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
			sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
		@rm -f $*.d.tmp

$(UTILDIR)/metro.o: $(UTILDIR)/metro.cpp
	$(CPP) -c $(UTILDIR)/metro.cpp -o $(UTILDIR)/metro.o -Wall -std=c++11 $(CXXFLAGS)

metro: $(UTILDIR)/metro.o
	$(CPP) $(UTILDIR)/metro.o -o metro $(CXXFLAGS) -lportaudiocpp -lportaudio

$(UTILDIR)/spect.o: $(UTILDIR)/spect.cpp
	$(CPP) -c $(UTILDIR)/spect.cpp -o $(UTILDIR)/spect.o -Wall -std=c++11 $(CXXFLAGS)

spect: $(UTILDIR)/spect.o
	$(CPP) $(UTILDIR)/spect.o -o spect $(CXXFLAGS) -lfftw3 -lsndfile -lboost_iostreams

$(UTILDIR)/onsets.o: $(UTILDIR)/onsets.cpp
	$(CPP) -c $(UTILDIR)/onsets.cpp -o $(UTILDIR)/onsets.o -Wall -std=c++11 $(CXXFLAGS)

onsets: $(UTILDIR)/onsets.o
	$(CPP) $(UTILDIR)/onsets.o -o onsets $(CXXFLAGS) -lfftw3 -lsndfile -lboost_iostreams

$(UTILDIR)/melbank.o: $(UTILDIR)/melbank.cpp
	$(CPP) -c $(UTILDIR)/melbank.cpp -o $(UTILDIR)/melbank.o -Wall -std=c++11 $(CXXFLAGS)

melbank: $(UTILDIR)/melbank.o
	$(CPP) $(UTILDIR)/melbank.o -o melbank $(CXXFLAGS) -lfftw3 -lboost_iostreams

all: tetrak metro spect onsets melbank

geany:
	geany -g -P tetrak.cpp.tags $(HEADERS)

clean:
	rm -f tetrak $(OBJS) $(OBJS:.o=.d) tetrak.cpp.tags

distclean:
	rm -f tetrak $(OBJS) $(OBJS:.o=.d) metro $(UTILDIR)/metro.o \
		spect $(UTILDIR)/spect.o onsets $(UTILDIR)/onsets.o \
		melbank $(UTILDIR)/melbank.o tetrak.cpp.tags
