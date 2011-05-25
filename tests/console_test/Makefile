APP = console_test
OBJS = console_test.o \
       menus.o

VPATH = ../../app/console_menu
CFLAGS = -I../../app/console_menu/

$(APP): $(OBJS)
	$(CXX) $(CFLAGS) $(CXXFLAGS) -o $@  $^ 

%.o: %.cpp
	$(CXX) $(CFLAGS) $(CXXFLAGS)  -MMD -MP -MF $(@:%.o=%.d) -MT $@ -o $@ -c $< 

%.o: %.c
	$(CC) $(CFLAGS)  -MMD -MP -MF $(@:%.o=%.d) -MT $@ -o $@ -c $< 
