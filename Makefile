SSG5=ssg5
page_title="Hidrômetro Digital"
domain="http://192.168.4.1"
data_folder=data/
src_folder=webpage/
sim_folder=web_sim/

TEST_SRCS=test/test.cpp src/protocol/protocol.cpp
CC=g++
TEST_BIN=test/test.bin

device=controller
port=/dev/ttyUSB0
baudrate=115200

${sim_folder}:
	mkdir -p ${sim_folder}

test:
	${CC} ${TEST_SRCS} -o ${TEST_BIN}
	./${TEST_BIN}

webpage:
	${SSG5} ${src_folder} ${data_folder} ${page_title} ${domain}
	rm -fr ${data_folder}/.files ${data_folder}/sitemap.xml ${data_folder}/routes_sim.py

compile:
	pio run -e ${device}

flash:
	pio run -e ${device} -t upload --upload-port ${port}

monitor:
	pio device monitor --baud ${baudrate} --port ${port} -f colorize

partition: webpage
	pio run -e ${device} -t uploadfs --upload-port ${port}

sim: ${sim_folder}
	${SSG5} ${src_folder} ${sim_folder} ${page_title} ${domain}
	cp ${data_folder}/*.json ${sim_folder}/
	python ${sim_folder}/routes_sim.py

.PHONY: test webpage sim
