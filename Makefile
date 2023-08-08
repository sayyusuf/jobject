NAME = jobject.a

_SRC = jobject.c stringify.c parser.c 

_OBJ = $(_SRC:.c=.o)

EXLIB1 = cvec
EXLIB1PATH = cvec
EXLIB1LINK = https://github.com/sayyusuf/str_lexer.git

EXLIB2 = cmap
EXLIB2PATH = cmap
EXLIB2LINK = https://github.com/sayyusuf/cmap.git

EXLIB3 = str_lexer
EXLIB3PATH = str_lexer
EXLIB3LINK = https://github.com/sayyusuf/cvec.git


EXLIBS=	EXLIB1	\
	EXLIB2	\
	EXLIB3





CC = gcc
CFLAGS = -Wall -Wextra -Werror 

.PHONY: all clean fclean re libs  test

all : $(EXLIBS)  $(NAME)

$(NAME): $(_OBJ)
	ar -rcs $@ $^
	ranlib $@

%.o: %.c
	$(CC)  $(CFLAGS) -I./ $(INC) -c $< -o $@

$(EXLIBS):
	-cd ../ && git clone --recurse-submodules $($@LINK)
	$(eval INC += -I../$($@PATH))

clean : 
	rm -f $(_OBJ)
fclean: clean
	rm -f $(NAME)

re: fclean all



test: libs

libs:
	-@$(foreach lib,$(EXLIBS),   git clone --recurse-submodules $($(lib)LINK) test/$($(lib)PATH);)
	@$(foreach lib,$(EXLIBS),  make test CFLAGS=$(CFLAGS) -C ./test/$($(lib)PATH);)
	$(foreach lib,$(EXLIBS), $(eval TESTINC += -I./test/$($(lib)PATH)))
	$(foreach lib,$(EXLIBS), $(eval TESTLIB += ./test/$($(lib)PATH)/$($(lib).a)))
	
	$(CC)  $(CFLAGS) -I./ $(TESTINC) $(TESTLIB) ./test/main.c -o test && cd test && ./test


#link: $(LINKS)
#$(LINKS):
#	cd test
#	git clone --recurse-submodules $@ 



