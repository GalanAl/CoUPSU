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
 * UPSU protocol algorithm definitions
 ****************************************************************/

#pragma once
#include "UPSU/multipoint_eval.h"
#include "UPSU/utils.h"

using namespace std;
using namespace helib;
using namespace NTL;

void Encode(Ctxt& E_S_batch, vector<zz_pE>::const_iterator beg_S_set, const long S_size, const PubKey& pk_S);

void Reduce(Ctxt& E_eval_mask, Ctxt& E_mask, Ctxt& E_S_batch, vector<zz_pE>::const_iterator beg_R_set, const Ptxt<BGV>& mask, const long R_size, const PubKey& pk_S, const PubKey& pk_R, const Context& context);

void Map(Ctxt& E_eval, Ctxt& E_eval_Y, const Ctxt& E_eval_mask, const Ctxt& E_mask, const Ptxt<BGV>& S_batch, const SecKey& sk_S, const PubKey& pk_R, const Context& context);

void Union(vector<zz_pE>& R_set,Ctxt& E_eval, Ctxt& E_eval_Y,const SecKey& sk_R, const Context& context);

void UPSU(vector<zz_pE>& S_set, vector<zz_pE>& R_set, const Context& context, const bool detail, vector<float>& timings_R, vector<float>& timings_S);

#include "UPSU/CoUPSU_lib.inl"




