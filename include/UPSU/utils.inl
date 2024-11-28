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
 * Utilitary function inline implementations
 ****************************************************************/

#include "UPSU/utils.h"

using namespace std;
using namespace helib;
using namespace NTL;

// Reduce the capacity of a ciphertext and fix its modulus chain to a single prime
inline void low(Ctxt& C)
{
  Ptxt<BGV> one(C.getPubKey().getContext(),ZZX(1));
  Ctxt Eone(C.getPubKey());
  (C.getPubKey()).Encrypt(Eone,one);
  while (C.capacity()>60) C*=Eone;
  C.modDownToSet(IndexSet(C.getPrimeSet().first()));
  C.cleanUp();
  return;
}

// fulfill the slots of a Ptxt
inline void batch_set(Ptxt<BGV>& batched_ptxt,vector<zz_pE>::const_iterator beg_set,const long set_size)
{
  for(long i=0;i<min(batched_ptxt.lsize(),set_size);++i) batched_ptxt[i]=to_ZZX((*(beg_set+i))._zz_pE__rep);
  return;
}

// generates a vector < X-a_1,X-a_2,...,X_a_N > from < a_1, a_2,..., a_N >
inline void to_root_poly(vector<zz_pEX>::iterator beg, vector<zz_pE>::const_iterator rootsbeg, const long size)
{
  for(long i=0;i<size;++i)
  {
    SetX(*(beg+i));
    *(beg+i) -= *(rootsbeg+i);
  } 
  return;
}

// Binary tree to compute P = (X-a_1)x...x(X-a_N)
inline void recomposed_poly(zz_pEX& P,vector<zz_pEX>::const_iterator beg, const long size)
{
  if(size==1) P=*(beg);
  else
  {
    zz_pEX left;
    recomposed_poly(left,beg,size/2);
    recomposed_poly(P,beg+size/2,size-size/2);
    P*=left;
    left.kill();
  }
  return;
}

// Compute P = (X-a_1)x...x(X-a_size) from < a_1,...,a_size >
inline void build_from_roots(zz_pEX& P, vector<zz_pE>::const_iterator beg, const long size, const Context& context)
{
  zz_p::init(context.getPPowR()); 
  zz_pE::init(to_zz_pX((*(context.getSlotRing())).G));
  vector<zz_pEX> to_roots(size);
  to_root_poly(to_roots.begin(), beg, size);
  recomposed_poly(P, to_roots.begin(),size);
  to_roots.clear();
  return;
}

// From < a_1,...,a_N >, compute numb_polys polys of degrees numb_polys
// st. P_1x...xP_sqrt(N)|(X-a_1)x...x(X-a_N)
inline void small_polys(vector<zz_pEX>::iterator beg_polys, const long numb_polys, vector<zz_pE>::const_iterator beg_R_set, const Context& context)
{
  #pragma omp parallel for shared(beg_polys,beg_R_set,numb_polys,context) 
  for(long i=0;i<numb_polys;++i) 
  { 
    build_from_roots(*(beg_polys+i), beg_R_set+i*numb_polys, numb_polys,context); 
  }
  return;
}

// Generate two random plaintext sets with a fixed intersection 
// size_inter <= size_2 <= size_1
inline void two_rand_sets(vector<zz_pE>::iterator beg_S1, const long size_1, vector<zz_pE>::iterator beg_S2, const long size_2, const long size_inter)
{
  srand(time(NULL));
  
  auto rng = std::default_random_engine {};
  long i;
  for(i=0;i<size_1;++i) *(beg_S1+i) = random_zz_pE();
  for(i=0;i<size_inter;++i) *(beg_S2+i) = *(beg_S1+i);
  for(i;i<size_2;++i) *(beg_S2+i) = random_zz_pE();
  shuffle(beg_S2,beg_S2+size_2,rng);
  return;
}


