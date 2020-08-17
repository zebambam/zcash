#include "policy/fees.h"

#ifdef FUZZ_WITH_AFL

#error "The AFL version of this fuzzer has not yet been implemented."

int main (int argc, char *argv[]) {
    return 0;
}

#endif

#ifdef FUZZ_WITH_LIBFUZZER

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    TxConfirmStats stats;

    // This initialization was copied from fees.cpp's initialization of feeStats.
    CFeeRate minTrackedFee;
    minTrackedFee = CFeeRate(MIN_FEERATE);
    std::vector<double> vfeelist;
    for (double bucketBoundary = minTrackedFee.GetFeePerK(); bucketBoundary <= MAX_FEERATE; bucketBoundary *= FEE_SPACING) {
        vfeelist.push_back(bucketBoundary);
    }
    stats.Initialize(vfeelist, MAX_BLOCK_CONFIRMS, DEFAULT_DECAY, "FeeRate");

    std::vector<uint8_t> data;
    data.assign(Data, Data + Size);
    CAutoFile filein(fmemopen(&data[0], Size, "rb"), SER_DISK, CLIENT_VERSION);

    try {
        stats.Read(filein);
    } catch (const std::runtime_error& e) {
        // ignore the exceptions it throws upon file parsing errors.
    }
    return 0;
}


#endif
