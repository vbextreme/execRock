#execRock v0.1
run application as another user without requiring a password</br>

Released to the GPL v3

## How To
### Install
```
$ git clone https://github.com/vbextreme/execRock.git
$ cd execRock/src
$ make
$ sudo make install
```

### Uninstall
```
$ sudo make uninstall
```

### Usage
exeRock looking "/etc/execRock.conf", find applications that can run from current user and executes according to the user specified.</br>
if you want add application or script you can edit .conf file</br>
```
# #Application name with full path
# [/usr/binapplication]
# #from @user call or group
# from = @fromUser
# #set user:group
# to = user:group
# #Argument set with regex 'regex' and need match count
# arg = '0' '1' '2' ...
# #execute in chroot, no, tmp, /dir
# chroot = no
```


example, execute apt-get</br>
add in /etc/execRock.conf
```
[/usr/bin/apt-get]
from = @myuser
to = root:root
arg = 'up(date|grade)'
chroot = no
```

now call
```
$ execRock apt-get update
```

run for max N milliseconds
```
$ execRock -t 1000 apt-get update
```

example, execute ls in a chroot</br>
add in /etc/execRock.conf
```
[/bin/ls]
from = @myuser
to = unprivilegeUser:unprivilegeGroup
arg = 
chroot = tmp
```

now call
```
$ execRock ls
```
and this execute in chroot
