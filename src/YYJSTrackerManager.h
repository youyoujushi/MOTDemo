#ifndef YYJSTrackerManager_H
#define YYJSTrackerManager_H

#include "global.h"
#include "opencv2/tracking.hpp"

struct ST_TRACK_INFO{
    Ptr<Tracker>    tr;
    bool            findBound;  //tr跟踪器是否找跟踪到了目标
    Rect2d          tr_bound;   //tr跟踪器跟踪到的目标的矩形
    Rect2f          bound;      //目标实际的位置
    int             id;         //跟踪器id
    KalmanFilter    kf;         //卡尔曼滤波器
    int             age;        //跟踪器存在的时长，每帧递增1,不管有没有跟踪到目标
    int             totalVisibleCount;  //从跟踪器创建到当前为止，总共跟踪到目标的帧数量
    int             consecutiveInvisibleCount;//连续未跟踪到目标的帧数，当跟踪到目标时，该值重置0
};

class YYJSTrackerManager
{

private:
    int nextTrackerID;
    Mat frame;
public:
    vector<ST_TRACK_INFO>   trackers;

public:
    YYJSTrackerManager();

    /**
     * @brief 更新各个跟踪器，该函数会执行三种操作，
     * 对于新检测到的对象，创建一个新的跟踪器；对于消失的对象，删除对应的跟踪器；对于已有的跟踪器，更新对象的当前位置
     * @param detectedObjects   检测到的对象列表
     * @param trackerIds        和对象匹配的跟踪器的id，和detectedObjects一一对应，如果没有匹配跟踪器，对应的id为-1
     * @param frame             当前帧
     */
    void updateTracker(vector<Rect2f> detectedObjects,vector<int>& trackerIds,Mat frame);//检测到的对象

    ST_TRACK_INFO findTracker(int id);

private:
    void initKalmanFilter(KalmanFilter& KF); //初始化卡尔曼过滤器


    //创建新的跟踪器
    ST_TRACK_INFO createTracker(Rect2f bound);

    ///删除失效的跟踪器
    void deleteInvalidTrackers();

    /**
     * @brief assignObjectToTracker    使用卡尔曼滤波匹配目标和跟踪器
     * @param object
     * @param trackers
     * @return 返回匹配的tracker在vector中的索引
     */
    int assignObjectToTracker(Rect2f object);

    //使用KCF跟踪算法匹配目标和跟踪器
    int assignObjectToTracker2(Rect2f object);
};

#endif // YYJSTrackerManager_H
