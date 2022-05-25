############### BUILD ####################
CC := g++

CFLAGS := -Wall -O3 -std=c++14 -Wno-psabi
LDFLAGS := -lpthread -ljsoncpp
STATICLDFLAGS := -L./ext_libs

##########################################


############### DIR & FILES ##############
EXT_DIR := ./ext_libs
INC_DIR := ./includes/
OBJ_DIR := ./objs
SRC_DIR := ./srcs

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o , $(SRC_FILES))

##########################################

############## UNITTEST ##################
GTEST_OUTPUT := ./unittest
GTEST_DIR := ./unittest/srcs
GTEST_HEADERS := ./unittest/includes/*.h \

UNITTEST_SRC_FILES := $(wildcard $(GTEST_DIR)/*.cpp)
UNITTEST_OBJ_FILES := $(patsubst $(GTEST_DIR)/%.cpp, /%, $(UNITTEST_SRC_FILES))
UNITTEST_EXE_FILES := $(patsubst $(GTEST_DIR)/%.cpp, %, $(UNITTEST_SRC_FILES))

NONE_MAIN_OBJS := $(OBJ_DIR)/nets.o $(OBJ_DIR)/file_scouter.o $(OBJ_DIR)/session.o $(OBJ_DIR)/objects.o

##########################################

############# BUILD PROGRAM ##############
TARGET := fs_server

all : $(TARGET)

$(TARGET) : $(OBJ_FILES)
		$(CC) -o $@ $^ -I $(INC_DIR) $(LDFLAGS) $(STATICLDFLAGS)

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -I $(INC_DIR) -c -o $@ $<

##########################################


############# BUILD UNITTEST #############
test : $(UNITTEST_EXE_FILES)

$(UNITTEST_EXE_FILES): $(UNITTEST_OBJ_FILES)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ_DIR)/$@.o $(NONE_MAIN_OBJS) $(EXT_DIR)/libgtest_main.a $(EXT_DIR)/libgtest.a -o $(GTEST_OUTPUT)/$@.test

$(UNITTEST_OBJ_FILES): $(UNITTEST_SRC_FILES)
	$(CC) $(CFLAGS) -I $(INC_DIR) -c -o $(OBJ_DIR)/$@.o $(GTEST_DIR)/$@.cpp

##########################################

clean:
	rm -f $(TARGET) $(LIB) $(GTEST_OUTPUT)/*.test $(OBJ_DIR)/*.o
