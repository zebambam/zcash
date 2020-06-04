linux_CFLAGS=-pipe 
linux_sanitize=-fsanitize=address -fsanitize-recover=address -fno-omit-frame-pointer -fno-optimize-sibling-calls 
linux_CXXFLAGS=$(linux_CFLAGS) $(linux_sanitize)

linux_release_CFLAGS=-O1 $(linux_sanitize)
linux_release_CXXFLAGS=$(linux_release_CFLAGS)

linux_debug_CFLAGS=-O1 $(linux_sanitize) 
linux_debug_CXXFLAGS=$(linux_debug_CFLAGS)

linux_debug_CPPFLAGS=-D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC

ifeq (86,$(findstring 86,$(build_arch)))
i686_linux_CC=clang-8 -m32
i686_linux_CXX=clang++-8 -m32
i686_linux_AR=ar
i686_linux_RANLIB=ranlib
i686_linux_NM=nm
i686_linux_STRIP=strip

x86_64_linux_CC=clang-8 -m64
x86_64_linux_CXX=clang++-8 -m64
x86_64_linux_AR=ar
x86_64_linux_RANLIB=ranlib
x86_64_linux_NM=nm
x86_64_linux_STRIP=strip
else
i686_linux_CC=$(default_host_CC) -m32
i686_linux_CXX=$(default_host_CXX) -m32
x86_64_linux_CC=$(default_host_CC) -m64
x86_64_linux_CXX=$(default_host_CXX) -m64
endif
