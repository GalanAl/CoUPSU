#include "UPSU/CoUPSU_lib.h"

#include <stdio.h>   
#include <stdlib.h>    
#include <time.h>       

using namespace std;
using namespace helib;
using namespace NTL;

int main(int argc, char* argv[])
{
// argv[1] <- (log(n)-10)/2 (between 0 and 5)
// argv[2] <- number of tests (default 3)
// argv[3] <- number of parallel tasks (default number of threads)

  if(argc <= 1)
  {
    cerr << "Usage: " << argv[0] << " <(log(n)-10)/2> <number of tests><number of parallel tasks>" << endl;
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
  
  cout<<"Number of cores : "<<numthreads<<endl;
  
  omp_set_num_threads((argc>3?atoi(argv[3]):numthreads));
  cout<<"Number of tasks : "<<(argc>3?atoi(argv[3]):numthreads)<<endl;
  
  unsigned long m,p,bits;
  if (lambda <= 5)
  {
    vector<int> liste( {380,460,520,580,640,700});
    m=64*1024;p=6143; bits = liste[lambda];
  }
  else 
  {
    printf("Too much depth required. Choose 0<=argv[1]<=5\n");
    exit(0);
  }
  
  Context context = ContextBuilder<BGV>()
                               .m(m)
                               .p(p)
                               .r(1)	
                               .bits(bits)
                               .c(3)
                               .build();
                               
  cout<<"Max depth : "<<2*lambda+11<<endl;
  cout<<"Security : "<<context.securityLevel()<<endl;
  
  /* Plaintext space initialisation */
  zz_p::init((context.getPPowR())); 
  zz_pE::init(to_zz_pX((*(context.getSlotRing())).G));
  
  cout<<"Ptxt space field : F_("<<p<<"^"<<context.getOrdP()<<")"<<endl;
  cout<<"Slots per Ctxt : "<<context.getNSlots()<<endl;
  
  bool detail = 1;
  vector<float> timings_R;
  vector<float> timings_S;
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
      
    UPSU(S_set,R_set,context, detail, timings_R, timings_S);
    
    if(bool(size_inter != R_size + S_size - R_set.size())){ cout<<"Correctness Failure!"<<endl;exit(0);}  
    detail = 0;
  }
  sort(timings_R.begin(),timings_R.end());
  sort(timings_S.begin(),timings_S.end());
   
  cout<<"... After "<<nb_test<<" protocols :"<<endl;
  cout<<"Median receiver's time : "<<timings_R[timings_R.size()/2]<<endl;
  cout<<"Median sender's time : "<<timings_S[timings_S.size()/2]<<endl;
  
  return 0;
} 
