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
example, execute apt-get</br>
add in /etc/execRock.conf
```
# application @ fromUser > toUser
apt-get @ vbextreme > root
```

now call
```
$ execRock -c 'apt-get update'
```

run for max N milliseconds
```
$ execRock -tc 1000 'apt-get update'
```

the conf file accept regular expression for validate argument of a command.
```
# application | regexArgument@ fromUser > toUser
```

for example run only update or upgrade for apt
```
apt-get | up(date|grade)@ vbextreme > root
```

