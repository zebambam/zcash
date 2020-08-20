#include <bits/stdc++.h> 
#include "chain.h"

// actual fuzzer

bool fuzz_Deserialize (const std::vector<unsigned char> x) {
        CDiskBlockIndex obj;
        CDataStream ssData(x, SER_NETWORK, PROTOCOL_VERSION);
        try {
            ssData >> obj;
            return true;
        } catch (const std::exception&) {
            return false;
        }
}

#ifdef FUZZ_WITH_AFL

// AFL

#error "This fuzzer is not defined for AFL."

//int main (int argc, char *argv[]) { assert(0); }

#endif

#ifdef FUZZ_WITH_LIBFUZZER

// libFuzzer

bool success = false;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  std::vector<unsigned char> vect(size);
  memcpy(&vect[0], data, size * sizeof(uint8_t));

  success = fuzz_Deserialize(vect);
  return 0;  // Non-zero return values are reserved for future use.
}

#endif
