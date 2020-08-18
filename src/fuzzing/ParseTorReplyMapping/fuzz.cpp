static std::map<std::string,std::string> ParseTorReplyMapping(const std::string &s)

#ifdef FUZZ_WITH_AFL

#error "The AFL version of this fuzzer has not yet been implemented."

int main (int argc, char *argv[]) {
    // not implemented
    return 0;
}

#endif // FUZZ_WITH_AFL
#ifdef FUZZ_WITH_LIBFUZZER

size_t length_of_map = 0;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  std::string s;
  s.assign(Data, Data + Size);
  result = ParseTorReplyMapping(s);
  return 0;
}

#endif
