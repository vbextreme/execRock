#Easy Makefile

APPPRJNAME = execRock
LIBPRJNAME = 
HMASTER    = 
LIBSO      = 
LIBA       =
APPIPATH   = /usr/bin/
IHPATH     = /usr/include/
APPCFLAGS  = -Wall -Wextra -O2
APPDFLAGS  = -Wall -Wextra -O2
APPDEFINE:= -D_APP

APPCC      = gcc
APPLK      = gcc
CFILE      = $(shell echo *.c)
HFILE      = $(shell echo *.h)
APPOBJPATH = ./build/
APPOFILE   = $(patsubst %.c,$(APPOBJPATH)%.o,$(CFILE)) 


all: ./build $(APPPRJNAME)
	@echo end make $(APPPRJNAME)

$(APPPRJNAME): $(APPOFILE)
	$(APPLK) $(APPDFLAGS) -o $(APPPRJNAME) $(APPOFILE) $(LIBA) $(LIBSO)

$(APPOBJPATH)%.o: %.c $(HFILE)
	$(APPCC) $(APPCFLAGS) -c $< -o $@ $(APPDEFINE) 

./build:
	@mkdir -p build
	
dbg:
	@make APPDEFINE="-DDEBUG_ENABLE=1 -DASSERT_ENABLE=1"

clean:
	rm -rf $(APPOBJPATH)*.o
	rm -rf ./$(APPPRJNAME)

install:
	@cp ./$(APPPRJNAME) /usr/bin/$(APPPRJNAME)
	@chown root:root /usr/bin/$(APPPRJNAME)
	@chmod ug=swrx,o=sx /usr/bin/$(APPPRJNAME)
	@echo '# app @ fromUser > Touser' > ./execRock.conf
	@cp -n ./execRock.conf /etc/execRock.conf
	@rm ./execRock.conf
	@chown root:root /etc/execRock.conf
	@chmod ug=wrx,o-rwx /etc/execRock.conf
	
uninstall:
	@rm /usr/bin/$(APPPRJNAME)
	@rm /etc/execRock.conf

fakeinstall:
	@chown root:root ./$(APPPRJNAME)
	@chmod ug=swrx,o=sx ./$(APPPRJNAME)
	@echo '# app @ fromUser > Touser\napt-get@odroid>root' > /etc/execRock.conf
	@chown root:root /etc/execRock.conf
	@chmod ug=wrx,o-rwx /etc/execRock.conf
	
fakeuninstall:
	@rm ./$(APPPRJNAME)
	@rm /etc/execRock.conf
