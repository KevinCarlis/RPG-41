a.out: main.cc map.h circsl.h actor.h
		g++ main.cc -O3 -lncurses -lcurl
clean:
		rm -rf a.out core *.o
