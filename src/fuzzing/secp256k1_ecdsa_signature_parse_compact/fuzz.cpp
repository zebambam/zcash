#include <secp256k1.h>

#ifdef FUZZ_WITH_AFL

#error "The AFL version of this fuzzer has not yet been implemented."

int main (int argc, char *argv[]) {
    // not implemented
    return 0;
}

#endif // FUZZ_WITH_AFL
#ifdef FUZZ_WITH_LIBFUZZER

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    static secp256k1_context *ctx = NULL;

    if (ctx == NULL) {
        ctx = secp256k1_context_create(SECP256K1_CONTEXT_VERIFY);
    }

    std::vector<uint8_t> input64(data, data + size);
    input64.resize(64, 0);

    secp256k1_ecdsa_signature sig;
    secp256k1_ecdsa_signature_parse_compact(ctx, &sig, &input64[0]);

    // ctx is cached, so we don't destroy it.
    return 0;
}

#endif
