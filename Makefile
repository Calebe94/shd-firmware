test:
	gcc test/test.c src/protocol/protocol.c -o test/test.bin
	./test/test.bin

.PHONY: test
