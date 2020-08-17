#include "tinyformat.h"

#ifdef FUZZ_WITH_AFL

#error "The AFL version of this fuzzer has not yet been implemented."

int main (int argc, char *argv[]) {
    // not implemented
    return 0;
}

#endif // FUZZ_WITH_AFL
#ifdef FUZZ_WITH_LIBFUZZER

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    std::string s;
    s.assign((const char *)Data, Size);

    bool spacePadPositive = false;
    int ntrunc = 0;
    int argIndex = 0;
    double d = 0.5;
    tinyformat::detail::FormatArg fmtarg(d);
    std::ostringstream oss;

    try {
        streamStateFromFormat(oss, spacePadPositive, ntrunc, s.c_str(), &fmtarg, argIndex, 1);
    } catch (std::runtime_error& e) {
        // ignore correctly-detected parsing errors
    }

    return 0;
}

#endif
