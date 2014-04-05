.PHONY: clean, mrproper, test

SHELL=/bin/bash
CC=gcc #color-gcc ?
CFLAGS=-Wall -Wextra -std=gnu99 -lbluetooth

INCDIR=./include
export C_INCLUDE_PATH=$(INCDIR)
SRCDIR=./src
OBJDIR=./obj
vpath %.c $(SRCDIR)
vpath %.h $(INCDIR)

SRC=$(wildcard $(SRCDIR)/*.c)
OBJ=$(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
HDR=$(wildcard $(INCDIR)/*.h)
DEP=$(OBJ:.o=.d)
EXEC=bin/hci_test

RED=\e[0;31m
NEUTRAL=\e[0;m
BLUE=\e[34m
BOLD=\033[1m

${EXEC}: ${OBJ}
	@printf "${BOLD}>> Creating the executable file...${NEUTRAL}\n"
	$(CC) $^ -o ${EXEC} $(CFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@printf "${BOLD}>> Generating the object file \"$@\"...${NEUTRAL}${RED}\n"
	@$(CC) -c $< -o $@ $(CFLAGS)
	@printf "${NEUTRAL}"

clean:
	@printf "${BOLD}>> Removing all object files...${NEUTRAL}\n"
	@rm -rfv ${OBJ}
	@printf "${BOLD}>> Removing all temporary files...${NEUTRAL}\n"
	$(shell find . -type f -name "*~" -delete)

mrproper: clean
	@printf "${BOLD}>> Removing the executable file...${NEUTRAL}\n"
	@rm -fv ${EXEC}
	@printf "${BOLD}>> Removing the dependencies file...${NEUTRAL}\n"
	@rm -fv .depend

test: ${EXEC}
	@$(shell ./RunTest)

depend: .depend

.depend: $(SRC) $(HDR)
	@printf "${BOLD}>> Creating the dependencies file...${NEUTRAL}\n"
	@rm -f ./.depend
	$(CC) $(CFLAGS) -MM $^>>./.depend;

-include .depend