#makefile for those who have many excutable files

.PHONY: all strip clean sync install mem_check
.SUFFIXES: .sh .h .hpp .c .cc .cpp

OSNAME      := $(shell uname)

CC          = gcc
CFLAGS      = -O0 -ggdb3
CX          = g++
# CXFLAGS     = -O0 -g -ggdb3
CFLAGS     = -O3 -Werror -std=c++11

TAR         = ar
TARFLAGS    = crsuv

# SO_FLAGS    = -shared -fPIC
SO_FLAGS    = -shared

INC_DIR     = ./export/include
SRC_DIR     = ./src
EXAM_DIR    = ./exam
OBJ_DIR     = ./obj
LIB_DIR     = ./export/lib
BIN_DIR     = ./bin
VERSION     = 1.0.0
LIB_BIN     = libuds$(VERSION).a
LIB_SO      = libuds$(VERSION).so
BIN         = client server

INC         = -I$(INC_DIR)
LIB         = -lpthread -L$(LIB_DIR) -luds$(VERSION)

ifeq ($(OSNAME), Darwin)
#    LIB     += -L./third/protobuf/mac_lib -lprotobuf
endif
ifeq ($(OSNAME), Linux)
#    LIB     += -Wl,-rpath,./third/protobuf/linux_lib -L./third/protobuf/linux_lib -lprotobuf
endif


all: dir $(addprefix $(LIB_DIR)/,$(LIB_BIN)) $(addprefix $(BIN_DIR)/,$(BIN))

#@ do not outoupt this command - ingnore its error and continue to make
dir:
	@-mkdir -p $(OBJ_DIR)
	@-mkdir -p $(LIB_DIR)
	@-mkdir -p $(BIN_DIR)

$(OBJ_DIR)/%.o:$(SRC_DIR)/%.cpp
	$(CX) $(CXFLAGS) $(INC) -o $@ -c $<

$(OBJ_DIR)/%.o:$(EXAM_DIR)/%.cpp
	$(CX) $(CXFLAGS) $(INC) -o $@ -c $<

$(LIB_DIR)/$(LIB_BIN):$(addprefix $(OBJ_DIR)/, uds_datagram_client.o uds_datagram.o uds_datagram_server.o)
	$(TAR) $(TARFLAGS) $@ $^

# $(LIB_DIR)/$(LIB_SO):$(addprefix $(OBJ_DIR)/, )
# 	$(CX) $(CXFLAGS) $(SO_FLAGS) -o $@ $^

$(BIN_DIR)/client:$(addprefix $(OBJ_DIR)/, client.o)
	$(CX) $(CXFLAGS) -o $@ $^ $(LIB)

$(BIN_DIR)/server:$(addprefix $(OBJ_DIR)/, server.o)
	$(CX) $(CXFLAGS) -o $@ $^ $(LIB)

strip:
	strip $(addprefix $(BIN_DIR)/,$(BIN))

clean:
	@rm -f $(addprefix $(BIN_DIR)/,$(BIN)) $(OBJ_DIR)/*.o ./valgrind_info
	@rm -rf $(BIN_DIR) $(OBJ_DIR) $(LIB_DIR)

mem_check:
	valgrind --log-file=valgrind_info --track-origins=yes --leak-check=full --show-reachable=yes -v bin/tellist_fb
