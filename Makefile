SSG5=ssg5
page_title="Hidrômetro Digital"
domain="http://192.168.4.1"
data_folder=data/
src_folder=webpage/

TEST_SRCS=test/test.c src/protocol/protocol.c
CC=gcc
TEST_BIN=test/test.bin

device=controller
port=/dev/ttyUSB0
baudrate=115200

test:
	${CC} ${TEST_SRCS} -o ${TEST_BIN}
	./${TEST_BIN}

webpage:
	${SSG5} ${src_folder} ${data_folder} ${page_title} ${domain}
	rm -fr ${data_folder}/.files ${data_folder}/sitemap.xml

compile:
	pio run -e ${device}

flash:
	pio run -e ${device} -t upload --upload-port ${port}

monitor:
	pio device monitor --baud ${baudrate} --port ${port} -f colorize

.PHONY: test webpage
