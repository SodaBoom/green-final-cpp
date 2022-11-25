执行脚本编译mariadb-conn，注意，一定要是`source`，不能是`sh`
```sh
$ source build.sh
```

之后继续编译该项目
```
$ mkdir -p build && cd build
$ cmake ..
$ make -j4
```