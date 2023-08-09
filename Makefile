NAME = libjobject.a
LIBNAME = jobject
_SRC = jobject.c stringify.c parser.c 
_OBJ = $(_SRC:.c=.o)
EXTRAFLAGS=


EXLIB1 = str_lexer
EXLIB1LINK = https://github.com/sayyusuf/str_lexer.git

EXLIB2 = cmap
EXLIB2LINK = https://github.com/sayyusuf/cmap.git

EXLIB3 =  cvec
EXLIB3LINK = https://github.com/sayyusuf/cvec.git

EXLIBS=	EXLIB1	\
	EXLIB2	\
	EXLIB3





CC = gcc
CFLAGS = -Wall -Wextra -Werror 

.PHONY: all clean fclean re libs  test update

all : .gitignore test/main.c $(EXLIBS)  $(NAME)

$(NAME): $(_OBJ)
	ar -rcs $@ $^
	ranlib $@

%.o: %.c
	$(CC)  $(CFLAGS) -I./ $(INC) -c $< $(EXTRAFLAGS) -o $@

$(EXLIBS):
	-cd ../ && git clone --recurse-submodules $($@LINK) && echo $($@LINK) OK.
	$(eval INC += -I../$($@))
	make CFLAGS=$(CFLAGS) -C ../$($@)

.gitignore:
	echo >> $@ "*.o"

test/main.c:
	-mkdir test
	echo >> $@ \
		"#include <stdio.h>\n \
		int main(){ printf(\"test stated...\\\n\");\n}"

update:
	git pull
	-@$(foreach lib,$(EXLIBS), make update -C ./test/modules/$($(lib);)
	-@$(foreach lib,$(EXLIBS), make update -C ../$($(lib));)

clean : 
	-rm -f $(_OBJ)
	-@$(foreach lib,$(EXLIBS), make clean -C ../$($(lib));)
	-@$(foreach lib,$(EXLIBS), make clean -C ./test/modules/$($(lib));)

fclean: clean
	-@$(foreach lib,$(EXLIBS), make fclean -C ../$($(lib));)
	-@$(foreach lib,$(EXLIBS), rm -rf ./test/modules/$($(lib));)
	-rm -f $(NAME) test/run


re: fclean all



test: all ready runtest

ready:
	-@$(foreach lib,$(EXLIBS),   git clone --recurse-submodules $($(lib)LINK) test/modules/$($(lib));)
	-@$(foreach lib,$(EXLIBS),  make test CFLAGS=$(CFLAGS) EXTRAFLAGS=$(EXTRAFLAGS) -C ./test/modules/$($(lib));)
	@$(foreach lib,$(EXLIBS),  make CFLAGS=$(CFLAGS) EXTRAFLAGS=$(EXTRAFLAGS) -C ./test/modules/$($(lib));)

runtest:
	$(eval TESTMODULES = $(shell ls test/modules))
	$(foreach module ,$(TESTMODULES), $(eval TESTINC += -I./test/modules/$(module)))
	$(foreach module ,$(TESTMODULES), $(eval TESTLIB += -l $(module)))
	$(foreach module ,$(TESTMODULES), $(eval TESTLIBDIR += -L./test/modules/$(module)))
	@echo 	*********************************************/\*********************************************
	echo $(TESTMODULES)
	$(CC)  $(CFLAGS) -I./ $(TESTINC) -L. $(TESTLIBDIR)  ./test/main.c  -l$(LIBNAME) $(TESTLIB) $(EXTRAFLAGS) -o test/run && cd test && ./run


