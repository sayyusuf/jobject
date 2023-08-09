NAME = libjobject.a
LIBNAME = jobject
_SRC = jobject.c stringify.c parser.c 
_OBJ = $(_SRC:.c=.o)



EXLIB1 = str_lexer
EXLIB1PATH = str_lexer
EXLIB1LINK = https://github.com/sayyusuf/str_lexer.git

EXLIB2 = cmap
EXLIB2PATH = cmap
EXLIB2LINK = https://github.com/sayyusuf/cmap.git

EXLIB3 =  cvec
EXLIB3PATH = cvec
EXLIB3LINK = https://github.com/sayyusuf/cvec.git

EXLIBS=	EXLIB1	\
	EXLIB2	\
	EXLIB3





CC = gcc
CFLAGS = -Wall -Wextra -Werror 

.PHONY: all clean fclean re libs  test update

all : $(EXLIBS)  $(NAME)

$(NAME): $(_OBJ)
	ar -rcs $@ $^
	ranlib $@

%.o: %.c
	$(CC)  $(CFLAGS) -I./ $(INC) -c $< -o $@

$(EXLIBS):
	-cd ../ && git clone --recurse-submodules $($@LINK) && echo $($@LINK) OK.
	$(eval INC += -I../$($@PATH))

update:
	git pull
	-@$(foreach lib,$(EXLIBS), make update -C ./test/$($(lib)PATH);)
	-@$(foreach lib,$(EXLIBS), make update -C ../$($(lib)PATH);)

clean : 
	-rm -f $(_OBJ)
	-@$(foreach lib,$(EXLIBS), make clean -C ../$($(lib)PATH);)
	-@$(foreach lib,$(EXLIBS), make clean -C ./test/$($(lib)PATH);)

fclean: clean
	-@$(foreach lib,$(EXLIBS), make fclean -C ../$($(lib)PATH);)
	-@$(foreach lib,$(EXLIBS), rm -rf ./test/$($(lib)PATH);)
	-rm -f $(NAME) 

re: fclean all



test: all libs

libs:
	-@$(foreach lib,$(EXLIBS),   git clone --recurse-submodules $($(lib)LINK) test/$($(lib)PATH);)
	-@$(foreach lib,$(EXLIBS),  make test CFLAGS=$(CFLAGS) -C ./test/$($(lib)PATH);)
	@$(foreach lib,$(EXLIBS),  make CFLAGS=$(CFLAGS) -C ./test/$($(lib)PATH);)
	$(foreach lib,$(EXLIBS), $(eval TESTINC += -I./test/$($(lib)PATH)))
	$(foreach lib,$(EXLIBS), $(eval TESTLIB += -l $($(lib))))
	$(foreach lib,$(EXLIBS), $(eval TESTLIBDIR += -L./test/$($(lib)PATH) -l $($(lib))))
	@echo 	*********************************************/\*********************************************
	$(CC)  $(CFLAGS) -I./ $(TESTINC) -L. $(TESTLIBDIR)  ./test/main.c  -l$(LIBNAME) $(TESTLIB)  -o test/run && cd test && ./run


#link: $(LINKS)
#$(LINKS):
#	cd test
#	git clone --recurse-submodules $@ 



