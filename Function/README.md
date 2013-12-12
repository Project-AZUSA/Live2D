Live2D组件函数表
=============


添加模型
--------
### 函数
UI_AddModel(path)

### 参数
1：要添加的模型路径

### 注意事项
添加模型需保证上一次添加的模型已添加完毕（返回非-1），若返回-1表示上次添加的模型未添加，这是由于模型添加不能在渲染中添加，需等一次渲染结束后添加

删除所有模型
--------
### 函数
UI_RemoveModels()

### 参数
无参数

### 注意事项
更换模型请先清除再添加，官方例程也是如此操作，其原因不明

获得模型的JSON路径
--------
### 函数
UI_GetModelPath(modelindex)

### 参数
1：模型索引（模型索引均从0开始，下同）

设置表情
--------
### 函数
UI_SetExpression(exp_name,modelindex)

### 参数
1：表情名称
2：模型索引

### 使用方法
待写

设置动作
--------
### 函数
UI_SetMotion(motion_name,motion_index,modelindex)

### 参数
1：动作类型名称
2：动作索引
3：模型索引

### 使用方法
动作类型名称来自于JSON文件
如JSON内有以下内容：
  "motions":
  {
   "idle":
    [
     {"file":"motions/idle/haru_idle_01.mtn" ,"fade_in":2000, "fade_out":2000},
     {"file":"motions/idle/haru_idle_02.mtn" ,"fade_in":2000, "fade_out":2000},
     {"file":"motions/idle/haru_idle_03.mtn" ,"fade_in":2000, "fade_out":2000}
    ],
   ………………
那么这里有一个类型名称idle，索引0、1、2分别对应haru_idle_01.mtn，haru_idle_02.mtn，haru_idle_03.mtn
如要使第一个角色播放haru_idle_01.mtn
则运行

		UI_SetMotion(idle,0,0)


设置模型参数
--------
### 函数
UI_SetParameter(para_name,para_val,para_weight,modelindex)

### 参数
1：参数名称
2：参数值
3：参数权值
4：模型索引

### 使用方法
待写

### 注意事项
会覆盖，试用阶段

设置嘴部参数
--------
### 函数
UI_SetMouthOpen(val,modelindex)

### 参数
1：参数值（一般在0~1间）
2：模型索引

### 使用方法
		UI_SetMouthOpen(0.5,0)

播放音频
--------
### 函数
UI_PlaySound(path,modelindex)

### 参数
1：wav文件路径(预置的wav文件在res\sound)
2：模型索引

### 使用方法
相对路径：

		UI_PlaySound(res\sound\1.wav,0)
		
绝对路径：

		UI_PlaySound(C:\1.wav,0)


### 注意事项
现阶段不允许用该函数同时播放多个音频
音频格式仅限Wav
		
停止音频播放
--------
终止使用UI_PlaySound播放的音频
### 函数
UI_StopSound()

### 参数
无参数

设置眼睛朝向
--------
### 函数
UI_SetEyeBalls(eye_X,eye_Y,modelindex)

### 参数
1：眼睛X（一般在-1~1）
2：眼睛Y（一般在-1~1）
3：模型索引

### 使用方法
		UI_SetEyeBalls(0.5,-0.5,0)
		
设置身体朝向
--------
### 函数
UI_SetBody(body_X,modelindex)

### 参数
1：身体X方向（一般在-1~1）
2：模型索引

### 使用方法
		UI_SetBody(-0.5,0)
		
设置头部(脸)朝向
--------
### 函数
UI_SetFace(face_X,face_Y,face_Z,modelindex)

### 参数
1：脸X（一般在-1~1）
2：脸Y（一般在-1~1）
3：脸Z（一般在-1~1）
4：模型索引

### 使用方法
		UI_SetFace(1,-1,1,0)

### 注意事项
现阶段暂时不要只修改X，Y，Z的一个值。

设置头部(脸)朝向
--------
### 函数
UI_ShowMessage(x,y,width,height,msg,fontHeight,fontWidth,fontWeight,italic,family,color)

### 参数
1：文本框X
2：文本框Y
3：文本框宽
4：文本框高
5：消息
6：字体高
7：字体宽
8：字体粗
9：斜提（0/1）
10：字体
11：颜色ARGB(0xFF000000)

### 使用方法
		UI_ShowMessage(20,20,200,40,Hello,25,10,1,0,微软雅黑,0xFF000000)

### 注意事项
新的命令会将旧的显示抹去