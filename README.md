--------------------------------------------------------------------------------
# CoUPSU: Communication optimal Unbalanced Private Set Union using FHE
--------------------------------------------------------------------------------

**Authors**:  Jean-Guillaume Dumas, Alexis Galan, Bruno Grenet, Aude Maignan, Daniel S. Roche

**Requirements**:
- [HElib](https://github.com/homenc/HElib)
- [NTL](https://libntl.org/), dev: headers & library
- [GMP](https://gmplib.org/), dev: headers & library

## Automatic linux install and benchmarking

- Requirements: `bash`, `make`, `wget`, `git`, `g++`, `cmake`, `libgmp-dev`, `libntl-dev`.
- Installation: 
    - Download [coupsu-auto-install.sh](https://raw.githubusercontent.com/GalanAl/CoUPSU/refs/heads/main/bin/coupsu-auto-install.sh)
    - Run it: `sh coupsu-auto-install.sh`
	    - Will clone and install `HElib`.
	    - Will clone and install `CoUPSU`.
- Benchmarking: 
    - `cd libcoupsu/CoUPSU`
    - `./bin/CoUPSU_benchmark 0 5 4`

## Docker install 

- Fetch and run [bin/auto-docker.run](https://raw.githubusercontent.com/GalanAl/CoUPSU/refs/heads/main/bin/auto-docker.run)

	- Requires a linux docker virtual machine or sudoer rights to install packages.
	- Will install distribution packages: `bash`, `make`, `wget`, `git`, `g++`, `cmake`, `libgmp-dev`, `libntl-dev`.
	- Then clone and install the `HElib` and `HEXL` libraries.
    - Then run two small examples.

## Install by hand (supposing HElib is already installed)
1.  Clone the CoUPSU directory and `cd CoUPSU`.
2.  Set the HElib/NTL/GMP path within the `Makefile`.
3.  Toggle compilation flags variants within the `Makefile`.
4.  Compile with `make`.


## Benchmarking
- `cd CoUPSU`
- Run executable `./CoUPSU_benchmark`
	- Usage: `CoUPSU_benchmark [(log(Receiver's set size)-10)/2] [number of tests (default=3)] [number of parallel tasks (default=threads)]`
	- Example: `./CoUPSU_benchmark 0 5 4`




## Results
- Context description according to Receiver's set size:
  - max. multiplicative depth.
  - security estimation by HElib.
  - plaintext space.
  - size of a batched ciphertext.
    
- Detailed timings and communications for each algorithms in the UPSU protocol:
  - "SETUP": public keys exchanges.
  - "ENCODE": Sender encodes, batch and send its set.
  - "REDUCE": Receiver evaluates homomorphically its set in the received ciphertext, and sends the result masked.
  - "MAP": Sender homomorphically remove the mask and sends the result.
  - "UNION": Receiver computes the set union.

- Final results after [number of tests]:
  - Median time computation of the receiver.
  - Median time computation of the sender.
  - (Remark: the communication volume is constant according to a receiver's set size).




## References
- CoUPSU: Communication optimal Unbalanced Private Set Union.
  ACNS 2025: 23rd International Conference on Applied Cryptography and Network Security,
  J-G. Dumas, A. Galan, B. Grenet, A. Maignan, D. S. Roche,
  [ArXiv 2402.16393](https://arxiv.org/abs/2402.16393)
