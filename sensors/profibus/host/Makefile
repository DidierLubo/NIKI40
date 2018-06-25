CC=arm-linux-gnueabihf-g++

CPPFLAGS = -I./ -Wall -c -std=c++17 -g2 
LDFLAGS = -lpthread
SRC_DIR = PatternMachers PacketTypes Dissector
BUILD_DIR = ./bin
OBJS = $(BUILD_DIR)/RabinKarpPatternMatcher.o $(BUILD_DIR)/SD2_Packet.o $(BUILD_DIR)/SD3_Packet.o $(BUILD_DIR)/Dissector.o $(BUILD_DIR)/main.o

vpath %.cpp $(SRC_DIR)
vpath %.h $(SRC_DIR)

.PHONY: all checkdirs clean

all: checkdirs parser

parser: $(OBJS)
	$(CC) $(OBJS) -o $(BUILD_DIR)/parser $(LDFLAGS)

$(BUILD_DIR)/RabinKarpPatternMatcher.o: PatternMachers/RabinKarpPatternMatcher.cpp	
	$(CC) $(CPPFLAGS) $< -o $(BUILD_DIR)/RabinKarpPatternMatcher.o

$(BUILD_DIR)/SD2_Packet.o: PacketTypes/SD2_Packet.cpp	
	$(CC) $(CPPFLAGS) $< -o $(BUILD_DIR)/SD2_Packet.o

$(BUILD_DIR)/SD3_Packet.o: PacketTypes/SD3_Packet.cpp	
	$(CC) $(CPPFLAGS) $< -o $(BUILD_DIR)/SD3_Packet.o

$(BUILD_DIR)/Dissector.o: Dissector/Dissector.cpp	
	$(CC) $(CPPFLAGS) $< -o $(BUILD_DIR)/Dissector.o

$(BUILD_DIR)/main.o: main.cpp	
	$(CC) $(CPPFLAGS) $< $(LDFLAGS) -o $(BUILD_DIR)/main.o	

##PROFIBUS_parser.o:PROFIBUS_parser.cpp
##	$(CC) $(CPPFLAGS) PROFIBUS_parser.cpp 

checkdirs: $(BUILD_DIR)

$(BUILD_DIR): 
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR)

