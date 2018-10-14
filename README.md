# 多进程Demo
类似于Chrome浏览器，多进程按照Tab的方式显示

## 进程间通讯机制

主进程启动之后，创建一个LoaclServer，使用随机uuid作为server名称。

创建Tab时启动子进程，将server名称传递给子进程，子进程启动后，创建LoclaSocket来连接server。


