/******************************************************************************
 *
 *  $Id: example_event_generator.cpp,v 1.1 2006/10/19 15:20:43 capote Exp $
 *
 *  This is an example event generator written in C++.
 *  It provides a mono-energetic pencil beam with particle type, energy and
 *  position defined in an input file.
 *  This event generator can be loaded several times. 
 *  This is implemented to demonstrate the idea that each event generator 
 *  shared library (DLL) may be used several times in a run.
 *
 *****************************************************************************/

#include "iaea_config.h"

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class MySimplePencilBeam {

public:

    MySimplePencilBeam(const char *input_file);
    ~MySimplePencilBeam() {};

    bool  isOk() const { return ok; };
    float getMaximumEnergy() const { return e; };
    float getMinimumEnergy() const { return e; };
    void  getExtraNumbers(IAEA_I32 *nf, IAEA_I32 *ni) const {
                          *nf = 0; *ni = 0; };
    int   getTypeExtraLongVariables(IAEA_I32 index) const { return -1; };
    int   getTypeExtraFloatVariables(IAEA_I32 index) const { return -1; };
    IAEA_I64 getNstat() const { return count; };
    void  setParallelRun(const IAEA_I32 *,const IAEA_I32 *,const IAEA_I32 *,
           IAEA_I32 *is_ok) const { *is_ok = 0; };
    void  getNextParticle(IAEA_I32 *n_stat, IAEA_I32 *type, float *E,
           float *wt, float *x, float *y, float *z, float *u, float *v, 
           float *w, float *extra_floats, IAEA_I32 *extra_longs) {
            *n_stat = 1; *type = q; *E = e; *wt = 1; 
            *x = xo; *y = yo; *z = zo; *u = 0; *v = 0; *w = 1;
          };

private:

    int   q;
    float e;
    IAEA_I64 count;
    float xo,yo,zo;
    bool  ok;

};

MySimplePencilBeam::MySimplePencilBeam(const char *input_file) : 
           count(0), ok(false) {
    ifstream in(input_file); if( !in ) return;
    in >> q >> e >> xo >> yo >> zo;
    if( in.good() && !in.fail() ) ok = true;
}

#ifdef WIN32

#define MY_EXPORT __declspec(dllexport)
#define MY_LOCAL

#else

#ifdef HAVE_VISIBILITY
#define MY_EXPORT __attribute__ ((visibility ("default")))
#define MY_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define MY_EXPORT
#define MY_LOCAL
#endif

#endif

static MY_LOCAL vector<MySimplePencilBeam *> sources;

extern "C" {

MY_EXPORT void init_source(IAEA_I32 *id,const char *inpfile,IAEA_I32 *result,
    int ifl) {
    MySimplePencilBeam *new_source = new MySimplePencilBeam(inpfile);
    int j;
    for(j=0; j<sources.size(); j++) {
        if( !sources[j] ) break;
    }
    if( j < sources.size() ) { sources[j] = new_source; *id = j; }
    else { *id = sources.size(); sources.push_back(new_source); }
    if( new_source->isOk() ) *result = 0; else *result = -1;
}

MY_EXPORT void get_maximum_energy(const IAEA_I32 *id, float *energy) {
    if( *id >= 0 && *id < sources.size() ) 
        *energy = sources[*id]->getMaximumEnergy();
}

MY_EXPORT void get_minimum_energy(const IAEA_I32 *id, float *energy) {
    if( *id >= 0 && *id < sources.size() ) 
        *energy = sources[*id]->getMinimumEnergy();
}

MY_EXPORT void get_extra_numbers(const IAEA_I32 *id, int *ni, int *nf) {
    if( *id >= 0 && *id < sources.size() ) sources[*id]->getExtraNumbers(ni,nf);
    else { *ni = 0; *nf = 0; }
}

MY_EXPORT void get_type_extra_long_variable(const IAEA_I32 *id, const int *ind,
                                            int *typ) {
    if( *id >= 0 && *id < sources.size() ) 
        *typ = sources[*id]->getTypeExtraLongVariables(*ind);
    else *typ = -1;
}

MY_EXPORT void get_type_extra_float_variable(const IAEA_I32 *id, const int *ind,
                                             int *typ) {
    if( *id >= 0 && *id < sources.size() ) 
        *typ = sources[*id]->getTypeExtraFloatVariables(*ind);
    else *typ = -1;
}

MY_EXPORT void get_nstat(const IAEA_I32 *id, IAEA_I64 *nstat) {
    if( *id >= 0 && *id < sources.size() )
        *nstat = sources[*id]->getNstat();
    else *nstat = -1;
}

MY_EXPORT void set_parallel_run(const IAEA_I32 *id, const IAEA_I32 *iparallel,
           const IAEA_I32 *ichunk, const IAEA_I32 *nchunk, IAEA_I32 *result) {
    if( *id >= 0 && *id < sources.size() ) 
        sources[*id]->setParallelRun(iparallel,ichunk,nchunk,result);
    else *result = -1;
}

MY_EXPORT void get_next_particle(const IAEA_I32 *id,IAEA_I32 *n_stat, 
           IAEA_I32 *type, float *E, float *wt, 
           float *x, float *y, float *z, float *u, float *v,
           float *w, float *extra_floats, IAEA_I32 *extra_longs) {
    if( *id >= 0 && *id < sources.size() )
        sources[*id]->getNextParticle(n_stat,type,E,wt,x,y,z,u,v,w,
                                      extra_floats,extra_longs);
    else *n_stat = -1;
}

MY_EXPORT void destroy_source(const IAEA_I32 *id,IAEA_I32 *result) {
    if( *id >= 0 && *id < sources.size() ) {
        if( sources[*id] ) { delete sources[*id]; sources[*id] = 0; *result=0;}
    } else *result = -1;
}

}
