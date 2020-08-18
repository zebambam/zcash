#include <bits/stdc++.h> 
#include "coins.h"

// actual fuzzer

bool fuzz_CCoins__Unserialize (const std::vector<unsigned char> x) {
        CCoins coins; 
        CDataStream ssData(x, SER_NETWORK, PROTOCOL_VERSION);
        try {
            ssData >> coins;
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

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  std::vector<unsigned char> vect(Size);
  memcpy(&vect[0], Data, Size * sizeof(uint8_t));

  success = fuzz_CCoins__Unserialize(vect);
  return 0;  // Non-zero return values are reserved for future use.
}

#endif
