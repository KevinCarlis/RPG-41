a.out: main.cc map.h player.h
	g++ main.cc -lncurses #-lcurl
clean:
	rm -rf a.out core *.o
