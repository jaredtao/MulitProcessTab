# 多进程Demo
类似于Chrome浏览器，多进程按照Tab的方式显示

## Build status
| [Linux][lin-link] | [Windows][win-link] |
| :---------------: | :-----------------: |
| ![lin-badge]      | ![win-badge]        |

[lin-badge]: https://travis-ci.org/wentaojia2014/HelloCI.svg?branch=master "Travis build status"
[lin-link]: https://travis-ci.org/wentaojia2014/HelloCI "Travis build status"
[win-badge]: https://ci.appveyor.com/api/projects/status/yykx4xufxtrax1hi?svg=true "AppVeyor build status"
[win-link]: https://ci.appveyor.com/project/jiawentao/helloci "AppVeyor build status"

## License
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/wentaojia2014/TableEdit/blob/master/LICENSE)

## 预览图

主页

![](DemoImages/main.png)

Tab页

![](DemoImages/sub.png)

## 进程间通讯机制

主进程启动之后，创建一个LoaclServer，使用随机uuid作为server名称。

创建Tab时启动子进程，将server名称传递给子进程，子进程启动后，创建LoclaSocket来连接server。

## 开发环境

* Qt 5.12.x Windows/Ubuntu

#### 联系方式:
***
|作者|贾文涛|
|---|---|
|QQ|759378563|
|微信|xsd2410421|
|邮箱|jared2020@163.com|
|blog|https://wentaojia2014.github.io/|

###### 请放心联系我，乐于提供相关帮助
***
#### **打赏**
<img src="https://github.com/wentaojia2014/wentaojia2014.github.io/blob/master/img/weixin.jpg?raw=true" width="30%" height="30%" /><img src="https://github.com/wentaojia2014/wentaojia2014.github.io/blob/master/img/zhifubao.jpg?raw=true" width="30%" height="30%" />

###### 觉得分享的内容还不错, 就请作者喝杯奶茶吧~~
***