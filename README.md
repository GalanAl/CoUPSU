--------------------------------------------------------------------------------
# CoUPSU: Communication optimal Unbalanced Private Set Union using FHE
--------------------------------------------------------------------------------

**Authors**:  Jean-Guillaume Dumas, Alexis Galan, Bruno Grenet, Aude Maignan, Daniel S. Roche



**Requirements**:
- [HElib](https://github.com/homenc/HElib)
- [NTL](https://libntl.org/), dev: headers & library
- [GMP](https://gmplib.org/), dev: headers & library




**Automatic linux install**:
- Fetch and run [bin/auto-docker.run](https://raw.githubusercontent.com/GalanAl/CoUPSU/refs/heads/main/bin/auto-docker.run)

	- Requires a linux docker virtual machine or sudoer rights to install packages.
	- Will install distribution packages: `bash`, `make`, `wget`, `git`, `g++`, `cmake`, `libgmp-dev`, `libntl-dev`.
	- Then clone and install the `HElib` and `HEXL` libraries.
	- Then run one small example and a large benchmark with increasing degrees.





**Installation only**:
- Automatic (will install HElib -- and when applicable HEXL intel accelarators --)
	- Fetch and run [coupsu-auto-install.sh](https://raw.githubusercontent.com/GalanAl/CoUPSU/refs/heads/main/bin/coupsu-auto-install.sh)
		- Same requirements (git, make, cmake, c++ compiler, OpenMP, GMP/NTL --dev: headers & library--, ...) already installed.
		- Will clone and install `HElib`.
		- Will clone and install `CoUPSU`.

- By hand (supposing HElib is already installed)
	1.  Clone the CoUPSU directory and `cd CoUPSU`.
	2.  Set the HElib/NTL//GMP path within the `Makefile`.
	3.  Toggle compilation flags variants within the `Makefile`.
	4.  Compile with `make`.



**References**:
- CoUPSU: Communication optimal Unbalanced Private Set Union.
  ACNS 2025: 23rd International Conference on Applied Cryptography and Network Security,
  J-G. Dumas, A. Galan, B. Grenet, A. Maignan, D. S. Roche,
  [ArXiv 2402.16393](https://arxiv.org/abs/2402.16393)
