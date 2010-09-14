/*
 * Copyright (C) 2006 International Atomic Energy Agency
 * -----------------------------------------------------------------------------
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 *-----------------------------------------------------------------------------
 *
 *   AUTHORS:
 *
 *   Roberto Capote Noy, PhD
 *   e-mail: R.CapoteNoy@iaea.org (rcapotenoy@yahoo.com)
 *   International Atomic Energy Agency
 *   Nuclear Data Section, P.O.Box 100
 *   Wagramerstrasse 5, Vienna A-1400, AUSTRIA
 *   Phone: +431-260021713; Fax: +431-26007
 *
 *   Iwan Kawrakow, PhD
 *   e-mail iwan@irs.phy.nrc.ca
 *   Ionizing Radiation Standards
 *   Institute for National Measurement Standards 
 *   National Research Council of Canada Ottawa, ON, K1A 0R6 Canada
 *   Phone: +1-613-993 2197, ext.241; Fax: +1-613-952 9865
 *
 **********************************************************************************
 * For documentation
 * see http://www-nds.iaea.org/reports-new/indc-reports/indc-nds/indc-nds-0484.pdf
 **********************************************************************************/
//
// Sources files for the interfase (not tested with event generators):
// iaea_header.cpp (iaea_header.h)
// iaea_phsp.cpp (iaea_phsp.h)
// iaea_record.cpp (iaea_record.h)
// utilities.cpp (utilities.h)
//
// Test files: test_IAEAphsp.cpp (C++) or test_IAEAphsp_f.F (FORTRAN)
//
// To compile in Linux without make 
// cc test_IAEAphsp.cpp iaea_header.cpp iaea_phsp.cpp iaea_record.cpp utilities.cpp -lm -lstdc++ -o test_IAEAphsp
// Tested in RED HAT LINUX with compilers gcc,cc,g95,icc
//
// If you have GNU make available the corresponding makefiles are provided 
// In that case the executables and DLL can be produced (for a given compiler/OS)  
// make -f Makefile_... (depends on your compiler/OS)
// make -f Makefile_Linux_g++_g77 (an example in Linux using g77/g++ compilers)
//
//
#include <cstdio>
#include "iaea_phsp.h"
//#define NUM_EXTRA_LONG 1
//#define NUM_EXTRA_FLOAT 1

int main()
{
   // Declaring IAEA phsp and access rights 
   IAEA_I32 source_read = 0, source_write = 1, source_append = 2; 
   IAEA_I32 access_read = 1, access_write = 2, access_append = 3;

   IAEA_I32 result; 

   // Particle properties
   IAEA_I32 type, extra_ints[1];
   IAEA_Float E, wt, x, y, z, u, v, w, extra_floats[1];

   // Initializing IAEA source for reading and retrieving logical information
   IAEA_I32 len = 81;
   char fiaearead[81]={'\0'};
   iaea_new_source(&source_read , fiaearead , &access_read , &result, len);

   //iaea_print_header(&source_read, &result);  
   
   IAEA_I64 histories; // Getting total number of histories

   result = -1; // To request for the total number of particles
   iaea_get_max_particles(&source_read, &result, &histories); 
   printf("\n Total number of histories: %lld\n",histories);

   // Initializing IAEA source for writing 
   char fiaeawrite[81]={'\0'};
   iaea_new_source(&source_write, fiaeawrite, &access_write, &result, len ); 

   // Changing i/o flags from the source
   IAEA_I32 iextrafloat = 0; // No ZLAST
   IAEA_I32 iextralong  = 1; // LATCH 
   iaea_set_extra_numbers(&source_write, &iextrafloat, &iextralong);

   IAEA_I32 index = 0; // corresponds to the first defined extralong variable
   IAEA_I32 extralong_type = 2; // EGS LATCH stored
   iaea_set_type_extralong_variable(&source_write, &index, &extralong_type);

   // Setting z as a constant variable
   index = 2; // corresponds to z coordenate
   IAEA_Float z_constant = 80.f;
   iaea_set_constant_variable(&source_write, &index, &z_constant); 

   // copying "source_read" header to "source_write" header
   iaea_copy_header(&source_read, &source_write, &result);

   IAEA_I64 tot_hist = histories;
   
   /* The commented block below could be used to retrieve chunks of the phsp
   
   // The starting point set at the second chunk of the phsp (i_chunk =2)
   IAEA_I32 i_parallel = 1, i_chunk = 2, n_chunk = 3;
   iaea_set_parallel(&source_read, 
            &i_parallel, &i_chunk, &n_chunk, &result);

   histories /= n_chunk; // Only histories/n_chunk number is read

   printf("\n Phase space divided in %d pieces of %d records each \n",
         n_chunk,histories);   

   printf(" Start reading at record number %i up to record number %i\n",
         (i_chunk-1)*histories,i_chunk*histories);   

   */
   
   IAEA_I32 n_stat;
   IAEA_I64 read_indep = 0, nrecorded = 0;
   IAEA_I64 irecord;

   for(irecord=0; irecord<histories; irecord++)
   {
       // read IAEA particle
       iaea_get_particle(&source_read, &n_stat,
              &type, /* particle type */
              &E   , /* kinetic energy in MeV */
              &wt  , /* statistical weight */
              &x   ,
              &y   ,
              &z   , /* position in cartesian coordinates*/
              &u   ,
              &v   ,
              &w   , /* direction in cartesian coordinates*/
              extra_floats, extra_ints);

       if( n_stat == -1 ) break; 
   
       // write IAEA particle
       iaea_write_particle(&source_write, &n_stat,
              &type, /* particle type */
              &E   , /* kinetic energy in MeV */
              &wt  , /* statistical weight */
              &x   ,
              &y   ,
              &z   , /* position in cartesian coordinates*/
              &u   ,
              &v   ,
              &w   , /* direction in cartesian coordinates*/
              extra_floats, extra_ints);

       nrecorded++;
   }

   iaea_print_header(&source_write, &result);  

   printf(" Total number of histories written %lld \n",nrecorded);

   // Closing sources
   iaea_destroy_source(&source_read,&result);
   iaea_destroy_source(&source_write,&result);

   return (1);

   //*************************************************************************
   // Appending the same test source for the second time
   //
   iaea_new_source(&source_read , fiaearead , &access_read , &result, len);

   IAEA_I64 read_original_particles;
   iaea_get_total_original_particles(&source_read, &read_original_particles); 
   iaea_new_source(&source_append, fiaeawrite, &access_append, &result, len ); 
   read_original_particles += read_original_particles;

   iaea_set_total_original_particles(&source_append, &read_original_particles); 
   result = -1; // To request for the total number of particles
   iaea_get_max_particles(&source_read, &result, &histories); 

   printf("\n Total number of histories to read: %lld\n",histories);   
   for(irecord=0; irecord<histories; irecord++)
   {
       // read IAEA particle
       iaea_get_particle(&source_read, &n_stat,
              &type, /* particle type */
              &E   , /* kinetic energy in MeV */
              &wt  , /* statistical weight */
              &x   ,
              &y   ,
              &z   , /* position in cartesian coordinates*/
              &u   ,
              &v   ,
              &w   , /* direction in cartesian coordinates*/
              extra_floats, extra_ints);

       if( n_stat == -1 ) break; 

   
       // write IAEA particle
       iaea_write_particle(&source_append, &n_stat,
              &type, /* particle type */
              &E   , /* kinetic energy in MeV */
              &wt  , /* statistical weight */
              &x   ,
              &y   ,
              &z   , /* position in cartesian coordinates*/
              &u   ,
              &v   ,
              &w   , /* direction in cartesian coordinates*/
              extra_floats, extra_ints);

       nrecorded++;
   }

   iaea_print_header(&source_append, &result);  
   iaea_get_total_original_particles(&source_read, &read_original_particles); 
   printf("\n Number of independent histories written %lld \n",read_original_particles);
   printf(" Total number of histories written %lld \n",nrecorded);

   // Closing sources
   iaea_destroy_source(&source_read,&result);
   iaea_destroy_source(&source_append,&result);
   // *************************************************************
   printf("\n\n Normal Program Termination\n");

   return(1);
}