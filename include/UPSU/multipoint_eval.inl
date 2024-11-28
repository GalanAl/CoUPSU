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

/*************************************************************************
 * Homomorphic multipoint polynomial evaluation inline implementations
 *************************************************************************/

#include "UPSU/multipoint_eval.h"

using namespace std;
using namespace helib;
using namespace NTL;

// From a batched ciphertext <ŷ>, generates <ŷ,ŷ^2,...,ŷ^deg>
inline void F_pow(vector<Ctxt>::iterator begV, const Ctxt batch_ctxt,const long deg)
{
  long i =1;
  *(begV) = batch_ctxt;
  while (i<deg)
  {
    #pragma omp parallel for shared(begV)
    for(long k=0;k<min(i,deg-i);++k)
    {
      *(begV+i+k) = *(begV+i-1);
      *(begV+i+k) *= *(begV+k);
    }
    i*=2;
  }
  return;
}
                                               
//homomorphic scalar product
inline void E_scal(Ctxt& EvalP_i,PtxtArray& pa,zz_pEX& P_i, const long deg, vector<Ctxt>::const_iterator begC, const long size)
{
  Ctxt accu = EvalP_i;
  EvalP_i.clear();
  for(long i = 0;i<size;++i)
  { 
    accu=*(begC+i);
    pa.load(to_ZZX(coeff(P_i,deg+i)._zz_pE__rep));// TODO Do that outside 
    accu *= pa;
    EvalP_i+=accu;
  }
  accu.clear();
  return;
}

//homomorphic multiplication binary tree
inline void Emulti_low_depth(Ctxt& C, vector<Ctxt>::const_iterator begC,const long sizeC)
{
  if (sizeC==1) C = *(begC);
  else
  {
	  Ctxt A=C;
	  Emulti_low_depth(A,begC,sizeC/2);
	  Emulti_low_depth(C,begC+sizeC/2,sizeC-sizeC/2);
	  C*=A;
	  A.clear();  
  }
  return;
}                                                                                                                                                                                
 
//homomorphic multiplipoint evaluation combining previous functions
inline void F_MEv(Ctxt& E_eval,vector<PtxtArray>::iterator beg_PA,vector<Ctxt>::iterator beg_temp, vector<zz_pEX>::iterator begpolys, const long sizepolys, vector<Ctxt>& Evector, const long Size) // TODO Input Size for memory managment; empirical 
{
  vector<Ctxt> All_ev(sizepolys,E_eval);
  vector<Ctxt>::iterator begAll_ev = All_ev.begin();

  vector<Ctxt>::iterator begEvector = Evector.begin();
  
  #pragma omp parallel for shared(begpolys,begAll_ev,beg_PA)
  for(long i=0;i<sizepolys;++i)
  {
    (*(beg_PA+omp_get_thread_num())).load(to_ZZX(coeff(*(begpolys+i),0)._zz_pE__rep));
    *(begAll_ev+i)+=(*(beg_PA+omp_get_thread_num()));
  }
  long l;
  long deg = 0;
  while(deg<long( Evector.size()))
  {
    l = min(Size,long(Evector.size())-deg);
    #pragma omp parallel for shared(begpolys,begAll_ev,begEvector,l,deg,beg_PA,beg_temp)
    for(long i=0;i<sizepolys;++i)
    {
      E_scal(*(beg_temp+omp_get_thread_num()),*(beg_PA+omp_get_thread_num()),*(begpolys+i),deg+1,begEvector+deg,l);
      *(begAll_ev+i)+=(*(beg_temp+omp_get_thread_num()));
    }
    deg+= Size;
  }
  Evector.clear();
  int64_t numthreads(1);

  #if defined(_OPENMP)
  #pragma omp parallel
  #pragma omp single
  {
    numthreads = omp_get_num_threads();
  }
  #endif
  
  long pow_2 =1;
  while(2*pow_2<numthreads) pow_2*=2;
  
  long cut_size=All_ev.size()/pow_2;
  #pragma omp parallel for shared(beg_temp,begAll_ev,cut_size)
  for(long i=0;i< pow_2;++i)
  {
    (*(beg_temp+omp_get_thread_num())).clear();
    Emulti_low_depth(*(beg_temp+omp_get_thread_num()),begAll_ev+i*cut_size,cut_size);
  }
  All_ev.clear();
  
  Emulti_low_depth(E_eval,beg_temp,pow_2);
  return;
}


