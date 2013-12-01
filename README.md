Live2D
======

搭載 AZUSA 使用的 Live2D 整合介面


Azusa-Live2D exe 接口

支持平台：

WIN XP：32（已测） 64

WIN7:32（已测） 64（已测）

Ver 1.2.2

实现了嘴型音频的同步，根据波形控制嘴型。

Ver 1.2.1

改成了exe实现，减少了环境依赖，增加了部分函数接口，可嵌入Azusa

以下代码调用控制台，注释后才可以嵌入Azusa

    ::AllocConsole();    // 打开控件台资源
    
    freopen("CONOUT$", "w+t", stdout);    // 申请写
    
  	freopen("CONIN$","r+t",stdin); // 申请读
  	
  	FreeConsole(); //关闭控制台
  	
  	函数接口详情见function.txt
