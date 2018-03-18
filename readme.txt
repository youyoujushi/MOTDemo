简介：
该程序是一个基于OpenCV的视频车流客流统计演示程序，以GPL协议发布。demo是在linux平台下开发的，
要想在windows平台运行，需要链接bgslibrary的windows库文件，使用者请自行下载编译。

引用算法介绍：
1、混合高斯模型的前景检测
2、卡尔曼滤波的位置估计跟踪结合‘KCF’跟踪算法
3、使用计数窗口的统计策略

使用的第三方开源库
OpenCV3.2   - https://github.com/opencv/opencv
bgslibrary  - https://github.com/andrewssobral/bgslibrary

编译方法
cd build
cmake ..
make


使用方法
1、cd build
2、./MOTDemo
3、启动后用鼠标在窗口中框出需要统计流量的窗口位置，选好后，按任意键继续
4、在命令行输入流入方向标识（1、2、3、4为矩形的左上右下四个方向），按回车确认
5、在命令行输入流入方向标识（1、2、3、4为矩形的左上右下四个方向），按回车确认。
  如流入方向设为1,流出方向设为3，代表从矩形左边出来的统计为流入数，从矩形右边出来的统计为流出数。
6、设置好流入流出方向后，程序就开始统计从计数窗口流过的目标数，并在计数窗口上边显示，in为流入数，out为流出数。

注意事项：
1、config目录要和可执行文件在同一目录下，config目录是背景抽离算法要读取的配置文件目录。
2、使用了c++11标准，如果c++编译器不支持c++11，请自行修改BgsNameEnum相关代码

车流统计演示视频1：http://www.iqiyi.com/w_19rtxuhhdl.html
车流统计演示视频2：http://www.iqiyi.com/w_19rtxuhi59.html 
客流统计演示视频3：http://www.iqiyi.com/w_19rtxuhmt9.html

结语
  如果我的demo对您有借鉴意义，我将感到非常荣幸，也希望您能给一些反馈或者改进建议。我本人对智能机器人领域非常感兴趣
，也希望能结交一些志同道合的朋友。QQ:幽幽居士（550335242）微信：w550335242
