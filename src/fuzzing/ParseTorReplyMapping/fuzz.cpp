#include "torcontrol.cpp"

#ifdef FUZZ_WITH_AFL

#error "The AFL version of this fuzzer has not yet been implemented."

int main (int argc, char *argv[]) {
    // not implemented
    return 0;
}

#endif // FUZZ_WITH_AFL
#ifdef FUZZ_WITH_LIBFUZZER

std::map<std::string, std::string> result;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  std::string s;
  s.assign(Data, Data + Size);
  result = ParseTorReplyMapping(s);
  return 0;
}

#endif
