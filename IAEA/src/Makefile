
CXX = g++
F77 = g77

COUT = -o 
FOUT = -o 
DEFS = -DDEBUG
F77_DEFS = 
OPTCXX = -O2 -fPIC
OPTF77 = -O2
OBJE = .o
EXE = 
libpre = lib
libext = .so

cxx_sources = iaea_header iaea_phsp iaea_record utilities iaea_event_generator

CXX_RULE = $(CXX) $(DEFS) $(OPTCXX) -c $(COUT)$@ $(notdir $(basename $@)).cpp
F77_RULE = $(F77) $(F77_DEFS) $(OPTF77) -c $(FOUT)$@ $(notdir $(basename $@)).F

cxx_objects = $(addsuffix $(OBJE),$(cxx_sources))

all: $(libpre)iaea_phsp$(libext) test_iaea$(EXE) test_iaea_f$(EXE) test2$(EXE) \
     test_eg$(EXE) $(libpre)test_f$(libext) $(libpre)test_cpp$(libext)

$(libpre)iaea_phsp$(libext): $(cxx_objects)
	$(CXX) $(OPTCXX) -shared -o $@ $^ -ldl

$(libpre)test_f$(libext): example_event_generator.F example_event_generator_f.h
	$(F77) $(OPTF77) -shared -o $@ $<

$(libpre)test_cpp$(libext): example_event_generator.cpp
	$(CXX) $(OPTCXX) -shared -o $@ $<

test_iaea$(EXE): test_IAEAphsp$(OBJE) $(libpre)iaea_phsp$(libext)
	$(CXX) -L. -Wl,-rpath,. -o $@ $< -liaea_phsp

test_iaea_f$(EXE): test_IAEAphsp_f$(OBJE)
	$(F77) -L. -Wl,-rpath,. -o $@ $< -liaea_phsp

test2$(EXE): test_IAEAphsp_f$(OBJE) $(c_objects) $(cxx_objects) 
#	$(CXX) $^ -o $@ -lfrtbegin -lg2c -ldl
	$(F77) $^ -o $@ -lstdc++ -ldl

test_eg$(EXE): test_event_generator$(OBJE) $(libpre)iaea_phsp$(libext)
	$(CXX) -L. -Wl,-rpath,. -o $@ $< -liaea_phsp

iaea_header$(OBJE):   iaea_header.cpp utilities.h iaea_header.h iaea_record.h \
                      iaea_config.h
iaea_phsp$(OBJE):     iaea_phsp.cpp utilities.h iaea_record.h iaea_config.h \
                      iaea_header.h iaea_phsp.h
iaea_record$(OBJE):   iaea_record.cpp iaea_record.h utilities.h iaea_config.h
test_IAEAphsp$(OBJE): test_IAEAphsp.cpp iaea_phsp.h iaea_config.h
utilities$(OBJE):     utilities.cpp utilities.h
iaea_event_generator$(OBJE): iaea_event_generator.cpp iaea_event_generator.h \
                             iaea_config.h
test_event_generator$(OBJE): test_event_generator.cpp iaea_event_generator.h \
                             iaea_config.h

$(cxx_objects):
	$(CXX_RULE)

test_IAEAphsp$(OBJE): test_IAEAphsp.cpp iaea_phsp.h iaea_config.h
	$(CXX_RULE)

test_IAEAphsp_f$(OBJE): test_IAEAphsp_f.F
	$(F77_RULE)

test_event_generator$(OBJE): test_event_generator.cpp iaea_event_generator.h \
                             iaea_config.h
	$(CXX_RULE)
