/****************************************************************
 * UPSU protocol algorithm inline implementations
 ****************************************************************/

#include "UPSU/CoUPSU_lib.h"
#include <stdio.h>   
#include <stdlib.h>    
#include <time.h>
#include <typeinfo>
using namespace std;
using namespace helib;
using namespace NTL;

inline void Encode(Ctxt& E_S_batch, vector<zz_pE>::const_iterator beg_S_set, const long S_size, const PubKey& pk_S)
{
  Ptxt<BGV> S_batch(pk_S);
  batch_set(S_batch,beg_S_set,S_size);
  pk_S.Encrypt(E_S_batch,S_batch);
  S_batch.clear();

  return;
}

inline void Reduce(Ctxt& E_eval_mask, Ctxt& E_mask, Ctxt& E_S_batch, vector<zz_pE>::const_iterator beg_R_set, const Ptxt<BGV>& mask, const long R_size, const PubKey& pk_S, const PubKey& pk_R, const Context& context)
{  

  int64_t numthreads(1);

  #if defined(_OPENMP)
  #pragma omp parallel
  #pragma omp single
  {
    numthreads = omp_get_num_threads();
  }
  #endif
  
  /* compute sqrt(R_size) polys */
  long numb_polys = lround(sqrt(R_size));
  vector<zz_pEX> polys(numb_polys);
  vector<zz_pEX>::iterator beg_polys=polys.begin();
  small_polys(beg_polys, numb_polys, beg_R_set, context);
  
  /* power vector of E_S_batch */
  vector<Ctxt> E_S_pow(numb_polys,E_S_batch);
  F_pow(E_S_pow.begin(), E_S_batch, numb_polys);
  E_S_batch.clear();
  
  /* F.Scal + F.Prod */
  PtxtArray pa(context);
  
  vector<PtxtArray> PA(numthreads,pa);
  vector<PtxtArray>::iterator beg_PA = PA.begin();
  vector<Ctxt> temp(numthreads,E_eval_mask);  
  vector<Ctxt>::iterator beg_temp = temp.begin();
  
  F_MEv(E_eval_mask,beg_PA,beg_temp,beg_polys,numb_polys,E_S_pow,min(32,numb_polys));// TODO Last Input for memory managment; 32 empirical 
  temp.clear();
  polys.clear();
  PA.clear();
  
  /* Random mask */
  pk_R.Encrypt(E_mask,mask);
  E_eval_mask+=mask;
  
  /* flood() */
  low(E_eval_mask);
  low(E_mask);
  
  return;
}

inline void Map(Ctxt& E_eval, Ctxt& E_eval_Y, const Ctxt& E_eval_mask, const Ctxt& E_mask, const Ptxt<BGV>& S_batch, const SecKey& sk_S, const PubKey& pk_R, const Context& context)
{
  Ptxt<BGV> eval_mask(context);
  sk_S.Decrypt(eval_mask,E_eval_mask);
  pk_R.Encrypt(E_eval,eval_mask);
  E_eval-=E_mask;
  E_eval_Y=E_eval;
  E_eval_Y*=S_batch;
  
  /* low() */
  low(E_eval);
  low(E_eval_Y);

}

inline void Union(vector<zz_pE>& R_set,Ctxt& E_eval, Ctxt& E_eval_Y,const SecKey& sk_R, const Context& context)
{
  Ptxt<BGV> eval(context);
  Ptxt<BGV> eval_Y(context);
  
  sk_R.Decrypt(eval,E_eval);
  sk_R.Decrypt(eval_Y,E_eval_Y);
  
  zz_p::init((context.getPPowR())); 
  zz_pE::init(to_zz_pX((*(context.getSlotRing())).G));
  long inter = 0;
  for(long i=0;i<eval.lsize();++i)
  {
    if( eval[i]!=ZZX(0)) {R_set.push_back(to_zz_pE(to_zz_pX(ZZX(eval_Y[i])))/to_zz_pE(to_zz_pX(ZZX(eval[i]))));}
  }
  
  return;
}

inline void UPSU(vector<zz_pE>& S_set, vector<zz_pE>& R_set, const Context& context, const bool detail, vector<float>& timings_R, vector<float>& timings_S) 
{
  ostringstream setup_com;
  ostringstream protocol_com;
  
  chrono::time_point<std::chrono::system_clock> start, end;
  chrono::duration<double> time_S,time_R;
  
  long S_size = S_set.size();
  long R_size = R_set.size(); 
  vector<zz_pE>::iterator beg_S_set= S_set.begin();
  vector<zz_pE>::iterator beg_R_set= R_set.begin();
  
  /*************SETUP***********/
  /* Sender's keys */
  SecKey sk_S(context);
  sk_S.GenSecKey();
  PubKey& pk_S = sk_S;
  
  pk_S.writeTo(setup_com);

  /* Receiver's keys */
  SecKey sk_R(context);
  sk_R.GenSecKey();
  PubKey& pk_R = sk_R;
  
  pk_R.writeTo(setup_com);

  /************ENCODE***********/
 
  start = chrono::system_clock::now();
  
  Ctxt E_S_batch(pk_S);
  Encode(E_S_batch,beg_S_set,S_size,pk_S);
  E_S_batch.writeTo(protocol_com);
  
  end = chrono::system_clock::now();
  time_S += end - start;
  if(detail)
  { 
    clog<<"\n ************ DETAILED TIMINGS + COMMUNICATION ****************"<<endl;
    
    clog<<"____SETUP com : "<<(setup_com.str()).length()<<endl;
    ostringstream encode_com;
    E_S_batch.writeTo(encode_com);
    chrono::duration<double> time_encode = end - start;
    clog << "____ENCODE time : " << time_encode.count() <<"s"<< endl;
    clog<<"____ENCODE com : "<<(encode_com.str()).length()<<endl;
  }
  
 

  /************REDUCE***********/
  
  start = chrono::system_clock::now();
  
  Ctxt E_eval_mask(pk_S);
  Ctxt E_mask(pk_R);
  Ptxt<BGV> mask(context);  
  mask.random();
  
  Reduce(E_eval_mask, E_mask, E_S_batch, beg_R_set, mask, R_size, pk_S, pk_R, context);
  E_eval_mask.writeTo(protocol_com);
  E_mask.writeTo(protocol_com);
  
  end = chrono::system_clock::now();
  time_R += end - start;
  if(detail)
  {
    ostringstream reduce_com;
    E_eval_mask.writeTo(reduce_com);
    E_mask.writeTo(reduce_com);
    chrono::duration<double> time_reduce = end - start;
    clog << "____REDUCE time : " << time_reduce.count() <<"s"<< endl;
    clog<<"____REDUCE com : "<<(reduce_com.str()).length()<<endl;
  }
  
  

  /*************MAP*************/
  
  start = chrono::system_clock::now();
  
  Ctxt E_eval(pk_R);
  Ctxt E_eval_Y(pk_R);
  Ptxt<BGV> S_batch(context);
  batch_set(S_batch,beg_S_set,S_size);
  
  Map(E_eval,E_eval_Y, E_eval_mask, E_mask, S_batch, sk_S, pk_R, context); 
  E_eval.writeTo(protocol_com);
  E_eval_Y.writeTo(protocol_com);
  
  end = chrono::system_clock::now();
  time_S += end - start;
  if(detail)
  { 
    ostringstream map_com;
    E_eval.writeTo(map_com);
    E_eval_Y.writeTo(map_com);
    chrono::duration<double> time_map = end - start;
    clog << "____MAP time : " << time_map.count() <<"s"<< endl;
    clog<<"____MAP com : "<<(map_com.str()).length()<<endl;
  }
  
  

  /************UNION***********/
  
  start = chrono::system_clock::now();
  
  Union(R_set, E_eval,E_eval_Y, sk_R, context);
 
  end = chrono::system_clock::now();
  time_R += end - start;
  if(detail) 
  {
    chrono::duration<double> time_union = end - start;
    clog << "____UNION time : " << time_union.count() <<"s"<< endl;
    clog<<"\n TOTAL PROTOCOL time : "<<time_S.count()+time_R.count()<<endl;
    clog<<" TOTAL PROTOCOL com : "<<(protocol_com.str()).length()<<endl;
    clog<<" **************************************************************\n"<<endl;
  }
  
  
  timings_S.push_back(time_S.count());
  timings_R.push_back(time_R.count());
  
  return;
  
}  


