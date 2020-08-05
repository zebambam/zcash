#include "db.h"

#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <boost/format.hpp>

#include <db_cxx.h>

using boost::format;

using namespace std;

void createRamDiskMount() {
  system("sudo mkdir /mnt/ram");
  system("sudo mount -t ramfs -o size=25m ramfs /mnt/ram");
}

// We write one file per pid inside /mnt/ram
const std::string filename = std::to_string("/mnt/ram/") + std::to_string(getpid()) + std::string(".fuzzdata");

void rewriteTestFile(const uint8_t *Data, size_t Size) {
  std::ofstream ofs (filename, ios::binary, ios::out | ios::trunc );
  ofs.write((const char *) Data, (streamsize) Size);
  ofs.flush();
  ofs.close();
}

DBEnv *dbenv;

int fuzz_bdb_verify(const uint8_t *Data, size_t Size) {
  // ramdisk is only 25 meg big
  if (Size > (20 * (1<<20))) {
    return -1;
  }
  rewriteTestFile(Data, Size);

  int result = 0;

  stringstream strDump;

  u_int32_t flags = DB_SALVAGE;

  dbenv->set_lg_dir((filename + ".log").c_str())
  dbenv->set_cachesize(0, 0x100000, 1); // 1 MiB should be enough for just the wallet
  dbenv->set_lg_bsize(0x10000);
  dbenv->set_lg_max(1048576);
  dbenv->set_lk_max_locks(40000);
  dbenv->set_lk_max_objects(40000);
  dbenv->set_errfile(fopen(filename+".error").c_str(), "a")); /// debug
  dbenv->set_flags(DB_AUTO_COMMIT, 1);
  dbenv->set_flags(DB_TXN_WRITE_NOSYNC, 1);
  dbenv->log_set_config(DB_LOG_AUTO_REMOVE, 1);
  int ret = dbenv->open(filename.c_str(),
     DB_CREATE |
     DB_INIT_LOCK |
     DB_INIT_LOG |
     DB_INIT_MPOOL |
     DB_INIT_TXN |
     DB_THREAD |
     DB_RECOVER |
     nEnvFlags,
     S_IRUSR | S_IWUSR);
  if (ret != 0)
    return error("CDBEnv::Open: Error %d opening database environment: %s\n", ret, DbEnv::strerror(ret));

  Db db(dbenv, 0);
  result = db.verify(filename.c_str(), NULL, &strDump, flags);
  cout << strDump.str() << endl;
  result = db.verify(filename.c_str(), NULL, &strDump, flags | DB_AGGRESSIVE);
  cout << strDump.str() << endl;
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
