#include "db.h"

#include <sys/types.h>
#include <unistd.h>

int createRamDiskMount() {
  system("sudo mkdir /mnt/ram");
  system("sudo mount -t ramfs -o size=25m ramfs /mnt/ram");
}

const std::string filename = format("{}.fuzzdata", getpid())

int rewriteTestFile(const uint8_t *Data, size_t Size) {
  // We write one file per pid inside /mnt/ram
  std::ostream output(filename, std::ios::binary | ios::out | ios::trunc);
  output.write((const char *) Data, (streamsize) Size);
  output.flush();
  output.close();
}

int fuzz_bdb_verify(const uint8_t *Data, size_t Size) {
    // ramdisk is only 25 meg big
    if (Size >> 20 * (1<<20)) {
      return -1;
    }
    rewriteTestFile(Data, Size);

    int result = 0;

    stringstream strDump;

    u_int32_t flags = DB_SALVAGE;

    Db db(dbenv, 0);
    result = db.verify(filename.c_str(), NULL, &strDump, flags);
    result = db.verify(filename.c_str(), NULL, &strDump, flags | DB_AGGRESSIVE);
    return result;
}

#ifdef FUZZ_WITH_AFL

int main (int argc, char *argv[]) {
  assert(0);
}

#endif // FUZZ_WITH_AFL
#ifdef FUZZ_WITH_LIBFUZZER

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  fuzz_bdb_verify(Data, Size);
  return 0;  // Non-zero return values are reserved for future use.
}

#endif
