# 变量定义
CC = gcc
CFLAGS = -Wall -g -std=c99
LDFLAGS = -luser32 -lgdi32 # 假设你需要这些库，根据实际情况修改
DEPS = pages/*.h fishBone_Struct.h fb_handware.h \

# 包含所有依赖的头文件
OBJ = build/page_menu.o build/page_test1.o build/page_test2.o \
      build/fishBone_Struct.o build/fb_handware.o build/main.o 

# 假设有对应的 fb_hardware.c 文件
EXEC = myprogram

# 默认目标
all: $(EXEC)

# 链接目标
$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

# 生成目标文件
build/%.o: pages/%.c $(DEPS)
#@mkdir -p build  
#创建 build 目录（如果不存在）
	$(CC) $(CFLAGS) -c $< -o $@

# 生成目标文件规则对于根目录下的源文件
build/%.o: ./%.c $(DEPS)
#@mkdir -p build  # 创建 build 目录（如果不存在）
	$(CC) $(CFLAGS) -c $< -o $@
# 清理目标
clean:
#  del $(OBJ) $(EXEC)
	del build/*.o 