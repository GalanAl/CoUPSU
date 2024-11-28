######################################################
# Location of the HELIB library
# Change this with the localization of the HElib include, lib, share directories
HELIB_DIR=/usr/local/soft/HElib-2.2.1

# If needed change also NTL_DIR, and GMP_DIR
# NTL_DIR=/usr/local/soft/ntl-11.5.1
# GMP_DIR=/usr/local/soft/gmp-6.3.0

# Compilation FLAGS
OPTFLAGS=-Ofast -march=native

# UPSU FLAGS
UPSUFLAGS=-Iinclude -I../include
OBJ=CoUPSU_bentchmark

# HELIB FLAGS
HELFLAGS = -fopenmp -std=c++17
HELFLAGS += -DHELIB_BOOT_THREADS -DHELIB_THREADS

# Linking FLAGS
LIBS_NAME= helib ntl gmp
LIBS_DIR=${HELIB_DIR} ${NTL_DIR} ${GMP_DIR}

######################################################
#CXX=clang
LOADINGLIBS=${LIBS_NAME:%=-l%}
INCLUDES=${LIBS_DIR:%=-I%/include} ${LIBS_DIR:%=-I%}
LIBFLAGS=${LIBS_DIR:%=-L%/lib} ${LIBS_DIR:%=-L%} ${LIBS_DIR:%=-Wl,-rpath,%/lib}

CXXFLAGS += ${OPTFLAGS} ${HELFLAGS}  ${UPSUFLAGS}  ${INCLUDES}
LDFLAGS += ${LIBFLAGS}
LOADLIBES += ${LOADINGLIBS}

SRC=${OBJ:%=src/%.cpp}
BIN=${OBJ:%=bin/%}

######################################################
all: ${BIN}

bin/%: src/%.cpp
	$(LINK.cpp) $^ $(LOADLIBES) $(LDLIBS) -o $@

clean:
	- \rm ${BIN}
