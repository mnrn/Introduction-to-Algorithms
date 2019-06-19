#################################################################################
# @brief makefileのテンプレートです...
# @note  GNU Make 3.81で動作確認しました
# @note  あんまり複雑なことはしません
# @date  作成日     : 2016/02/16
# @date  最終更新日 : 2016/02/16
#################################################################################


CC      = g++-5
CFLAGS  = -fopenmp -std=c++14 -O3
SCRS    = 
OBJS    = matvec.o      # 複数指定できます
INC     = 
TARGET  = matvec
LIBS    = 
DEPENDS = $(OBJS:.o=.d)

$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) -o $@ $^ 

clean:
	rm -f $(TARGET) $(OBJS) $(DEPENDS)

-include $(DEPENDS)

