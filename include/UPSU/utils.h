/****************************************************************
 * Utilitary function definitions
 ****************************************************************/

#pragma once
#include <helib/helib.h>
#include <NTL/ZZX.h>
#include <NTL/ZZ_pXFactoring.h>
#include <NTL/ZZ_pEX.h>
#include <vector>
#include "omp.h"
#include <algorithm>
#include <random>

using namespace std;
using namespace helib;
using namespace NTL;

// Reduce the capacity of a ciphertext and fix its modulus chain to a single prime
void low(Ctxt& C);

// fulfill the slots of a Ptxt
void batch_set(Ptxt<BGV>& batched_ptxt,vector<zz_pE>::const_iterator beg_set,const long set_size);

// generates a vector < X-a_1,X-a_2,...,X_a_N > from < a_1, a_2,..., a_N >
void to_root_poly(vector<zz_pEX>::iterator beg, vector<zz_pE>::const_iterator rootsbeg, const long size);

// Binary tree to compute P = (X-a_1)x...x(X-a_N)
void recomposed_poly(zz_pEX& P,vector<zz_pEX>::const_iterator beg, const long size);

// Compute P = (X-a_1)x...x(X-a_N) from < a_1,...,a_N >
void build_from_roots(zz_pEX& P, vector<zz_pE>::const_iterator beg, const long size, const Context& context);

// From < a_1,...,a_N >, compute numb_polys polys of degrees numb_polys
// st. P_1x...xP_sqrt(N)|(X-a_1)x...x(X-a_N)
void small_polys(vector<zz_pEX>::iterator beg_polys, const long numb_polys, vector<zz_pE>::const_iterator beg_R_set, const Context& context);

// Generate two random plaintext sets with a fixed intersection
// size_inter <= size_2 <= size_1
void two_rand_sets(vector<zz_pE>::iterator beg_S1, const long size_1, vector<zz_pE>::iterator beg_S2, const long size_2, const long size_inter);


#include "UPSU/utils.inl"
