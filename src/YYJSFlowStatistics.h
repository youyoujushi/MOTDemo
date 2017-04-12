#pragma once
#include "global.h"
#include "YYJSTrackerManager.h"
#include <time.h>

/**
流量统计类
*/

enum EN_DIRECTION{
    DIRECTION_NONE=0,
    DIRECTION_LEFT,
    DIRECTION_TOP,
    DIRECTION_RIGHT,
    DIRECTION_BOTTOM
};

enum EN_FLOW_DIRECTION{
    EN_FLOW_NONE,   //未分配方向
    EN_FLOW_IN,     //进
    EN_FLOW_OUT     //出
};

struct ST_TRACKER_FLOW_INFO{
    int                 trackerId;  //跟踪器的id
    EN_FLOW_DIRECTION   direction;  //跟踪器的方向
    bool                inCounted;      //该跟踪器是否已统计入数
    bool                outCounted;     //该跟踪器是否已统计出数
};

struct ST_STATISTICS_INFO{
    int             inCount;        //总流入数
    int             outCount;       //总流出数
    time_t          beginCountTime; //开始计数的时间
    time_t          endCountTime; //停止计数的时间
};

class YYJSFlowStatistics
{
private:
    Rect            countWindow;//计数窗口
    EN_DIRECTION    in_direction;   //计数窗口的哪个方向标记为进
    EN_DIRECTION    out_direction;  //计数窗口的哪个方向标记为出
    bool            isCounting;     //是否正在计数
    ST_STATISTICS_INFO      statistics_info;
    YYJSTrackerManager*     pTrackerManager;
    vector<ST_TRACKER_FLOW_INFO>    trackerFlowInfos;

public:
	YYJSFlowStatistics(void);
	~YYJSFlowStatistics(void);

    void create(YYJSTrackerManager* pManager,Rect countWindow,EN_DIRECTION in=DIRECTION_NONE,EN_DIRECTION out=DIRECTION_NONE);

    void setDirection(EN_DIRECTION in,EN_DIRECTION out);

    bool beginCount();

    /**
     * @brief count 每次调用该函数，就检查所有在统计窗口内的跟踪器，统计流量
     *  流入数统计规则：
     *      1、已标注方向为入的目标
     *      2、目标在入方向还没有统计过
     *  流出数统计规则：
     *      1、已标注方向为出的目标
     *      2、目标在出方向还没有统计过
     * 流入方向标注规则：
     *      1、目标在窗口内部的入口附近，且还没有标注过方向
     * 流出方向标注规则：
     *      1、目标在窗口内部的出口附近，且还没有标注过方向
     */
    void count();

    void stopCount();

    bool getStatisticsInfo(ST_STATISTICS_INFO &info);
};
