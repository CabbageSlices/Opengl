#cheat sheet https://devhints.io/makefile
SRC_DIR = src#.cpp / .h
BUILD_DIR_BASE = build#.o files, base path, where all the build subfolders will be made. Folder structure will match SRC_DIR folder structure when built
#on clean ENTIRE build folder will be deleted
OUTPUT_DIR = bin#.exe
INCLUDE_DIR = include
RUN_MAINS = %main.cpp %main.o#stuff containing int main(...), the entry point. need to filter this out while testing since it will build with test_mains
TEST_MAINS = %main_test.cpp %main_test.o#main to use while testing, filter this out in regular build
SHADERS = shaders
LIB_DIR = lib

#get ALL .cpp files in all subdirectories
#https://docs.microsoft.com/en-us/previous-versions/windows/it-pro/windows-server-2012-R2-and-2012/cc753551(v=ws.11)?redirectedfrom=MSDN
SRC := $(shell FORFILES /P $(SRC_DIR) /s /m *.cpp /c "CMD /c ECHO @relpath")

#remove quotation marks around src files
SRC := $(patsubst ".\\%.cpp", $(SRC_DIR)\\%.cpp, $(SRC))

#change srcDirectory/relativepath.cpp to buildDirectory/relativepath.o
OBJECTS := $(SRC:$(SRC_DIR)\\%.cpp=$(BUILD_DIR_BASE)\\%.o)
DEPS := $(OBJECTS:.o=.d)

#directorys needed for objects
#get all directories, remove the \ suffix, remove duplicates because sort removes duplicates
BUILD_DIRS := $(dir $(OBJECTS))
BUILD_DIRS := $(patsubst %\,%,$(BUILD_DIRS)) 
BUILD_DIRS := $(sort $(BUILD_DIRS))

CPP = g++
CPP_FLAGS = -I$(INCLUDE_DIR) -I$(SHADERS) -I$(SRC_DIR) -MMD -MP -std=c++14 -g
LINKER_FLAGS = -L$(LIB_DIR) -lsfml-graphics -lsfml-window -lsfml-system -lopengl32 -lgmock  -lgtest

#regular builds, filter out mains used for testing
all : $(filter-out $(TEST_MAINS), $(OBJECTS))
	$(CPP) -o $(OUTPUT_DIR)/main.exe $^ $(LINKER_FLAGS)

run : $(filter-out $(TEST_MAINS), $(OBJECTS))
	$(CPP) -o $(OUTPUT_DIR)/main.exe $^ $(LINKER_FLAGS)
	$(OUTPUT_DIR)/main.exe

#for testing, don't include main file because it's included with test_mains
test: $(filter-out $(RUN_MAINS), $(OBJECTS))
	$(CPP) -o $(OUTPUT_DIR)/test.exe $^ $(LINKER_FLAGS)
	$(OUTPUT_DIR)/test.exe

#https://www.gnu.org/software/make/manual/html_node/Secondary-Expansion.html
.SECONDEXPANSION:

# $(_BUILD_DIR_BASE)/%.o : $(SRC_DIR)/%.cpp
# 	$(CPP) -o $@ -c $< $(CPP_FLAGS)

#https://www.gnu.org/software/make/manual/make.html#Static-Usage
# https://www.gnu.org/software/make/manual/make.html#Prerequisite-Types order only (the pipe symbol, don't let directory timestamp force target rebuild)
$(OBJECTS) : $(BUILD_DIR_BASE)\\%.o: $(SRC_DIR)\\%.cpp | $$(@D)
	$(CPP) -o $@ -c $< $(CPP_FLAGS)

#make directories if it doens't exsist, otherwise make wil fk up
$(BUILD_DIRS) :
	mkdir $@


.PHONY : clean

#https://docs.microsoft.com/en-us/powershell/module/microsoft.powershell.management/remove-item?view=powershell-7 delete shit
#ignore errors since you removed files will cause an error if they don't exist
#user powershell directly because i can't figure out a way to delete subfolders
clean :
	-powershell Remove-Item $(BUILD_DIR_BASE)/* -Recurse
	-del $(OUTPUT_DIR)\*.exe


-include $(DEPS)