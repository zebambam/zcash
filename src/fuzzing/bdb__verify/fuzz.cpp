#include "db.h"

#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <boost/format.hpp>
#include <cstdio>

#include <db_cxx.h>

using boost::format;

using namespace std;

const std::string bdbhomedir = std::string("/mnt/ram/") + std::to_string(getpid()) + std::string(".bdb.homedir");
const std::string bdbfuzzdata = bdbhomedir + "/db.fuzz.data";
const std::string bdblogdir = bdbhomedir + "/log.out";

bool mounted = false;

void sys(std::string cmd) {
  (void)system((const char *)cmd.c_str());
}

void createRamDiskMount() {
  sys("sudo mkdir /mnt/ram");
  sys("sudo mount -t ramfs -o size=25m ramfs /mnt/ram");
  sys("sudo chown -R " + std::to_string(getuid()) + " /mnt/ram");
}

void makeDbHomeDir() {
  sys("mkdir " + bdbhomedir);
  sys("mkdir " + bdblogdir);
}

void removeDbHomeDir() {
  sys("rm -rf " + bdbhomedir);
}

void rewriteTestFile(const uint8_t *Data, size_t Size) {
  std::ofstream ofs;
  ofs.open(bdbfuzzdata, ios::binary | ios::out | ios::trunc );
  ofs.write((const char *) Data, Size);
  ofs.flush();
  ofs.close();
}

DbEnv *dbenv;

void fuzz_bdb_verify(const uint8_t *Data, size_t Size) {
  if (!mounted) {
    createRamDiskMount();
    makeDbHomeDir();
    mounted = true;
  }

  dbenv = new DbEnv(DB_CXX_NO_EXCEPTIONS); // we want a new one of these each time...

  if (Size > (20 * (1<<20))) { // ramdisk has size 25 meg, don't try to write more than 20
    return;
  }

  rewriteTestFile(Data, Size);

  stringstream strDump;

  u_int32_t flags = DB_SALVAGE;

  dbenv->set_lg_dir(bdblogdir.c_str());
  dbenv->set_cachesize(0, 0x100000, 1); // 1 MiB should be enough for just the wallet
  dbenv->set_lg_bsize(0x10000);
  dbenv->set_lg_max(1048576);
  dbenv->set_lk_max_locks(40000);
  dbenv->set_lk_max_objects(40000);
  dbenv->set_errfile(fopen((bdbhomedir+"/log.error").c_str(), "a")); /// debug
  dbenv->set_flags(DB_AUTO_COMMIT, 1);
  dbenv->set_flags(DB_TXN_WRITE_NOSYNC, 1);
  dbenv->log_set_config(DB_LOG_AUTO_REMOVE, 1);
  int ret = dbenv->open(bdbhomedir.c_str(),
     DB_CREATE |
     DB_INIT_LOCK |
     DB_INIT_LOG |
     DB_INIT_MPOOL |
     DB_INIT_TXN |
     DB_THREAD |
     DB_RECOVER |
     DB_PRIVATE,
     S_IRUSR | S_IWUSR);
  if (ret != 0)
    cout << "CDBEnv::Open: Error " << ret << " opening database environment: " << DbEnv::strerror(ret) << endl;

  Db db(dbenv, 0);
  db.verify(bdbfuzzdata.c_str(), NULL, &strDump, flags);
  if (strDump.str().length() > 0) {
    cout << strDump.str();
  }
  db.verify(bdbfuzzdata.c_str(), NULL, &strDump, flags | DB_AGGRESSIVE);
  if (strDump.str().length() > 0) {
    cout << strDump.str();
  }
  dbenv->close(0);
  delete dbenv;
}

#ifdef FUZZ_WITH_AFL

#error This fuzzer is not implemented for AFL.

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
