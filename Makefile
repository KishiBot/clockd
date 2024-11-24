build:
	clang clockd.c -o main -O3

install:
	clang clockd.c -o clockd -O3
	sudo mv clockd /usr/local/bin/

clean:
	rm ./main

run:
	./main
