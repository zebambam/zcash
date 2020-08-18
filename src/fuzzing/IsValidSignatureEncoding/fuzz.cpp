#include "script/interpreter.cpp"

#ifdef FUZZ_WITH_AFL

#error "The AFL version of this fuzzer has not yet been implemented."

int main (int argc, char *argv[]) {
    // not implemented
    return 0;
}

#endif // FUZZ_WITH_AFL
#ifdef FUZZ_WITH_LIBFUZZER

bool result = false;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  std::vector<unsigned char> data;
  data.assign(Data, Data + Size);
  result = IsValidSignatureEncoding(data);
  return 0;
}

#endif
