rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
BIN_DIR=bin
OUT=output
SRC_DIR=src
SRC_FILES=$(call rwildcard,$(SRC_DIR)/,*.c)
SRC=$(SRC_FILES:$(SRC_DIR)/%=%)
OBJ_DIR=obj
OBJ=$(SRC:.c=.o)
CC=clang
LD=$(CC)
CFLAGS=-Wall -Werror -Wextra -pedantic --std=c99 `pkg-config --cflags gtk+-2.0`
SDL_FLAGS=`sdl-config --libs` `pkg-config --libs SDL_image`
GTK_FLAGS=`pkg-config --libs gtk+-2.0`
LDFLAGS= $(CFLAGS) $(SDL_FLAGS) $(GTK_FLAGS) -lm -pthread
FLAGS=$(CFLAGS)
DEBUG_FLAGS=-g3 -ggdb
DBG=debug
RELEASE_FLAGS=-O2
REL=release
INCLUDE_DIR=includes
RM=rm -rf
MKDIR=mkdir -p

all: $(DBG) | $(REL)

$(DBG): MODE=$(DBG)
$(DBG): intern-compile-$(DBG)

$(REL): MODE=$(REL)
$(REL): intern-compile-$(REL)

$(DBG)/%.o: $(SRC_DIR)/%.c
	@$(MKDIR) $(OBJ_DIR)/$(dir $(@))
	@echo "build $(@)"
	@$(CC) -c $< -o $(OBJ_DIR)/$@ $(FLAGS) $(DEBUG_FLAGS) -I $(INCLUDE_DIR)

$(REL)/%.o: $(SRC_DIR)/%.c
	@$(MKDIR) $(OBJ_DIR)/$(dir $(@))
	@echo "build $(@)"
	@$(CC) -c $< -o $(OBJ_DIR)/$@ $(FLAGS) $(RELEASE_FLAGS) -I $(INCLUDE_DIR)

intern-compile-%: $(addprefix %/, $(OBJ))
	@$(MKDIR) $(BIN_DIR)/$(MODE)
	@$(LD) -o $(BIN_DIR)/$(MODE)/$(OUT) $(addprefix $(OBJ_DIR)/$(DBG)/, $(OBJ)) $(LDFLAGS)
	@echo "Project compiled in $(MODE)"

clean:
	@$(RM) $(call rwildcard,,*~) $(call rwildcard,,*#) $(call rwildcard,,*.bmp)
	@$(RM) $(BIN_DIR)
	@$(RM) $(OBJ_DIR)
	@$(RM) *.png
	@echo "project cleaned"

run:
	@$(BIN_DIR)/$(REL)/$(OUT)

gdb:
	@gdb $(BIN_DIR)/$(DBG)/$(OUT)

video:
	ffmpeg -f image2 -pattern_type glob -i 'anim/*.png' -r 40 demo.mp4
