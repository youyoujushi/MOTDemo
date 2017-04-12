#include "global.h"
#include "YYJSBgs.h"
#include <time.h>
#include <opencv2/tracking.hpp>
#include "YYJSTrackerManager.h"
#include "roiSelector.hpp"
#include "YYJSFlowStatistics.h"

Rect g_drawRect;
bool drawing;
Point original_pos;

BgsNameEnum curBgsName;
MultiTrackerTLD *tracker = new MultiTrackerTLD();
YYJSTrackerManager tr;
YYJSFlowStatistics flowStatistics;

//根据命令行输入字符串，选择背景抽离算法
BgsNameEnum selBgsAlgorithm();

//标识轮廓
void markContour(Mat frame,Mat srcFrame,vector<Rect2f>& contourRects);



//显示统计信息
void displayStatisticsInfo(Mat& canvas,Rect countWindow,ST_STATISTICS_INFO info){
    if(info.beginCountTime == 0)
        return;

    time_t now;
    time(&now);
    int totalTime = now-info.beginCountTime;
    if (totalTime <= 0)
        return;

    float inRate = (float)info.inCount / totalTime;
    float outRate = (float)info.outCount / totalTime;
    char  str[100];
    sprintf(str,"in:%d/out:%d ",info.inCount,info.outCount);
    putText(canvas,str,Point(countWindow.x,countWindow.y),CV_FONT_HERSHEY_SIMPLEX,1,Scalar(255,0,0),2);
    rectangle(canvas,countWindow,Scalar(0,255,255),3);
}

int main(int argv ,char** argc){

	YYJSBgs bgs;
  curBgsName = BgsNameEnum::LBMixtureOfGaussians; //指定背景抽离算法
	bgs.createBgs(curBgsName);

	clock_t startTime;
    int fps = 0;	//֡帧率
    int delay = 50;
    Mat frame,foreground,background,canvas;

    VideoCapture *vc = new VideoCapture("../test_data/videos/videocut_4.mp4");//videocut_3.mp4");//(0);//
//      VideoCapture *vc = new VideoCapture("/home/youyoujushi/test_data/videos/Video of Traffic on Highway Free HD Footage Videezy.com.mp4");//(0);//
//    VideoCapture *vc = new VideoCapture("/home/youyoujushi/test_data/videos/Cars pass under an overpass - Free HD Video Clips & Stock Vi.mp4");
//   VideoCapture *vc = new VideoCapture("/home/youyoujushi/test_data/videos/Traffic - Free HD Video Clips & Stock Video Footage at Videe.mp4");
//   VideoCapture *vc = new VideoCapture("/home/youyoujushi/test_data/videos/trafficdb/video/cctv052x2004080516x01638.avi");

    for(int i=0;i<10;i++){
        *vc >> frame;
        imshow("原视频",frame);
        waitKey(1);
    }
    //! [getframe]
    //! [selectroi]
    Rect countWindow=selectROI("原视频",frame);


    /**********select direction**********/
    int direction;
    EN_DIRECTION in,out;
    while(1){
        cout<<"set in direction"<<endl;
        cin>>direction;
        if(direction == 1 || direction == 2 || direction == 3 || direction == 4)
            break;
    }
    in = (EN_DIRECTION)direction;
    while(1){
        cout<<"set out direction"<<endl;
        cin>>direction;
        if(direction == 1 || direction == 2 || direction == 3 || direction == 4)
            break;
    }
    out = (EN_DIRECTION)direction;
    /*******************/

    //创建流量分析对象
    flowStatistics.create(&tr,countWindow,in,out);
    //开启统计
    flowStatistics.beginCount();

	for(;;){
		*vc >> frame;


    canvas = frame.clone();
		startTime = clock();

		if(!frame.empty()){
            //medianBlur(frame,frame,3);
            blur(frame,frame,Size(3,3));

            //调用背景抽离算法，计算前景目标
			      bgs.process(frame,foreground,background);
            if(foreground.channels() == 3)
              cvtColor(foreground,foreground,CV_BGR2GRAY);

            /***************对前景二值图像进行几何形态学滤波去掉噪声***************/
            Mat element1 = getStructuringElement(MORPH_RECT,Size(5,5));
            Mat element2 = getStructuringElement(MORPH_RECT,Size(5,5));
            morphologyEx(foreground,foreground,MORPH_OPEN,element1);
            morphologyEx(foreground,foreground,MORPH_CLOSE,element2);
            /**********************************/



            /***************计算前景轮廓并绘制*************/
            vector<Rect2f> contourRects;
            markContour(foreground,canvas,contourRects);
            /**********************************/

            //跟踪目标
            vector<int> trackerIds;
            tr.updateTracker(contourRects,trackerIds,canvas);

            //绘制目标的包围矩形
            for(int i=0;i<contourRects.size();i++){
                Rect2f rect = contourRects[i];
                rectangle(canvas,rect,Scalar(0,255,255));
                int id = trackerIds[i];
                if(id >= 0){
                    char  idStr[10];
                    sprintf(idStr,"%d",id);
                    putText(canvas,idStr,Point(rect.x,rect.y),CV_FONT_VECTOR0,0.5,Scalar(255,0,0),2);
                }
            }

            //流量分析并显示
            flowStatistics.count();
            ST_STATISTICS_INFO info;
            flowStatistics.getStatisticsInfo(info);
            displayStatisticsInfo(canvas,countWindow,info);

            //显示图像
            imshow("原视频",canvas);
            imshow("前景窗口",foreground);
		}
		int end = clock();

		int pastTime = (clock()-startTime)*1000/CLOCKS_PER_SEC;//每一帧的处理时间
		if(pastTime>0)
			fps = 1000 / pastTime;

    //保证帧率不至于过快
		int k = 0;
		if(pastTime < delay){
			k = waitKey(delay-pastTime);
		}else
			k = waitKey(1);

    //接收用户命令
		switch (k)
		{
		case 'a':
			{
				cout<<"请选择背景分离算法"<<endl;
				BgsNameEnum name = selBgsAlgorithm();
				bgs.createBgs(name);
				curBgsName = name;
			}
			break;
		case 'f':
			{
				cout<<"请输入视频文件路径"<<endl;
				char filePath[256];
				cin.clear();
				cin.getline(filePath,256);
				VideoCapture *fileVC = new VideoCapture();
				if(!fileVC->open(filePath)){
					cout<<"视频文件打开失败"<<endl;
					delete fileVC;
				}else{
					delete vc;
					vc = fileVC;
					bgs.createBgs(curBgsName);
				}
			}
			break;
        case 's':
        {


        }
            break;
		case 'z':
			cout<<"当前帧率"<<fps<<"fps 延时"<<delay<<"ms"<<endl;
			break;
		case 'q':
		case 27:
			delete vc;
            return 0;
			break;
		default:
			break;
		}


	}
}

void markContour(Mat frame,Mat srcFrame,vector<Rect2f>& contourRects){

	Mat img = frame ;
  Mat contoursFrame(img.size(),CV_8UC3);
	vector<vector<Point>> contours;
	vector<Vec4i> hierachy;
  findContours(frame,contours,hierachy,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);

	if(contours.size() == 0)
		return;

	for(int index = 0;index>=0;index=hierachy[index][0]){

    int minContourPoints = 20;//只有轮廓点数量大于该值才计算轮廓
    if(index < contours.size() && contours[index].size() < minContourPoints){
			       continue;
		}

    //绘制轮廓
		drawContours(srcFrame,contours,index,Scalar(0,0,255),1,8,hierachy);
		// drawContours(contoursFrame,contours,index,Scalar(0,0,255),1,8,hierachy);

		//计算轮廓的包围矩形
    RotatedRect rect =  minAreaRect(contours[index]);
    contourRects.push_back(rect.boundingRect2f());
	}

}

void printAllAlgorithms(){
	cout<<"======背景抽离算法======\n \
	  [0]FrameDifferenceBGS\n \
	  [1]StaticFrameDifferenceBGS\n \
	  [2]WeightedMovingMeanBGS\n \
	  [3]WeightedMovingVarianceBGS\n \
	  [4]MixtureOfGaussianV1BGS\n \
	  [5]MixtureOfGaussianV2BGS\n \
	  [6]AdaptiveBackgroundLearning\n \
	  [7]AdaptiveSelectiveBackgroundLearning\n \
	  [8]GMG\n \
	  [9]DPAdaptiveMedianBGS\n \
	  [10]DPGrimsonGMMBGS\n \
	  [11]DPZivkovicAGMMBGS\n \
	  [12]DPMeanBGS\n \
	  [13]DPWrenGABGS\n \
	  [14]DPPratiMediodBGS\n \
	  [15]DPEigenbackgroundBGS\n \
	  [16]DPTextureBGS\n \
	  [17]T2FGMM_UM\n \
	  [18]T2FGMM_UV\n \
	  [19]T2FMRF_UM\n \
	  [20]T2FMRF_UV\n \
	  [21]FuzzySugenoIntegral\n \
	  [22]FuzzyChoquetIntegral\n \
	  [23]LBSimpleGaussian\n \
	  [24]LBFuzzyGaussian\n \
	  [25]LBMixtureOfGaussians\n \
	  [26]LBAdaptiveSOM\n \
	  [27]LBFuzzyAdaptiveSOM\n \
	  [28]MultiLayerBGS\n \
	  [29]VuMeter\n \
	  [30]KDE\n \
	  [31]IndependentMultimodalBGS\n \
	  [32]SJN_MultiCueBGS\n \
	  [33]SigmaDeltaBGS\n \
	  [34]SuBSENSEBGS\n \
	  [35]LOBSTERBGS" <<endl;
}

BgsNameEnum selBgsAlgorithm(){

	printAllAlgorithms();
	for(;;){
		cin.clear();
		char order[10];
		cin.getline(order,10);
		int algoIndex = atoi(order);
		if(order[0] != 'h'){
			if(algoIndex == 4 || algoIndex == 8 || algoIndex == 32){
				cout<<"该算法不支持 ，请选择其他的算法"<<endl;
				continue;
			}else if(algoIndex >= 0 && algoIndex <= LOBSTERBGS){
				return BgsNameEnum(algoIndex);
			}else
				return DPAdaptiveMedianBGS;
		}else{
			//打印帮助文件
			printAllAlgorithms();
		}
		cout<<"请选择算法[]"<<endl;
	}

}
