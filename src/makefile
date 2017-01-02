#Easy Makefile

APPPRJNAME = execRock
LIBPRJNAME = 
HMASTER    = 
LIBSO      = 
LIBA       =
APPIPATH   = /usr/bin/
IHPATH     = /usr/include/
APPCFLAGS  = -Wall -Wextra -O2
APPDFLAGS  = -Wall -Wextra -O2 -mcpu=native -mtune=native
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
	@echo '# named @user or group' > ./execRock.conf
	@echo '# not use chroot == no' >> ./execRock.conf
	@echo '# temp chroot == tmp' >> ./execRock.conf
	@echo '# preset chroot == /dir/dir' >> ./execRock.conf
	@echo '# ' >> ./execRock.conf
	@echo '# [/dir/application]' >> ./execRock.conf
	@echo '# from = @user ' >> ./execRock.conf
	@echo '# to = user:group' >> ./execRock.conf
	@echo "# arg = 'regex' 'regex' ... '" >> ./execRock.conf
	@echo '# chroot = no' >> ./execRock.conf
	@cp -n ./execRock.conf /etc/execRock.conf
	@rm ./execRock.conf
	@chown root:root /etc/execRock.conf
	@chmod ug=wr,o-rwx /etc/execRock.conf

clean-conf:
	@rm /etc/execRock.conf

uninstall:
	@rm /usr/bin/$(APPPRJNAME)
	@rm /etc/execRock.conf
