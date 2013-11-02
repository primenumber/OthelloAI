CPPC      = 'clang++'
CPPFLAGS  = '-O3 -std=c++11 -Wall -Wextra -Weffc++'

primeOthello :
	make -C src primeOthello CPPC=$(CPPC) CPPFLAGS=$(CPPFLAGS)
ogs :
	make -C src ogs CPPC=$(CPPC) CPPFLAGS=$(CPPFLAGS)
rndo :
	make -C src rndo CPPC=$(CPPC) CPPFLAGS=$(CPPFLAGS)
clean :
	make -C src clean

