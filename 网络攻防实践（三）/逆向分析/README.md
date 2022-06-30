# 过程

打开文件

![image-20220511183428395](README.assets/image-20220511183428395.png)

打开时的界面

![image-20220511183517900](README.assets/image-20220511183517900.png)

选中图中黄色区域 按TAB 再双击main 进入下图

![image-20220511184510806](README.assets/image-20220511184510806.png)

双击sub_40100A 再双击JUMOUT里的401400 进入下图

![image-20220511184721312](README.assets/image-20220511184721312.png)

然后在option里把number of opcode bytes改成6

![image-20220511184823154](README.assets/image-20220511184823154.png)

![image-20220511184836615](README.assets/image-20220511184836615.png)

然后在004014B2的E8前按D

![image-20220511185006151](README.assets/image-20220511185006151.png)

再在E9前按C

![image-20220511185044783](README.assets/image-20220511185044783.png)

在C7前按C

![image-20220511185106347](README.assets/image-20220511185106347.png)

把从00401400开始的红色.text部分选中 按P 然后按TAB 反编译 如下图

![image-20220511190510361](README.assets/image-20220511190510361.png)

进入401005

![image-20220511190612275](README.assets/image-20220511190612275.png)

进入401600

找到会出现栈溢出的strcpy函数

![image-20220511190650378](README.assets/image-20220511190650378.png)

然后按shift+f12 进入string 再用ctrl+f 搜索flag

![image-20220511190817449](README.assets/image-20220511190817449.png)

双击黄色区域 再按TAB

![image-20220511191133151](README.assets/image-20220511191133151.png)

右键sub_4012E0 jump to xref 然后再来一次jump to xref

得到下图

![image-20220511192029681](README.assets/image-20220511192029681.png)

然后再选中所有红色.text的行 按P 再按TAB键

![image-20220511192315038](README.assets/image-20220511192315038.png)

找到输出想要的结果的函数  地址是401230 后面要用

然后在这里设置断点

![image-20220511192601980](README.assets/image-20220511192601980.png)

测试文件

![image-20220511194441676](README.assets/image-20220511194441676.png)

打开 win32_remote.exe  

![image-20220511193527879](README.assets/image-20220511193527879.png)

![image-20220511193643163](README.assets/image-20220511193643163.png)

开始测试

![image-20220511193715263](README.assets/image-20220511193715263.png)

断点触发

![image-20220511193731749](README.assets/image-20220511193731749.png)

按空格

![image-20220511193848280](README.assets/image-20220511193848280.png)

![image-20220511193925352](README.assets/image-20220511193925352.png)

![image-20220511194927642](README.assets/image-20220511194927642.png)

按F8下一步

发现31313131覆盖了0019FC54处原本的值

![image-20220511194724376](README.assets/image-20220511194724376.png)

![image-20220511194825916](README.assets/image-20220511194825916.png)

所以我们这里进行更改即可 把前面的40 12 30 放过来

![image-20220511195133084](README.assets/image-20220511195133084.png)

测试成功

![img](README.assets/bf390c5cb52bc325e81b298b0b5b15bd.png)