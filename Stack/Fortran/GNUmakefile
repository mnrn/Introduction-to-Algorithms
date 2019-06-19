#################################################################################
# @brief makefileのテンプレートです...
# @note  GNU Make 3.81で動作確認しました
# @note  あんまり複雑なことはしません
# @note  以下のサイトを参考にしました
#        http://urin.github.io/posts/2013/simple-makefile-for-clang/
# @note  わからないコマンドがあったらGNU Make(O'reilly)を参考にしてください
# @date  2016/03/07
#################################################################################


FC      = gfortran  
CFLAGS  = -Wall -std=f2003 -O2
SCRS    = 
OBJS    = stack.o      # 複数指定できます
INC     = #-I./include
TARGET  = stack
LIBS    =
DEPENDS = $(OBJS:.o=.d)

%.o: %.f90
	$(FC) $(CFLAGS) $(INC) -o $@ -c $<

$(TARGET): $(OBJS) $(LIBS)
	$(FC) -o $@ $^ 

clean:
	rm -f $(TARGET) $(OBJS) $(DEPENDS)

-include $(DEPENDS)


