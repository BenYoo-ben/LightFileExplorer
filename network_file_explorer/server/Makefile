############### BUILD ####################
CC = g++

COMPILE_RELEASE := -Wall -O3 -std=c++14 -Wno-psabi

COMPILE_DEBUG := -Wall -std=c++14 -Wno-psabi -fprofile-arcs -ftest-coverage

LDFLAGS := -lpthread -ljsoncpp
LDFLAGS_RELEASE := -lpthread -ljsoncpp


STATICLDFLAGS := -L./ext_libs

##########################################


############### DIR & FILES ##############
EXT_DIR := ./ext_libs
INC_DIR := ./includes/
OBJ_DIR_DEBUG := ./objs
OBJ_DIR_RELEASE := ./objs_release
SRC_DIR := ./srcs

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES_DEBUG := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR_DEBUG)/%.o , $(SRC_FILES))
OBJ_FILES_RELEASE := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR_RELEASE)/%.o , $(SRC_FILES))

##########################################


############## UNITTEST ##################
GTEST_OUTPUT := ./unittest
GTEST_DIR := ./unittest/srcs
GTEST_HEADERS := ./unittest/includes/*.h \

UNITTEST_SRC_FILES := $(wildcard $(GTEST_DIR)/*.cpp)
UNITTEST_OBJ_FILES := $(patsubst $(GTEST_DIR)/%.cpp, $(OBJ_DIR_DEBUG)/%.o, $(UNITTEST_SRC_FILES))


NONE_MAIN_OBJS := $(OBJ_DIR_DEBUG)/nets.o $(OBJ_DIR_DEBUG)/file_scouter.o $(OBJ_DIR_DEBUG)/session.o $(OBJ_DIR_DEBUG)/objects.o

##########################################


############# BUILD PROGRAM ##############
TARGET := fs_server

debug : $(TARGET)

$(TARGET) : $(OBJ_FILES_DEBUG)
		$(CC) -o $@ $^ -I $(INC_DIR) -lgcov $(LDFLAGS) $(STATICLDFLAGS)

$(OBJ_DIR_DEBUG)/%.o : $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR_DEBUG)
	$(CC) $(COMPILE_DEBUG) -lgcov -I $(INC_DIR) -c -o $@ $<

##########################################


############# BUILD RELEASE ##############
RELEASE := fs_server.$(shell date '+%Y%m%d')

release : $(RELEASE)

$(RELEASE) : $(OBJ_FILES_RELEASE)
		$(CC) -o $@ $^ -I $(INC_DIR) $(LDFLAGS_RELEASE) $(STATICLDFLAGS)

$(OBJ_DIR_RELEASE)/%.o : $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR_RELEASE)
	$(CC) $(COMPILE_RELEASE) -I $(INC_DIR) -c -o $@ $<

##########################################


############# BUILD UNITTEST #############
TEST := TEST
test : $(TEST)

$(TEST): $(UNITTEST_OBJ_FILES)
	$(CC) $(COMPILE_DEBUG) $(LDFLAGS) $^ $(NONE_MAIN_OBJS) $(EXT_DIR)/libgtest_main.a $(EXT_DIR)/libgtest.a -o $@

$(OBJ_DIR_DEBUG)/%.o: $(GTEST_DIR)/%.cpp
	$(CC) $(COMPILE_DEBUG) -lgcov -I $(INC_DIR) -c -o $@ $<

##########################################

clean:
	rm -f $(TARGET) $(LIB) $(GTEST_OUTPUT)/*.test $(OBJ_DIR_DEBUG)/*.o $(OBJ_DIR_RELEASE)/*.o ./*.gcov $(OBJ_DIR_DEBUG)/*.gcov
