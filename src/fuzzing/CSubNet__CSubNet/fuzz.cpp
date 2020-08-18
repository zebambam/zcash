#include "netbase.h"

#ifdef FUZZ_WITH_AFL

#error "The AFL version of this fuzzer has not yet been implemented."

int main (int argc, char *argv[]) {
    // not implemented
    return 0;
}

#endif // FUZZ_WITH_AFL
#ifdef FUZZ_WITH_LIBFUZZER

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {

    std::string subnet(data, data + size);
    CSubNet fuzz(subnet, false);

    // ctx is cached, so we don't destroy it.
    return 0;
}

#endif
