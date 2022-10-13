#
#   Copyright (C) 2022, Xiaosan Zhai(tom.zhai@aliyun.com)
#   This file is part of the xalgos library.
#
#   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
#   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#   See the MIT License for more details.
#
#   You should have received a copy of the MIT License along with this program.
#   If not, see <https://mit-license.org/>.
#

#    INCLUDE_PATH : extern header file's path                   -I
#    LIB_PATH     : while compiling, extern library's path      -L
#    RUN_PATH     : when running, extern library's path         -Wl, -rpath=
#    LIB_SOURCES  : extern librarys                             -l
#    Note         : Add prefix for each path  
INCLUDE_PATH  :=
LIB_PATH      :=
RUN_PATH      :=     # eg : -Wl, -rpath=.
LIB_SOURCES   :=


#    additional depedent files
#    Note: if you add more depedent files here, you have to use make all, if use just call make, it will not make all things


#    used for simple app (undefine it for complex use)
SIMPLE_APP   := true  


#    library depedent file parent dir name
DEPEDENT_DIR := .depend


#    find the target name and what kind of target should be compiled

TARGET_NAME_TMP := $(basename $(notdir $(shell pwd)))
TARGET_KIND_BIN   := $(shell echo $(TARGET_NAME_TMP) | grep "_bin")
TARGET_KIND_LIBSO := $(shell echo $(TARGET_NAME_TMP) | grep "_libso")
TARGET_KIND_LIBA  := $(shell echo $(TARGET_NAME_TMP) | grep "_liba")


#    choose compiler
FILE_TYPE := $(shell find . -name '*.cc')
ifdef FILE_TYPE
    POSTFIX := cc
    XCC     := g++
else
    FILE_TYPE2 := $(shell find . -name '*.cpp')
    ifdef FILE_TYPE2
        POSTFIX := cpp
        XCC     := g++
    else
        FILE_TYPE3 := $(shell find . -name '*.c')
        ifdef FILE_TYPE3
            POSTFIX := c
            XCC     := gcc
        endif
    endif
endif



#    auto-find the source file and dependence relations

SOURCES := $(sort $(shell find . -name '*.$(POSTFIX)'))

OBJECTS  := $(subst .$(POSTFIX),.o,$(SOURCES))

DEPENDS  := $(patsubst %.$(POSTFIX),$(DEPEDENT_DIR)/%.d,$(SOURCES))
DEPENDS1 := $(dir $(DEPENDS))

DEPDIR := $(filter $(DEPEDENT_DIR),$(shell ls -a))
ifndef DEPDIR
    DEPDIR := $(shell mkdir $(DEPEDENT_DIR))
endif

TMP1 := $(shell mkdir -p $(DEPENDS1));

GCC   := gcc



#    define the compile parameters

LOCAL_DIR       := $(shell pwd)
ifndef SIMPLE_APP
  EXPORT_DIR      := $(shell find ../../ -name export)
endif
INCLUDE_DIR     := $(EXPORT_DIR)/include

ifdef TARGET_KIND_BIN
    TARGET_NAME := $(patsubst %_bin,%,$(notdir $(shell pwd)))
    COMPILE_OPTIONS := -std=c99 -DXDEBUG
    LINK_OPTIONS :=
    EXPORT_TARGET_DIR  := $(EXPORT_DIR)/bin/
endif

ifdef TARGET_KIND_LIBA
    TARGET_NAME := $(patsubst %_liba,%,$(notdir $(shell pwd)))
	TARGET_NAME := lib$(TARGET_NAME).a
    COMPILE_OPTIONS := -std=c99 -DXDEBUG
    LINK_OPTIONS :=
    EXPORT_TARGET_DIR  := $(EXPORT_DIR)/lib/
endif

ifdef TARGET_KIND_LIBSO
    TARGET_NAME := $(patsubst %_libso,%,$(notdir $(shell pwd)))
    TARGET_NAME := lib$(TARGET_NAME).so
    COMPILE_OPTIONS := -Wcast-align -D__EXTENSIONS__ -DC_PANIC -DPOSIX -fno-builtin -pipe -O2 -pthread -O0 -g -fno-stack-protector -O3 -D_DETAILED_DBG -funsigned-char -DUSE_32BIT_ONLY -DXDEBUG -fpic -std=c99
    LINK_OPTIONS  := -shared -fPIC
    EXPORT_TARGET_DIR := $(EXPORT_DIR)/lib/ 
endif



#    do the compile processing

.PHONY: all version cleand clean
.INTERMEDIATE: $(OBJECTS)

all: maketest $(TARGET_NAME)
#all: maketest

maketest:
#	@echo "GNU make version: $(MAKE_VERSION) "
#	@echo "Source files: $(SOURCES) "
#	@echo $(TARGET_NAME)
#	@echo $(OS_TYPE)
#	@echo $(OS_VERSION)


ifdef TARGET_KIND_LIBA
$(TARGET_NAME):$(OBJECTS) $(LIBSOURCES)
	ar -rcs $(TARGET_NAME) $^
endif

ifndef TARGET_KIND_LIBA
$(TARGET_NAME):$(OBJECTS) $(LIBSOURCES)
	$(XCC) $(LINK_OPTIONS) $(RUN_PATH) $^ -o $@ $(LIB_PATH) $(LIB_SOURCES) 
endif

%.o: %.$(POSTFIX)
	$(XCC) $(COMPILE_OPTIONS)  $(INCLUDE_PATH) -c $< -o $@	

ifneq "$(MAKECMDGOALS)" "clean"
    -include $(DEPENDS)
endif

$(DEPEDENT_DIR)/%.d: %.$(POSTFIX)
	@$(GCC) -M $< > $@.$$$$;                              \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@;   \
	rm -f $@.$$$$





###########install
install:
	@rm -rf $(INCLUDE_DIR)/$(TARGET_NAME_TMP);             \
	mkdir $(TARGET_NAME_TMP);                              \
	mv $(TARGET_NAME_TMP) $(INCLUDE_DIR);                  \
	cp -rf ./* $(INCLUDE_DIR)/$(TARGET_NAME_TMP);          \
	cd $(INCLUDE_DIR)/$(TARGET_NAME_TMP);                  \
	rm -rf ./$(DEPEDENT_DIR)  $(TARGET_NAME) $(SOURCES) makefile;   \
	cd $(LOCAL_DIR);                                       \
	cp -rf $(TARGET_NAME) $(EXPORT_TARGET_DIR)





###########clean processing
clean:
	@rm -rf *.o $(TARGET_NAME)

cleand:
	@rm -rf $(DEPEDENT_DIR)/*
