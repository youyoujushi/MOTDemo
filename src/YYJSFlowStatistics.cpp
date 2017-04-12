#include "YYJSFlowStatistics.h"

#define COUNT_WINDOW_INNER_SHIFT    20

YYJSFlowStatistics::YYJSFlowStatistics(void)
{
}


YYJSFlowStatistics::~YYJSFlowStatistics(void)
{
}


void YYJSFlowStatistics::create(YYJSTrackerManager* pManager,Rect countWindow,EN_DIRECTION in,EN_DIRECTION out){

    if(pManager == NULL)
        return;
    this->pTrackerManager = pManager;
    this->countWindow       = countWindow;
    this->in_direction      = in;
    this->out_direction     = out;
}

void YYJSFlowStatistics::setDirection(EN_DIRECTION in,EN_DIRECTION out){
    this->in_direction      = in;
    this->out_direction     = out;
}

bool YYJSFlowStatistics::beginCount(){
    if(isCounting || pTrackerManager == NULL)
        return false;
    memset(&this->statistics_info,0,sizeof(ST_STATISTICS_INFO));
    isCounting = true;
    time_t t;
    this->statistics_info.beginCountTime = time(&t);
}

void YYJSFlowStatistics::count(){
    if(!isCounting || pTrackerManager == NULL)
        return;
    for(int i=0;i<pTrackerManager->trackers.size();i++){
        ST_TRACK_INFO tracker = pTrackerManager->trackers[i];
        int center_x = tracker.bound.x + tracker.bound.width/2;
        int center_y = tracker.bound.y + tracker.bound.height/2;
        int inBorderDis,outBorderDis;

        if(in_direction == DIRECTION_LEFT){
            inBorderDis = abs(center_x - countWindow.x);
        }else if(in_direction == DIRECTION_TOP){
            inBorderDis = abs(center_y - countWindow.y);
        }else if(in_direction == DIRECTION_RIGHT){
            inBorderDis = abs(center_x - countWindow.x-countWindow.width);
        }else if(in_direction == DIRECTION_BOTTOM){
            inBorderDis = abs(center_y - countWindow.y-countWindow.height);
        }

        if(out_direction == DIRECTION_LEFT){
            outBorderDis = abs(center_x - countWindow.x);
        }else if(out_direction == DIRECTION_TOP){
            outBorderDis = abs(center_y - countWindow.y);
        }else if(out_direction == DIRECTION_RIGHT){
            outBorderDis = abs(center_x - countWindow.x-countWindow.width);
        }else if(out_direction == DIRECTION_BOTTOM){
            outBorderDis = abs(center_y - countWindow.y-countWindow.height);
        }

        bool inCountWindow = true;

        if(center_x < countWindow.x || center_x > countWindow.x + countWindow.width
                || center_y < countWindow.y || center_y > countWindow.y + countWindow.height){
                inCountWindow = false;//目标不在统计窗口内
        }

        int id = tracker.id;
        int index = -1;
        for(int j=0;j<trackerFlowInfos.size();j++){
            if(trackerFlowInfos[j].trackerId == id){
                index = j;
                break;
            }
        }
        ST_TRACKER_FLOW_INFO info;
        ST_TRACKER_FLOW_INFO& refInfo = info;
        if(index != -1){
            if(inCountWindow)
                continue;
            refInfo = trackerFlowInfos[index];
            if(inBorderDis < outBorderDis){//由入口线超出统计窗口
                if(refInfo.direction == EN_FLOW_IN)//如果设了注入标记，统计入数
                    statistics_info.inCount++;
            }else{//由出口线超出统计窗口
                if(refInfo.direction == EN_FLOW_OUT)//如果设了注出标记，统计出数
                    statistics_info.outCount++;
            }
            refInfo.direction = EN_FLOW_NONE;

            //删除掉超出统计窗口的跟踪器
            vector<ST_TRACKER_FLOW_INFO>::iterator i = trackerFlowInfos.begin();
            while(i != trackerFlowInfos.end()){
                ST_TRACKER_FLOW_INFO fi = *i;
                if(fi.trackerId == refInfo.trackerId )
                    i=trackerFlowInfos.erase(i);
                else
                    i++;
            }

        }else{
            if(!inCountWindow)
                continue;
            if(inBorderDis < outBorderDis)
                refInfo.direction = EN_FLOW_OUT;
            else
                refInfo.direction = EN_FLOW_IN;
            refInfo.trackerId = tracker.id;
            trackerFlowInfos.push_back(refInfo);
        }
    }
}

void YYJSFlowStatistics::stopCount(){
    if(!isCounting)
        return;
    isCounting = false;
    trackerFlowInfos.clear();
}

bool YYJSFlowStatistics::getStatisticsInfo(ST_STATISTICS_INFO &info){
    info.inCount        = statistics_info.inCount;
    info.outCount       = statistics_info.outCount;
    info.beginCountTime = statistics_info.beginCountTime;
    info.endCountTime   = statistics_info.endCountTime;
    return true;
}
