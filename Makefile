a.out: main.cc map.h circsl.h actor.h
		g++ main.cc -g -lncurses -lcurl
clean:
		rm -rf a.out core *.o
