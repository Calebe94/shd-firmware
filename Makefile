SSG5=ssg5
page_title="Hidrômetro Digital"
domain="http://192.168.4.1"
data_folder=data/
src_folder=webpage/

TEST_SRCS=test/test.c src/protocol/protocol.c
CC=gcc
TEST_BIN=test/test.bin

test:
	${CC} ${TEST_SRCS} -o ${TEST_BIN}
	./${TEST_BIN}

webpage:
	${SSG5} ${src_folder} ${data_folder} ${page_title} ${domain}
	rm -fr ${data_folder}/.files ${data_folder}/sitemap.xml

.PHONY: test webpage
