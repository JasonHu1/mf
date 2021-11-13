
MAKE	:= make

PROTOVER:=Z3GatewayHost
SRC_DIR= $(shell pwd)/app#采用绝对路径

USER_SRC_DIRS = $(shell find $(SRC_DIR) -type d)
$(info USER_SRC_DIRS = $(USER_SRC_DIRS))

#APPLICATION_FILES += $(foreach dir, $(USER_SRC_DIRS), $(wildcard $(dir)/*.c)) 
#APPLICATION_FILES += $(foreach dir, $(USER_SRC_DIRS), $(wildcard $(dir)/*.cpp)) 
APPLICATION_FILES +=$(SRC_DIR)/app_uproto.c
APPLICATION_FILES +=$(SRC_DIR)/app.c
APPLICATION_FILES +=$(SRC_DIR)/app_interface.c
APPLICATION_FILES +=$(SRC_DIR)/app_device.c
APPLICATION_FILES +=$(SRC_DIR)/app_driver.c
APPLICATION_FILES +=$(SRC_DIR)/app_schedue.c
APPLICATION_FILES +=$(SRC_DIR)/app_util.c
APPLICATION_FILES +=$(SRC_DIR)/app_zcl.c
APPLICATION_FILES +=$(SRC_DIR)/information_process.c


$(info APPLICATION_FILES = $(APPLICATION_FILES))

#COMPILER_DEFINES =-Werror

export COMPILER_DEFINES
export APPLICATION_FILES
export COMPILER_INCLUDES += -I$(USER_SRC_DIRS)
$(info COMPILER_INCLUDES = $(COMPILER_INCLUDES))

##带着变量进到另外目录调用里面的Makefile文件
all :
	$(MAKE) -C ./$(PROTOVER)/

clean :
	make -C ./$(PROTOVER)/ clean
	rm -rf ./AmberGwZ3
	rm -rf ./tags
