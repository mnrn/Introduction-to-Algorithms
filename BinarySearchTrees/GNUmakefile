#################################################################################
# @brief quicksort用makefile
# @note  GNU Make 3.81で動作確認しました
# @note  あんまり複雑なことはしません
# @date  作成日     : 2016/02/03
# @date  最終更新日 : 2016/02/03
#################################################################################


CC     = g++          # 現在g++はllvmとなります.
                      # 普通のgccが使いたい場合はg++-5とします
CFLAGS  = -Wall -Wextra -std=c++14 -g -MMD -MP
SCRS    = 
OBJS    = bstree.o
INC     = #-I./include
TARGET  = bstree
LIBS    =
DEPENDS = $(OBJS:.o=.d)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INC) -o $@ -c $<

$(TARGET): $(OBJS) $(LIBS)
	$(CC) -o $@ $^ 

clean:
	rm -f $(OBJS) $(DEPENDS) $(TARGET)

-include $(DEPENDS)

