PROJ_NAME=af_converter

SOURCE=lib src
C_SOURCE=$(foreach d,$(SOURCE),$(wildcard $(addprefix $(d)/*,.c)))

INCLUDE=include
H_SOURCE=$(foreach d,$(INCLUDE),$(wildcard $(addprefix $(d)/*,.h)))

OBJ=$(subst .c,.o,$(C_SOURCE))

CC=gcc

PARAMS=
LDFLAGS=-c $(PARAMS) -L$(INCLUDE) -O2 -W -Wall -ansi -pedantic -std=c11
CFLAGS=$(PARAMS) -W -Wall -ansi -pedantic -std=c11

all: $(PROJ_NAME)

$(PROJ_NAME): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

obj: $(OBJ)

%.o: %.c
	$(CC) $(LDFLAGS) $< -o $@

clean:
	rm -rf lib/*.o src/*.o *.o $(PROJ_NAME) *~

show:
	@echo 'INCLUDE                     :' $(INCLUDE)
	@echo 'LIB                         :' $(LIB)
	@echo 'PROJ_NAME                   :' $(PROJ_NAME)
	@echo 'C_SOURCE                    :' $(C_SOURCE)
	@echo 'H_SOURCE                    :' $(H_SOURCE)
	@echo 'OBJ                         :' $(OBJ)
	@echo 'LDFLAGS                     :' $(LDFLAGS)
	@echo 'CFLAGS                      :' $(CFLAGS)
	@echo 'H_TEST                      :' $(H_TEST)

.PHONY: all clean show
