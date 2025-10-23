LINKED_LIST_TARGET = linked_list
LINKED_LIST_OBJS += linked_list.o
LINKED_LIST_OBJS += linked_list_test.o

LINKED_LIST_INTERACTIVE_TARGET = linked_list_interactive
LINKED_LIST_INTERACTIVE_OBJS += linked_list.o
LINKED_LIST_INTERACTIVE_OBJS += linked_list_test_interactive.o

POLYMORPHIC_INTS_TARGET = polymorphic_ints
POLYMORPHIC_INTS_OBJS += polymorphic_ints.o
POLYMORPHIC_INTS_OBJS += polymorphic_ints_test.o

ZLA_TARGET = zla
ZLA_OBJS += zla.o
ZLA_OBJS += zla_test.o

KOBJECT_TARGET = kobject
KOBJECT_OBJS += kobject.o
KOBJECT_OBJS += kobject_test.o

CC = gcc
CFLAGS += -std=gnu99 -Wall

ALL_EXECUTABLES = linked_list polymorphic_ints zla kobject
INTERACTIVE_EXECUTABLES = linked_list_interactive

all: CFLAGS += -g -O2 # release flags
all: $(ALL_EXECUTABLES)

release: clean all

interactive: CFLAGS += -g -O2 # release flags
interactive: $(INTERACTIVE_EXECUTABLES)

debug: CFLAGS += -g -O0 # debug flags
debug: clean $(ALL_EXECUTABLES)

$(LINKED_LIST_TARGET): $(LINKED_LIST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^
$(POLYMORPHIC_INTS_TARGET): $(POLYMORPHIC_INTS_OBJS)
	$(CC) $(CFLAGS) -o $@ $^
$(ZLA_TARGET): $(ZLA_OBJS)
	$(CC) $(CFLAGS) -o $@ $^
$(KOBJECT_TARGET): $(KOBJECT_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(LINKED_LIST_INTERACTIVE_TARGET): $(LINKED_LIST_INTERACTIVE_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-@rm -f *.o ${ALL_EXECUTABLES}
	-@rm -f linked_list_output/backward
	-@rm -f linked_list_output/forward
	-@rm -f zla_output/out

test:
	@chmod +x grade.py
	@sed -i -e 's/\r$$//g' grade.py # dos to unix
	@sed -i -e 's/\r/\n/g' grade.py # mac to unix
	@./grade.py
run:
	./linked_list_interactive
