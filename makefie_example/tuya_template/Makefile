ROOT_DIR ?= $(abspath .)
COMPILE_PREX ?=
USER_LINK_FLAGS ?=
APP_BIN_NAME ?= Z3GatewayHost
USER_SW_VER ?= 1.0


# 当前 makefile 文件所在目录
define my-dir
$(strip \
        $(eval _md_file_:=$$(lastword $$(MAKEFILE_LIST)))       \
        $(if $(filter $(CLEAR_VARS),$(_md_file_)),      \
                $(error LOCAL_PATH must be set before including $$(CLEAR_VARS)) \
                ,       \
                $(patsubst %/,%,$(dir $(_md_file_)))    \
        )       \
)
endef

define source-add-suffix
$(foreach src,$(1),$(addsuffix .o,$(wildcard $(src))))
endef
-include ./local.mk

AR = $(COMPILE_PREX)ar
CC = $(COMPILE_PREX)gcc
NM = $(COMPILE_PREX)nm
CPP = $(COMPILE_PREX)g++
OBJCOPY = $(COMPILE_PREX)objcopy
OBJDUMP = $(COMPILE_PREX)objdump
APP_PACK = ./build/pack.sh

LINKFLAGS = \
		-L$(ROOT_DIR)/library -lty_iot_sdk -lty_module_gw_base -lpthread -lm 
LINKFLAGS +=-lty_iot_sdk
LINKFLAGS +=-lty_module_center_ctrl
LINKFLAGS +=-lty_module_router
LINKFLAGS +=-lty_module_bluetooth
LINKFLAGS +=-lty_module_center_ctrl
LINKFLAGS +=-lty_module_gw_base
LINKFLAGS +=-lty_module_pegasus
LINKFLAGS +=-lty_module_router
LINKFLAGS +=-lty_module_zigbee

# LINKFLAGS += $(USER_LINK_FLAGS)
LINKFLAGS := -Wl,--start-group $(LINKFLAGS) -Wl,--end-group

CCFLAGS = \
	-g -fPIC -Wno-unused-function -Wno-incompatible-pointer-types

DEFINES = -DAPP_BIN_NAME=\"$(APP_BIN_NAME)\" \
		  -DUSER_SW_VER=\"$(USER_SW_VER)\"	\
		  $(ZIGBEE_HOST_DEFINES)

CFLAGS = $(CCFLAGS) $(DEFINES) $(USER_INCS) $(USER_CFLAGS)

#编译所用变量
OUTPUT_DIR = $(ROOT_DIR)/output/$(APP_BIN_NAME)_$(USER_SW_VER)
OUTPUT_DIR_OBJS = $(OUTPUT_DIR)/.objs

#user app基准路径
USER_SRC_BASE_DIR = ./src
USER_INC_BASE_DIR = ./include

#user app 编译文件 
USER_SRC_DIRS = $(shell find $(USER_SRC_BASE_DIR) -type d)
USER_SRCS += $(foreach dir, $(USER_SRC_DIRS), $(wildcard $(dir)/*.c))
USER_SRCS += $(foreach dir, $(USER_SRC_DIRS), $(wildcard $(dir)/*.cpp))
USER_SRCS += $(foreach dir, $(USER_SRC_DIRS), $(wildcard $(dir)/*.s))
USER_SRCS += $(foreach dir, $(USER_SRC_DIRS), $(wildcard $(dir)/*.S))

USER_SRCS += $(ZIGBEE_HOST_USER_SRC)
#user头文件
USER_INCS = $(addprefix -I ,  $(shell find $(USER_INC_BASE_DIR) -type d) )
USER_INCS += -I$(ROOT_DIR)/library/inc 
USER_INCS += -I$(ROOT_DIR)/library/inc/svc_devos/include/ 
USER_INCS += -I$(ROOT_DIR)/library/inc/base_utilities/include/ 
USER_INCS += -I$(ROOT_DIR)/library/inc/base_msgq/include/ 
USER_INCS += -I$(ROOT_DIR)/library/inc/base_timeq/include/ 
USER_INCS += -I$(ROOT_DIR)/library/inc/svc_api_iot/include/ 
USER_INCS += -I$(ROOT_DIR)/library/inc/svc_wifi_netcfg/include/ 
USER_INCS += -I$(ROOT_DIR)/library/inc/svc_lan/include/ 
USER_INCS += -I$(ROOT_DIR)/library/inc/base_security/include/ 
USER_INCS += -I$(ROOT_DIR)/library/inc/svc_gw_sdk_interface/include 
USER_INCS += -I$(ROOT_DIR)/library/inc/base_iot_config/include/
USER_INCS += $(ZIGBEE_HOST_USER_INCLUDES)

#user的obj命令,使用wildcard函数过滤USER_SRCS列出但实际不存在的文件
USER_OBJS = $(call source-add-suffix,$(USER_SRCS))
#user的实际obj地址
USER_OBJS_OUT =  $(addprefix $(OUTPUT_DIR_OBJS)/, $(USER_OBJS))

all: pack

pack: build_app
ifeq ($(APP_PACK),$(wildcard $(APP_PACK)))
	sh $(APP_PACK) $(OUTPUT_DIR) $(ROOT_DIR) $(APP_BIN_NAME) $(USER_SW_VER) $(COMPILE_PREX)
endif

build_app: $(USER_OBJS_OUT)
	@mkdir -p $(dir $@);
	$(CC) $(USER_OBJS_OUT) $(CFLAGS) $(LINKFLAGS) -o $(OUTPUT_DIR)/$(APP_BIN_NAME)
	@echo "Build APP Finish"

$(OUTPUT_DIR_OBJS)/%.c.o: %.c
	mkdir -p $(dir $@);
	$(CC) $(CFLAGS) -o $@ -c $<

$(OUTPUT_DIR_OBJS)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@);
	$(CC) $(CFLAGS) -o $@ -c $<

$(OUTPUT_DIR_OBJS)/%.s.o: %.s
	@mkdir -p $(dir $@);
	$(CC) $(CFLAGS) -o $@ -c $<

$(OUTPUT_DIR_OBJS)/%.S.o: %.S
	@mkdir -p $(dir $@);
	$(CC) $(CFLAGS) -D__ASSEMBLER__ -o $@ -c $<


.PHONY: all clean SHOWARGS build_app pack
clean:
	rm -rf $(OUTPUT_DIR)

