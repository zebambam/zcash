#include <bits/stdc++.h> 
#include "coins.h"

// actual fuzzer

bool fuzz_Deserialize (const std::vector<unsigned char> x) {
        // The inputs are actually a container format that beings with two ints,
        // the type and version of the stream used to serialize the object.
        int type;
        int version;
        try {
            CDataStream s(x, 0, 0);
            s >> type;
            s >> version;
        } catch (const std::exception&) {
            return false;
        }

        CCoins obj;
        CDataStream ssData(x, type, version);
        try {
            // These were already deserialized but we need to do it again
            // to get to the start of the actual serialized object.
            ssData >> type;
            ssData >> version;
            // Now deserialize the actual object.
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
