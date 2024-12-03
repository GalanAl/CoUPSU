// ==========================================================================
// CoUPSU: Communication Optimal Unbalanced Private Set Union
// Reference: [ACNS 2025: 23rd International Conference on Applied
// Cryptography and Network Security,
// J-G. Dumas, A. Galan, B. Grenet, A. Maignan, D. S. Roche,
// https://arxiv.org/abs/2402.16393 ]
// Authors: J-G Dumas, A. Galan, B. Grenet, A. Maignan, D. S. Roche.
//
// This software is governed by the CeCILL-B license under French law and
// abiding by the rules of distribution of free software.  You can  use,
// modify and/ or redistribute the software under the terms of the CeCILL-B
// license as circulated by CEA, CNRS and INRIA at the following URL
// "http://www.cecill.info".
// ==========================================================================

/****************************************************************
 * CoUPSU benchmark
 ****************************************************************/
#include "UPSU/CoUPSU_lib.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace std;
using namespace helib;
using namespace NTL;

int main(int argc, char* argv[])
{
// argv[1] <- (log(n)-10)/2 (between 0 and 6)
// argv[2] <- number of tests (default 3)
// argv[3] <- number of parallel tasks (default number of threads)
// argv[4] <- maximum depth allowed by the context (default 2*argv[1]+11)

  if(argc <= 1)
  {
    cerr << "Usage: " << argv[0] << " <(log(n)-10)/2> <number of tests><number of parallel tasks><max. allowed depth>" << endl;
    exit(0);
  }
  int lambda = atoi(argv[1]);
  int nb_test = (argc>2?atoi(argv[2]):3);

  int64_t numthreads(1);

  #if defined(_OPENMP)
  #pragma omp parallel
  #pragma omp single
  {
    numthreads = omp_get_num_threads();
  }
  #endif

  clog<<"[CoUPSU] Number of cores : "<<numthreads<<endl;

  omp_set_num_threads((argc>3?atoi(argv[3]):numthreads));
  clog<<"[CoUPSU] Number of tasks : "<<(argc>3?atoi(argv[3]):numthreads)<<endl;

  unsigned long m,p,bits, depth;
  depth =(argc>4? atoi(argv[4]) :2*lambda+11);
  if (2*lambda+11<=depth and depth <=23)
  {
    vector<int> liste( {380,460,520,580,640,700,760});
    m=64*1024;p=6143; bits = liste[(depth-11)/2];
  }
  else
  {
    printf("Too much depth required. Choose 0<=argv[1]<=6\n");
    exit(0);
  }

  Context context = ContextBuilder<BGV>()
                               .m(m)
                               .p(p)
                               .r(1)
                               .bits(bits)
                               .c(3)
                               .build();

  clog<<"[CoUPSU] Max depth : "<<depth<<endl;
  clog<<"[HElib ] Security : "<<context.securityLevel()<<endl;

  /* Plaintext space initialisation */
  zz_p::init((context.getPPowR()));
  zz_pE::init(to_zz_pX((*(context.getSlotRing())).G));

  clog<<"[HElib ] Ptxt space field : F_("<<p<<"^"<<context.getOrdP()<<")"<<endl;
  clog<<"[HElib ] Slots per Ctxt : "<<context.getNSlots()<<endl;

  bool detail = 1;
  vector<float> timings_R;
  vector<float> timings_S;
  vector<size_t> communications;
  long S_size = 1024;
  long R_size = pow(2,10+2*lambda);
  long size_inter;

  for(int i=0;i<nb_test;++i)
  {
    srand(time(NULL));
    size_inter = rand() % S_size;
    vector<zz_pE> S_set(S_size);
    vector<zz_pE> R_set(R_size);
    vector<zz_pE>::iterator beg_S_set= S_set.begin();
    vector<zz_pE>::iterator beg_R_set= R_set.begin();

    two_rand_sets(beg_R_set, R_size, beg_S_set, S_size, size_inter);

    UPSU(S_set,R_set,context, detail, timings_R, timings_S, communications);

    if(bool(size_inter != R_size + S_size - R_set.size())){
        cerr<<"\033[1;31m##### Correctness Failure! #####\033[0m"<<endl;exit(0);
    } else {
        clog<<"\033[1;32m[SUCCESS] |" << R_size << "| U |" << S_size
            << "| --> " << R_set.size() << ", Union OK.\033[0m"<<endl;
    }
    detail = 0;
  }
  sort(timings_R.begin(),timings_R.end());
  sort(timings_S.begin(),timings_S.end());
  size_t volume = std::accumulate(communications.begin(),
                                  communications.end(), 0);
  volume /= communications.size();


  clog<<"... After "<<nb_test<<" protocols :"<<endl;
  cout<<"[CoUPSU] Median receiver's time (s) :\t"<<timings_R[timings_R.size()/2]<<endl;
  cout<<"[CoUPSU] Communication volume (MB) :\t"<<double(volume)/1048576.0<<endl;
  cout<<"[CoUPSU] Median sender's time (s) :\t"<<timings_S[timings_S.size()/2]<<endl;

  return 0;
}
