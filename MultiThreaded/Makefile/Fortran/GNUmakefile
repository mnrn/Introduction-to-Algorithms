#################################################################################
# @brief makefileのテンプレートです...
# @note  GNU Make 3.81で動作確認しました
# @note  あんまり複雑なことはしません
# @note  以下のサイトを参考にしました
#        http://urin.github.io/posts/2013/simple-makefile-for-clang/
# @note  わからないコマンドがあったらGNU Make(O'reilly)を参考にしてください
# @date  2016/03/08
#################################################################################


FC      = gfortran  
CFLAGS  = -Wall -Wextra -O3 -fopenmp
SCRS    = 
OBJS    = hoge.o      # 複数指定できます
INC     = #-I./include
TARGET  = hoge
LIBS    =
DEPENDS = $(OBJS:.o=.d)

$(TARGET): $(TARGET).f03
	$(FC) $(CFLAGS) -o $@ $^ 

clean:
	rm -f $(TARGET) $(OBJS) $(DEPENDS)

-include $(DEPENDS)


