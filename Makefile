a.out: main.cc map.h actor.h
	g++ main.cc -lncurses #-lcurl
clean:
	rm -rf a.out core *.o
