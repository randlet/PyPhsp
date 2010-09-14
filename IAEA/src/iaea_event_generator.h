/******************************************************************************
 *
 *  $Id: iaea_event_generator.h,v 1.3 2006/10/20 23:09:36 capote Exp $
 *
 *****************************************************************************/
#ifndef IAEA_EVENT_GENERATOR_
#define IAEA_EVENT_GENERATOR_

#include "iaea_config.h"

class PrivateEventGenerator;

class IAEA_EXPORT IAEA_EventGenerator {

public:

    /*
     *  Construct an event generator.
     *  The shared library (DLL) name is in lib_name, the generator 
     *  input file in input_file. 
     *  lib_name should contain the absolute path to the library, but no 
     *  platform specific prefixes and extensions. For instance, 
     *  if /some/path/bar is passed in lib_name, 
     *  the shared library /some/path/libbar.so will be loaded on 
     *  Unix systems, whereas the DLL loaded on Windows will be 
     *  /some/path/bar.dll.
     *  The input file name should contain absolute path.
     *
     */
    IAEA_EventGenerator(const char *lib_name, const char *input_file);

    /*  Destructor */
    ~IAEA_EventGenerator();

    int isOk() const;

    void getMaximumEnergy(float *) const;
    void getMinimumEnergy(float *) const;
    void getExtraNumbers(IAEA_I32 *n_extra_float, IAEA_I32 *n_extra_int) const;
    void getTypeExtraLongVariable(IAEA_I32 *index, IAEA_I32 *type) const;
    void getTypeExtraFloatVariable(IAEA_I32 *index, IAEA_I32 *type) const;
    void getTypeExtraVariables(IAEA_I32 *index, IAEA_I32 *result, 
                          IAEA_I32 long_types[],IAEA_I32 float_types[]) const;
    void getOriginalHistories(IAEA_I64 *nstat) const;
    // void getFluence(float *fluence) const;
    void setParallelRun(const IAEA_I32 *i_parallel,const IAEA_I32 *i_chunk,
                        const IAEA_I32 *n_chunk, IAEA_I32 *is_ok) const;
    void getNextParticle(IAEA_I32 *n_stat,IAEA_I32 *type,float *E, float *wt,
           float *x, float *y, float *z, float *u, float *v, float *w,
           float *extra_floats, IAEA_I32 *extra_longs) const;

private:

    PrivateEventGenerator *p;

};

#endif
