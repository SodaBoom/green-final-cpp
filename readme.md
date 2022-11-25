# 0. 启动一个mariadb容器
```sh
docker pull mariadb:5.5.64
mkdir -p /data/mariadb/data
docker run --name mariadb -p 3306:3306 -e MYSQL_ROOT_PASSWORD=111111 -v /data/mariadb/data:/var/lib/mysql -d mariadb:5.5.64
# 确认容器启动
docker ps -a
docker container update --restart=always 容器id
docker exec -it 容器id bash
```

```sh
# 连接数据库
mysql -h127.0.0.1 -uroot -p111111
```

```sql
CREATE DATABASE atec2022;
use atec2022;
create table total_energy
    (
    id           int auto_increment
    primary key,
    gmt_create   datetime    null,
    gmt_modified datetime    null,
    user_id      varchar(64) null,
    total_energy int         null,
    constraint total_energy_pk
    unique (user_id)
    );
create table to_collect_energy
    (
    id                int auto_increment
    primary key,
    gmt_create        timestamp   null,
    gmt_modified      timestamp   null,
    user_id           varchar(64) null,
    to_collect_energy int         null,
    status            varchar(32) null
    );
```

确认表结构正确：
```sql
desc total_energy;
desc to_collect_energy;
```

# 1. 创建镜像
```sh
sh package.sh
```

# 2. run镜像
```sh
# 这么搞有点暴力
# 使用 --network=host，此时，Docker 容器的网络会附属在主机上，两者是互通的。
# 例如，在容器中运行一个Web服务，监听8080端口，则主机的8080端口就会自动映射到容器中。
sudo docker run -dit --network=host green-final /bin/bash  # 启动一个green-final
# sudo docker run -dit green-final /bin/bash
```

# 3. 进入镜像
```sh
sudo docker exec -it 容器id bash
```
