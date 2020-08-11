CCFLAGS=-std=gnu++11 -pedantic -Wall -Werror -ggdb3
proxy: parsing.o proxy.o Cacher.o LogPrinter.o
	g++ -pthread -o proxy $(CCFLAGS) parsing.o proxy.o Cacher.o LogPrinter.o

proxy.o: proxy.cpp proxy.h parsing.h Cacher.h LogPrinter.h
	g++ -c $(CCFLAGS) $<
cache.o: Cacher.cpp Cacher.h LRUCache.h
	g++ -c $(CCFLAGS) $<
parsing.o: parsing.cpp parsing.h
	g++ -c $(CCFLAGS) $<
LogPrinter.o: LogPrinter.cpp LogPrinter.h
	g++ -c $(CCFLAGS) $<
clean:
	rm -f *.o  *~ proxy
