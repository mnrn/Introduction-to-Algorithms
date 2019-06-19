#################################################################################
# @brief selection用makefile
# @note  GNU Make 3.81で動作確認しました
# @date  作成日     : 2016/02/03
# @date  最終更新日 : 2016/02/03
#################################################################################


CC     = g++ 
CFLAGS  = -Wall -Wextra -std=c++14 -O3 -MMD -MP
SCRS    = 
OBJS    = selection.o      # 複数指定できます
INC     = #-I./include
TARGET  = selection
LIBS    =
DEPENDS = $(OBJS:.o=.d)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INC) -o $@ -c $<

$(TARGET): $(OBJS) $(LIBS)
	$(CC) -o $@ $^ 

clean:
	rm -f $(TARGET) $(OBJS) $(DEPENDS)

-include $(DEPENDS)
