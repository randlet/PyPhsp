#include "iaea_event_generator.h"

#include <iostream>
#include <string>

using namespace std;

int main() {

    cout << "Input event generator name: "; string ename; cin >> ename;
    cout << "Input input file name: "; string fname; cin >> fname;

    IAEA_EventGenerator generator(ename.c_str(),fname.c_str());
    if( !generator.isOk() ) {
        cerr << "Error constructing the generator\n"; return 1;
    }

    float emax, emin;
    IAEA_I32 nef, nei;
    generator.getMaximumEnergy(&emax);
    generator.getMinimumEnergy(&emin);
    generator.getExtraNumbers(&nef,&nei);
    cout << "Maximum energy: " << emax << endl;
    cout << "Minimum energy: " << emin << endl;
    cout << "Extra floats: " << nef << endl;
    float *extra_floats; IAEA_I32 *extra_longs;
    if( nef > 0 ) {
        for(int j=0; j<nef; j++) {
            IAEA_I32 type;
            generator.getTypeExtraFloatVariable(&j,&type);
            cout << "   extra float " << j+1 << " is of type " << type << endl;
        }
        extra_floats = new float [nef];
    }
    if( nei > 0 ) {
        for(int j=0; j<nei; j++) {
            IAEA_I32 type;
            generator.getTypeExtraFloatVariable(&j,&type);
            cout << "   extra long " << j+1 << " is of type " << type << endl;
        }
        extra_longs = new IAEA_I32 [nei];
    }

    cout << "Now listing the first 5 particles of this generator:\n";
    
    for(int j=0; j<5; j++) {
        IAEA_I32 nstat,type;
        float E,wt,x,y,z,u,v,w;
        generator.getNextParticle(&nstat,&type,&E,&wt,&x,&y,&z,&u,&v,&w,
                                  extra_floats,extra_longs);
        cout << j+1 << ": nstat=" << nstat << " type=" << type << " E=" << E
             << " wt=" << wt << " position=(" << x << "," << y << "," << z 
             << ") direction=(" << u << "," << v << "," << w << ")\n";
        if( nef > 0 ) {
            cout << "  extra floats:";
            for(int j=0; j<nef; j++) cout << " " << extra_floats[j];
            cout << endl;
        }
        if( nei > 0 ) {
            cout << "  extra longs:";
            for(int j=0; j<nei; j++) cout << " " << extra_longs[j];
            cout << endl;
        }
    }

    return 0;

}
