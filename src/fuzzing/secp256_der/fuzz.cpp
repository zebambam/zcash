#include "pubkey.h"

#include <secp256k1.h>
#include <secp256k1_recovery.h>

#include <iostream>

using namespace std;

// this code is not thread safe
secp256k1_context *secp256k1_context_verify = NULL; 

int fuzz_secp256_der(const uint8_t *Data, size_t Size) {
  if (Size <= 0) {
    return -1;
  }

  int ret = 0;

  if (secp256k1_context_verify == NULL) {
    secp256k1_context_verify = secp256k1_context_create(SECP256K1_CONTEXT_VERIFY);
  }

  secp256k1_pubkey pubkey;
  secp256k1_ecdsa_signature sig;

  ret = secp256k1_ecdsa_signature_parse_der(secp256k1_context_verify, &sig, Data, Size);

  //secp256k1_context_destroy(secp256k1_context_verify);
  //secp256k1_context_verify = NULL;
  return 0;
}

#ifdef FUZZ_WITH_AFL

#error "The AFL version of this fuzzer has not yet been implemented."

int main (int argc, char *argv[]) {
    // not implemented
    return 0;
}

#endif // FUZZ_WITH_AFL
#ifdef FUZZ_WITH_LIBFUZZER

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  return fuzz_secp256_der(Data, Size);
}

#endif
