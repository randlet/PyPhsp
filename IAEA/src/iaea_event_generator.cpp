/******************************************************************************
 *
 *  $Id: iaea_event_generator.cpp,v 1.3 2006/10/20 23:09:36 capote Exp $
 *
 *****************************************************************************/

#include "iaea_event_generator.h"

#include <string>
#include <iostream>
#include <cstring>

#ifdef WIN32

#include <windows.h>

#define DLL_HANDLE HMODULE
#define LOAD_LIBRARY(fname) LoadLibrary(fname)
#define FREE_LIBRARY(lib)   FreeLibrary(lib);
#define RESOLVE_SYMBOL(lib,symb) (void *) GetProcAddress(lib,symb)

#else

#include <dlfcn.h>

#define DLL_HANDLE void*
#define LOAD_LIBRARY(fname) dlopen(fname,RTLD_LAZY)
#define FREE_LIBRARY(lib)   !dlclose(lib)
#define RESOLVE_SYMBOL(lib,symb) dlsym(lib,symb)

#endif

#ifdef NO_STD_NAMESPACE
#else
using namespace std;
#endif

typedef void (*InitLib)(IAEA_I32 *,const char *,IAEA_I32 *,int);
typedef void (*GetFloat1)(const IAEA_I32 *,float *);
typedef void (*GetInt2)(const IAEA_I32 *,IAEA_I32 *, IAEA_I32 *);
typedef void (*GetInt64)(const IAEA_I32 *,IAEA_I64 *);
typedef void (*SetParallel)(const IAEA_I32 *,const IAEA_I32 *,const IAEA_I32 *,const IAEA_I32 *,
                            IAEA_I32 *);
typedef void (*GetParticle)(const IAEA_I32 *,IAEA_I32 *, IAEA_I32 *,
   float *,float *,float *,float *,float *,float *,float *,float *,float *,
   IAEA_I32 *);
typedef void (*Destroy)(const IAEA_I32 *,IAEA_I32 *);

class PrivateEventGenerator {

public:

    PrivateEventGenerator(const char *lib_name, const char *input_file);

    ~PrivateEventGenerator();

    void *resolve(const char *symb);
    void *tryResolve(const char *funcname);

    int         ok;
    string      libname;
    DLL_HANDLE  lib;
    IAEA_I32    source_id;

    InitLib     createSource;
    GetFloat1   getMaximumEnergy;
    GetFloat1   getMinimumEnergy;
    GetInt2     getExtraNumbers;
    GetInt2     getTypeExtraLongVariable;
    GetInt2     getTypeExtraFloatVariable;
    GetInt64    getNstat;
    //GetFloat1   getFluence;
    SetParallel setParallelRun;
    GetParticle getNextParticle;
    Destroy     destroySource;

};


IAEA_EventGenerator::IAEA_EventGenerator(const char *lib_name, 
                                         const char *input_file) {
    p = new PrivateEventGenerator(lib_name,input_file);
}

IAEA_EventGenerator::~IAEA_EventGenerator() {
    delete p;
}

int IAEA_EventGenerator::isOk() const { return p->ok; }

void IAEA_EventGenerator::getMaximumEnergy(float *ener) const {
    if( p->ok ) p->getMaximumEnergy(&p->source_id,ener);
    else *ener = -1;
}

void IAEA_EventGenerator::getMinimumEnergy(float *ener) const {
    if( p->ok ) p->getMinimumEnergy(&p->source_id,ener);
    else *ener = 1e30;
}

void IAEA_EventGenerator::getExtraNumbers(IAEA_I32 *nf, IAEA_I32 *ni) const {
    if( p->ok ) p->getExtraNumbers(&p->source_id,nf,ni);
    else { *nf = 0; *ni = 0; }
}

void IAEA_EventGenerator::getTypeExtraLongVariable(IAEA_I32 *index, 
             IAEA_I32 *type) const {
    if( p->ok ) p->getTypeExtraLongVariable(&p->source_id,index,type);
    else *type = -1;
}

void IAEA_EventGenerator::getTypeExtraFloatVariable(IAEA_I32 *index, 
             IAEA_I32 *type) const {
    if( p->ok ) p->getTypeExtraFloatVariable(&p->source_id,index,type);
    else *type = -1;
}

void IAEA_EventGenerator::getTypeExtraVariables(IAEA_I32 *index, IAEA_I32 *res,
              IAEA_I32 long_types[],IAEA_I32 float_types[]) const {
    if( !p->ok ) { *res = -1; return; }
    int nf, ni; p->getExtraNumbers(&p->source_id,&nf,&ni);
    int j;
    for(j=0; j<ni; j++) 
        p->getTypeExtraLongVariable(&p->source_id,&j,&long_types[j]);
    for(j=0; j<nf; j++) 
        p->getTypeExtraFloatVariable(&p->source_id,&j,&float_types[j]);
    *res = 0;
}

void IAEA_EventGenerator::getOriginalHistories(IAEA_I64 *nstat) const {
    if( p->ok ) p->getNstat(&p->source_id,nstat);
    else *nstat = -1;
}

/*
void IAEA_EventGenerator::getFluence(float *fluence) const {
    if( p->ok ) p->getFluence(&p->source_id,fluence);
    else *fluence = -1;
}
*/

void IAEA_EventGenerator::setParallelRun(const IAEA_I32 *i_parallel,
      const IAEA_I32 *i_chunk, const IAEA_I32 *n_chunk, IAEA_I32 *is_ok) const {
    if( p->ok ) 
        p->setParallelRun(&p->source_id,i_parallel,i_chunk,n_chunk,is_ok);
    else *is_ok = -1;
}

void IAEA_EventGenerator::getNextParticle(IAEA_I32 *n_stat,
     IAEA_I32 *type, float *E, float *wt, 
     float *x, float *y, float *z, float *u, float *v, float *w, 
     float *extra_floats, IAEA_I32 *extra_longs) const {
    if( p->ok ) p->getNextParticle(&p->source_id,n_stat,type,
                    E,wt,x,y,z,u,v,w,extra_floats,extra_longs);
    else *n_stat = -1;
}

PrivateEventGenerator::PrivateEventGenerator(const char *lib_name,
                         const char *input_file) {
    lib = 0; ok = false;
    if( !lib_name ) return;
#ifdef WIN32
    libname = lib_name;
    libname += ".dll";
#else
    string aux(lib_name); int j;
    for(j=aux.size(); j>=0; j--) {
        if( aux[j] == '/' ) break;
    }
    if( j >= 0 ) {
        for(int i=0; i<=j; i++) libname += aux[i];
        libname += "lib"; 
        for(int i=j+1; i<aux.size(); i++) libname += aux[i];
    } else { libname = "lib"; libname += aux; }
    libname += ".so";
#endif
    lib = LOAD_LIBRARY(libname.c_str());
#ifdef DEBUG
    if( !lib ) {
        cerr << "\n **** Failed to load " << libname << endl;
#ifdef WIN32
        cerr << "      Error was " << GetLastError() << endl;
#else
        cerr << "      " << dlerror() << endl;
#endif
    }
#endif
    if( !lib ) return;

    createSource = (InitLib) tryResolve("init_source");
    if( !createSource ) return;
    getMaximumEnergy = (GetFloat1) tryResolve("get_maximum_energy");
    if( !getMaximumEnergy ) return;
    getMinimumEnergy = (GetFloat1) tryResolve("get_minimum_energy");
    if( !getMinimumEnergy ) return;
    getExtraNumbers = (GetInt2) tryResolve("get_extra_numbers");
    if( !getExtraNumbers ) return;
    getTypeExtraLongVariable = 
        (GetInt2) tryResolve("get_type_extra_long_variable");
    if( !getTypeExtraLongVariable ) return;
    getTypeExtraFloatVariable =
        (GetInt2) tryResolve("get_type_extra_float_variable");
    if( !getTypeExtraFloatVariable ) return;
    getNstat = (GetInt64) tryResolve("get_nstat");
    if( !getNstat ) return;
    //getFluence = (GetFloat1) tryResolve("get_fluence");
    //if( !getFluence ) return;
    setParallelRun = (SetParallel) tryResolve("set_parallel_run");
    if( !setParallelRun ) return;
    getNextParticle = (GetParticle) tryResolve("get_next_particle");
    if( !getNextParticle ) return;
    destroySource = (Destroy) tryResolve("destroy_source");
    if( !destroySource ) return;

    IAEA_I32 result; int slen = strlen(input_file);
    createSource(&source_id,input_file,&result,slen);
    if( result ) {
#ifdef DEBUG
        cerr << "\n **** init_source function of library " << libname 
             << " failed with status " << result << endl;
#endif
        return;
    }

    ok = true;
    
}

void *PrivateEventGenerator::resolve(const char *symb) {
    if( !lib ) return 0;
    return RESOLVE_SYMBOL(lib,symb);
}

void *PrivateEventGenerator::tryResolve(const char *symb) {
    if( !lib || !symb ) return 0;
    void *result = resolve(symb); if( result ) return result;
    string funcname(symb); funcname += '_';
    result = resolve(funcname.c_str()); if( result ) return result;
    funcname += '_'; 
    result = resolve(funcname.c_str()); if( result ) return result;
    funcname = symb; 
    //transform(funcname.begin(),funcname.end(),funcname.begin(),::toupper);
	for(int j=0; j<funcname.size(); j++) funcname[j] = ::toupper(funcname[j]);
    result = resolve(funcname.c_str()); if( result ) return result;
    funcname += '_';
    result = resolve(funcname.c_str()); if( result ) return result;
    funcname += '_';
    result = resolve(funcname.c_str()); if( result ) return result;
#ifdef DEBUG
    cerr << "\n **** Failed to resolve address of function " << symb << endl;
#endif
    return 0;
}
    
PrivateEventGenerator::~PrivateEventGenerator() {
    if( lib ) {
        IAEA_I32 failed;
        destroySource(&source_id,&failed);
        if( failed ) {
#ifdef DEBUG
            cerr << "\n **** Failed to destroy source " << source_id << 
             " from library " << libname << ": error code = " << failed << endl;
#endif
        }
        int result = FREE_LIBRARY(lib);
#ifdef DEBUG
        if( !result ) cerr << "\n **** Failed to unload library " << libname
                           << endl;
#endif
    }
}
