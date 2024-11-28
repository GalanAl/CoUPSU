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
 * Homomorphic multipoint polynomial evaluation definition
 ****************************************************************/

#pragma once
#include "UPSU/utils.h"

using namespace std;
using namespace helib;
using namespace NTL;

// From a batched ciphertext <ŷ>, generates <ŷ,ŷ^2,...,ŷ^deg>
void F_pow(vector<Ctxt>::iterator begV, const Ctxt batch_ctxt,const long deg);

//homomorphic scalar product
void E_scal(Ctxt& EvalP_i,PtxtArray& pa,zz_pEX& P_i, const long deg, vector<Ctxt>::const_iterator begC, const long size);

//homomorphic multiplication binary tree
void Emulti_low_depth(Ctxt& C, vector<Ctxt>::const_iterator begC,const long sizeC);

//homomorphic multiplipoint evaluation combining previous functions
void F_MEv(Ctxt& E_eval,vector<PtxtArray>::iterator beg_PA,vector<Ctxt>::iterator beg_temp, vector<zz_pEX>::iterator begpolys,const long sizepolys,vector<Ctxt>& Evector,const long Size);

#include "UPSU/multipoint_eval.inl"

