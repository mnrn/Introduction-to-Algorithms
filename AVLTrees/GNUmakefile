#################################################################################
# @brief AVLtree用makefile
# @note  GNU Make 3.81で動作確認しました
# @note  あんまり複雑なことはしません
# @date  作成日     : 2016/02/07
# @date  最終更新日 : 2016/02/07
#################################################################################


CC     = clang++
CFLAGS  = -Wall -Wextra -std=c++14 -O3 -MMD -MP
SCRS    = 
OBJS    = avltree.o      # 複数指定できます
INC     = #-I./include
TARGET  = avltree
LIBS    =
DEPENDS = $(OBJS:.o=.d)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INC) -o $@ -c $<

$(TARGET): $(OBJS) $(LIBS)
	$(CC) -o $@ $^ 

clean:
	rm -f $(TARGET) $(OBJS) $(DEPENDS)

-include $(DEPENDS)

