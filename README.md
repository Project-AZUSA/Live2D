Live2D
======

搭載 AZUSA 使用的 Live2D 整合介面


Azusa-Live2D DLL 接口

支持平台：

WIN XP：32（已测） 64

WIN7:32（已测） 64（已测）

运行环境：

DLL：VC++ 2008 redist、DX9 redist （June 2010）

Demo：.net framework 3.5

Ver 1.0.2

1.封装了一些常用的基本函数，实现了对模型的简单控制

2.兼容了XP平台，将运行环境降到了vc2008

3.修复了加载、删除模型所引发的内存访问错误

使用方法

将Live2D.DLL、res文件（夹）放到与调用exe文件同一目录下

res文件加名称不可修改，其内部文件结构可以修改

res\config.txt为启动参数文件，共5个整数，用空格隔开，从左到右依次是：

窗体left 窗体top 窗体width 窗体height 加载模型数

res\model.txt为模型路径文件，格式为：

模型路径1;模型路径2;......模型路径n

n由config.txt中的加载模型数决定

具体函数调用方法参见Live2D_Demo中的例程

在项目中添加Live2D.cs类即可

接口参数的说明在Live2D.cs类中有说明
