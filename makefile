.PHONY:Default print clean

export BUILD_ROOT = $(shell pwd)

export GCC := g++
export GCC_FLAG := -g -O0 -std=c++11
export GCC_INCS_DIR := -I/usr/local/include/mariadb
export GCC_LIBS_DIR := -L/usr/local/lib/mariadb
export GCC_LIBS := -pthread -lmariadb

BUILD_DIR := $(BUILD_ROOT)/XSrc \
	     $(BUILD_ROOT)/server

Default:
	@for dir in $(BUILD_DIR); \
	do \
		make -C $$dir; \
	done
	@echo "-----------------------------------------------------------------" 
	@echo "                      Good job! ~(@^_^@)~" 
	@echo "-----------------------------------------------------------------" 

print:
	@for dir in $(BUILD_DIR); \
	do \
		make print -C $$dir; \
	done
	@echo "-----------------------------------------------------------------" 
	@echo "                      Good job! ~(@^_^@)~" 
	@echo "-----------------------------------------------------------------" 

clean:
	@for dir in $(BUILD_DIR); \
	do \
		make clean -C $$dir; \
	done
	@echo "-----------------------------------------------------------------" 
	@echo "                      Good job! ~(@^_^@)~" 
	@echo "-----------------------------------------------------------------" 


