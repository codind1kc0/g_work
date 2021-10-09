.PHONY : all clean rebuild run

KERNEL_ADDR := B000
APP_ADDR    := 10000
IMG := Cstart.img
IMG_PATH := /mnt/hgfs

DIR_DEPS := deps
DIR_EXES := exes
DIR_OBJS := objs

DIRS := $(DIR_DEPS) $(DIR_EXES) $(DIR_OBJS)

KENTRY_SRC := kentry.asm
AENTRY_SRC := aentry.asm
BLFUNC_SRC := blfunc.asm
BOOT_SRC   := boot.asm
LOADER_SRC := loader.asm
COMMON_SRC := common.asm

KERNEL_SRC := kmain.c\
              screen.c kernel.c utility.c\
              task.c interrupt.c ihandler.c list.c\
			  queue.c    \
			  memory.c   \
			  mutex.c    \
			  keyboard.c \
			  event.c

APP_SRC := screen.c utility.c memory.c list.c\
			  sys_call.c demo2.c\
			  shell.c    \
              app.c

BOOT_OUT   := boot
LOADER_OUT := loader
KERNEL := kernel
APP    := app
KERNEL_OUT := kernel.out
APP_OUT    := app.out
KENTRY_OUT := $(DIR_OBJS)/kentry.o
AENTRY_OUT := $(DIR_OBJS)/aentry.o

KERNEL_EXE := kernel.out
KERNEL_EXE := $(addprefix $(DIR_EXES)/, $(KERNEL_EXE))
APP_EXE    := app.out
APP_EXE    := $(addprefix $(DIR_EXES)/, $(APP_EXE))

#SRCS := $(KERNEL_SRC)
#SRCS := $(wildcard *.c)
KERNEL_OBJS := $(KERNEL_SRC:.c=.o)
KERNEL_OBJS := $(addprefix $(DIR_OBJS)/, $(KERNEL_OBJS))
KERNEL_DEPS := $(KERNEL_SRC:.c=.dep)
KERNEL_DEPS := $(addprefix $(DIR_DEPS)/, $(KERNEL_DEPS))

APP_OBJS := $(APP_SRC:.c=.o)
APP_OBJS := $(addprefix $(DIR_OBJS)/, $(APP_OBJS))
APP_DEPS := $(APP_SRC:.c=.dep)
DEPS := $(addprefix $(DIR_DEPS)/, $(APP_DEPS))

all : $(DIR_OBJS) $(DIR_EXES) $(IMG) $(BOOT_OUT) $(LOADER_OUT) $(KERNEL_OUT) $(KERNEL) $(APP_OUT) $(APP)
	sudo cp ../img_file/data.img $(IMG)
	dd if=$(BOOT_OUT) of=$(IMG) count=512 bs=1 conv=notrunc;
	@echo "Build Success ==> Star_OS!"
	
ifeq ("$(MAKECMDGOALS)", "all")
-include $(KERNEL_DEPS)
-include $(APP_DEPS)
endif

ifeq ("$(MAKECMDGOALS)", "")
-include $(KERNEL_DEPS)
-include $(APP_DEPS)
endif

$(IMG) :
	@dd if=/dev/zero of=$@ count=1024 bs=1440
	@echo "init img..."
	
$(BOOT_OUT) : $(BOOT_SRC) $(BLFUNC_SRC)
	nasm $< -o $@
	dd if=$@ of=$(IMG) bs=512 count=1 conv=notrunc
	
$(LOADER_OUT) : $(LOADER_SRC) $(COMMON_SRC) $(BLFUNC_SRC)
	nasm $< -o $@
	sudo mount -o loop ../img_file/data.img $(IMG_PATH)
	sudo cp $@ $(IMG_PATH)/$@
	sudo umount $(IMG_PATH)
	
$(KENTRY_OUT) : $(KENTRY_SRC) $(COMMON_SRC)
	nasm -f elf $< -o $@
    
$(KERNEL_OUT) :$(KENTRY_OUT) $(KERNEL_OBJS)
	ld -s -melf_i386 $^ -o $@
	#kentry and kernel => 0xB000
	./elf2kobj -cB000 $@ $(KERNEL) 
	sudo mount -o loop ../img_file/data.img $(IMG_PATH)
	sudo cp $(KERNEL) $(IMG_PATH)/$(KERNEL)
	sudo umount $(IMG_PATH)

$(AENTRY_OUT) : $(AENTRY_SRC) $(COMMON_SRC)
	nasm -f elf $< -o $@
    
$(APP_OUT) :$(AENTRY_OUT) $(APP_OBJS)
	ld -s -melf_i386 $^ -o $@
	#aentry and app => 0x10000
	./elf2kobj -c10000 $@ $(APP)
	sudo mount -o loop ../img_file/data.img $(IMG_PATH)
	sudo cp $(APP) $(IMG_PATH)/$(APP)
	sudo umount $(IMG_PATH)
	
	
$(DIR_OBJS)/%.o : %.c
	gcc -O0 -m32 -fno-builtin -fno-stack-protector -c $(filter %.c, $^) -o $@

$(DIRS) :
	mkdir $@

ifeq ("$(wildcard $(DIR_DEPS))", "")
$(DIR_DEPS)/%.dep : $(DIR_DEPS) %.c
else
$(DIR_DEPS)/%.dep : %.c
endif
	@echo "Creating $@ ..."
	@set -e; \
	gcc -MM -E $(filter %.c, $^) | sed 's,\(.*\)\.o[ :]*,objs/\1.o $@ : ,g' > $@
	
clean :
	rm -fr $(IMG) $(BOOT_OUT) $(LOADER_OUT) $(KERNEL_OUT) $(DIRS) $(KERNEL) $(APP_OUT) $(APP)
	
rebuild :
	@$(MAKE) clean
	@$(MAKE) all

run:
	@bochs -f ../img_file/bochsrc
