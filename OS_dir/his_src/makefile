:
.PHONY : all clean rebuild run

KERNEL_ADDR := B000
IMG := Cstart.img
IMG_PATH := /mnt/hgfs

DIR_DEPS := deps
DIR_EXES := exes
DIR_OBJS := objs

DIRS := $(DIR_DEPS) $(DIR_EXES) $(DIR_OBJS)

KENTRY_SRC := kentry.asm
BLFUNC_SRC := blfunc.asm
BOOT_SRC   := boot.asm
LOADER_SRC := loader.asm
COMMON_SRC := common.asm

KERNEL_SRC := kmain.c screen.c kernel.c global.c

BOOT_OUT   := boot
LOADER_OUT := loader
KERNEL := kernel
KERNEL_OUT := kernel.out
KENTRY_OUT := $(DIR_OBJS)/kentry.o

EXE := kernel.out
EXE := $(addprefix $(DIR_EXES)/, $(EXE))

SRCS := $(KERNEL_SRC)
#SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
OBJS := $(addprefix $(DIR_OBJS)/, $(OBJS))
DEPS := $(SRCS:.c=.dep)
DEPS := $(addprefix $(DIR_DEPS)/, $(DEPS))

all : $(DIR_OBJS) $(DIR_EXES) $(IMG) $(BOOT_OUT) $(LOADER_OUT) $(KERNEL_OUT) $(KERNEL)
	sudo cp ../img_file/data.img $(IMG)
	dd if=$(BOOT_OUT) of=$(IMG) count=512 bs=1 conv=notrunc;
	@echo "Build Success ==> Star_OS!"
	
ifeq ("$(MAKECMDGOALS)", "all")
-include $(DEPS)
endif

ifeq ("$(MAKECMDGOALS)", "")
-include $(DEPS)
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
    
$(KERNEL_OUT) :$(KENTRY_OUT) $(OBJS)
	ld -s -melf_i386 $^ -o $@
	./elf2kobj -cB000 $@ $(KERNEL)
	sudo mount -o loop ../img_file/data.img $(IMG_PATH)
	sudo cp $(KERNEL) $(IMG_PATH)/$(KERNEL)
	sudo umount $(IMG_PATH)
	
$(DIR_OBJS)/%.o : %.c
	gcc -m32 -fno-builtin -fno-stack-protector -c $(filter %.c, $^) -o $@

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
	rm -fr $(IMG) $(BOOT_OUT) $(LOADER_OUT) $(KERNEL_OUT) $(DIRS) $(KERNEL) 
	
rebuild :
	@$(MAKE) clean
	@$(MAKE) all

run:
	@bochs -f ../img_file/bochsrc
