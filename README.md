Live2D
======

搭載 AZUSA 使用的 Live2D 整合介面

Live2D.exe为可嵌入Azusa程序

Live2D_Debug.exe为测试程序

Azusa-Live2D exe 接口

函数接口详情见Function/README.md

### 支持平台

WIN XP：32（已测） 64

WIN7:32（已测） 64（已测）


### 版本信息

Ver 1.2.9

增加鼠标跟踪

增加UI_LookAt

Ver 1.2.8

完善参数设置的使用与清空

Ver 1.2.7

添加StopSound函数用于停止播放

优化了嘴型音频的同步

Ver 1.2.6

修改Azusa进程检测机制为ProcessID检测

Ver 1.2.5

添加了UI_ShowMessage函数用于显示文本信息到UI

增加了Azusa进程检测机制，若无AZUSA.exe（大小写敏感）进程则程序关闭

Ver 1.2.4

实现了嘴型音频的同步，根据波形控制嘴型。

Ver 1.2.1

改成了exe实现，减少了环境依赖，增加了部分函数接口，可嵌入Azusa

### 调试需知

可通过更改CMD_DEBUG的值进行调试：0为关闭控制台，1为启用控制台
