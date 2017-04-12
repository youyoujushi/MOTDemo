#include "YYJSTrackerManager.h"

#define MAX_CONSECUTIVE_INVISIBLE_COUNT 50  //当跟踪器连续没有跟踪到目标次数超过该值时，删除跟踪器
#define TRACKER_VALID_VISIBLE_RATE    0.6   //当被跟踪目标的跟踪率小于这个值时，删除跟踪器

YYJSTrackerManager::YYJSTrackerManager()
{
    nextTrackerID = 1;
}


void YYJSTrackerManager::initKalmanFilter(KalmanFilter& KF){

    int stateNum=4;                                      //状态值4×1向量(x,y,r)
    int measureNum=2;                                    //测量值2×1向量(x,y,r)
    RNG rng;
    KF.init(stateNum,measureNum);
    KF.transitionMatrix = (Mat_<float>(stateNum, stateNum) <<1 ,0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1);  //转移矩阵A
    KF.measurementMatrix =  (Mat_<float>( measureNum,stateNum) <<1,0,0,0,0,1,0,0 );//(Mat_<float>( measureNum,stateNum) <<1,0 ,0 ,0, 0 ,0 ,1, 0);  //测量矩阵H
//    setIdentity(KF.processNoiseCov);                            //系统噪声方差矩阵Q
//    setIdentity(KF.measurementNoiseCov);                        //测量噪声方差矩阵R
    setIdentity(KF.processNoiseCov, Scalar::all(1e-5));                            //系统噪声方差矩阵Q
    setIdentity(KF.measurementNoiseCov, Scalar::all(1e-1));                        //测量噪声方差矩阵R
    setIdentity(KF.errorCovPost);                                  //后验错误估计协方差矩阵P
//    rng.fill(KF.statePost,RNG::UNIFORM,0,1);   //初始状态值x(0)

}


void YYJSTrackerManager::updateTracker(vector<Rect2f> detectedObjects,vector<int>& trackerIds,Mat frame){
    if(detectedObjects.size() == 0)
        return;

    this->frame = frame;

    for(int i=0;i<trackers.size();i++){
        ST_TRACK_INFO *tracker = &trackers[i];
        Mat prediction = tracker->kf.predict();
        tracker->findBound = tracker->tr->update(frame,tracker->tr_bound);
    }

    //匹配目标到跟踪器
//    vector<int> assignedTrackerIndex;
    for(int i=0;i<detectedObjects.size();i++){
        Rect2f object = detectedObjects[i];
        int trackerId = assignObjectToTracker(object);
        if(trackerId == -1)
            trackerId = assignObjectToTracker2(object);
        if(trackerId >= 0){
//            assignedTrackerIndex.push_back(trackerIndex);
            trackerIds.push_back(trackerId);
        }else
            trackerIds.push_back(-1);
    }


    for(int i=0;i<trackers.size();i++){
        bool assigned = false;
        for(int j =0;j<trackerIds.size();j++){
            if(trackerIds[j] == trackers[i].id && trackerIds[j] != -1){
                assigned = true;
                break;
            }
        }
        //更新没有匹配对象的跟踪器状态
        if(!assigned){
                trackers[i].age++;
                trackers[i].consecutiveInvisibleCount++;
        }
    }

    //删除无效的跟踪器
    deleteInvalidTrackers();

    //对未匹配到跟踪器的对象创建新的跟踪器
    for(int i=0;i<trackerIds.size();i++){
        if(trackerIds[i] == -1){
           ST_TRACK_INFO newTracker = createTracker(detectedObjects[i]);
           trackerIds[i] = newTracker.id;
        }
    }
}



ST_TRACK_INFO YYJSTrackerManager::createTracker(Rect2f bound){
    ST_TRACK_INFO tracker;
    initKalmanFilter(tracker.kf);
    tracker.id      = nextTrackerID++;
    tracker.age     = 1;
    tracker.bound   = bound;
    tracker.consecutiveInvisibleCount   = 0;
    tracker.totalVisibleCount           = 1;
    double center_x = bound.x + bound.width/2;
    double center_y = bound.y + bound.height/2;
    Mat measurement = Mat::zeros(4, 1, CV_32F);
    measurement.at<float>(0) = center_x;
    measurement.at<float>(1) = center_y;
    tracker.kf.statePost = measurement;
//    tracker.lastPt.x = center_x;
//    tracker.lastPt.y = center_y;

    tracker.tr = Tracker::create( "KCF" );
    Rect rect = bound;
    tracker.tr->init(frame,rect);

    trackers.push_back(tracker);
    return tracker;
}

void YYJSTrackerManager::deleteInvalidTrackers(){
    vector<ST_TRACK_INFO>::iterator i = trackers.begin();
    while(i != trackers.end()){

        ST_TRACK_INFO *tracker = &(*i);
        double visibleRate = (double)tracker->totalVisibleCount / (double)tracker->age;
        //删除长时间连续未跟踪到目标的跟踪器，删除跟踪率小于阙值的跟踪器
        if(tracker->consecutiveInvisibleCount >= MAX_CONSECUTIVE_INVISIBLE_COUNT )
//                ||(tracker->age < 8 && visibleRate < TRACKER_VALID_VISIBLE_RATE) )
            i=trackers.erase(i);
        else
            i++;
    }
}

int YYJSTrackerManager::assignObjectToTracker(Rect2f object){

    double minDis = -1;
    int id = -1;
    int index = -1;
    for(int i=0;i<trackers.size();i++){
        ST_TRACK_INFO tracker = trackers[i];
        KalmanFilter kf = tracker.kf;
        Mat prediction;

        float predict_x = kf.statePre.at<float>(0);//tracker.predictPt.x;//                //预测的坐标
        float predict_y = kf.statePre.at<float>(1);//tracker.predictPt.y;//kf.statePre.at<float>(1);
        float cur_x    = tracker.bound.width/2+tracker.bound.x;    //当前的坐标
        float cur_y    = tracker.bound.height/2+tracker.bound.y;

        Rect2f rect = object;
        float object_x = rect.x + rect.width/2;
        float object_y = rect.y + rect.height/2;
        float dx = object_x-predict_x;
        float dy = object_y-predict_y;
        float dis = sqrt(dx*dx+dy*dy);
        //把与预测位置最近的目标匹配给跟踪器
        if((minDis == -1 || minDis > dis) && dis < 50 ){// && dis <= radius){
            minDis = dis;
            id = trackers[i].id;
            index = i;
        }
    }

    if(id == -1){
        return -1;
    }else{
      //匹配到跟踪器，更新跟踪器信息
        Mat measurement = Mat::zeros(2, 1, CV_32F);
        float center_x = object.x + object.width/2;
        float center_y = object.y + object.height/2;
        measurement.at<float>(0) = center_x;
        measurement.at<float>(1) = center_y;
        trackers[index].kf.correct(measurement);
        trackers[index].age++;
        trackers[index].consecutiveInvisibleCount   = 0;
        trackers[index].totalVisibleCount++;
        Rect2f lastBound = trackers[index].bound;
        trackers[index].bound = object;

        return id;
    }
}


int YYJSTrackerManager::assignObjectToTracker2(Rect2f object){

    double minDis = -1;
    int id = -1;
    int index = -1;
    for(int i=0;i<trackers.size();i++){
        ST_TRACK_INFO tracker = trackers[i];
        if(!tracker.findBound)
            continue;

        float tr_x = tracker.tr_bound.x+tracker.tr_bound.width/2;//kf.statePre.at<float>(0);                //预测的坐标
        float tr_y = tracker.tr_bound.y+tracker.tr_bound.height/2;//kf.statePre.at<float>(1);


        Rect2f rect = object;
        float object_x = rect.x + rect.width/2;
        float object_y = rect.y + rect.height/2;
        float dx = object_x-tr_x;
        float dy = object_y-tr_y;
        float dis = sqrt(dx*dx+dy*dy);
        if((minDis == -1 || minDis > dis) && dis < 20 ){// && dis <= radius){
            minDis = dis;
            id = trackers[i].id;
            index = i;
        }
    }

    if(id == -1){
//        createTracker(object);  //没有找到匹配的跟踪器，创建一个新的
//        return trackers.size()-1;
        return -1;
    }else{
        Mat measurement = Mat::zeros(2, 1, CV_32F);
        float center_x = object.x + object.width/2;
        float center_y = object.y + object.height/2;
        measurement.at<float>(0) = center_x;
        measurement.at<float>(1) = center_y;
//        trackers[index].kf.correct(measurement);
        trackers[index].age++;
        trackers[index].consecutiveInvisibleCount   = 0;
        trackers[index].totalVisibleCount++;
        Rect2f lastBound = trackers[index].bound;
//        trackers[index].lastPt.x = lastBound.x + lastBound.width/2;
//        trackers[index].lastPt.y = lastBound.y + lastBound.height/2;
        trackers[index].bound = object;

        return id;
    }
}

ST_TRACK_INFO YYJSTrackerManager::findTracker(int id){
    for(int i=0;i<trackers.size();i++){
        if(id == trackers[i].id){
            return trackers[i];
        }
    }
    return ST_TRACK_INFO();
}
