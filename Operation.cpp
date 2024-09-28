#include "Operation.h"

#include <iostream>
#include <set>

#include "Util.h"
#include "neuron_editing/neuron_format_converter.h"

void Operation::addseg(const QString msg)
{
    QStringList pointlistwithheader=msg.split(',',Qt::SkipEmptyParts);
    if(pointlistwithheader.size()<1){
        std::cerr<<"ERROR:pointlistwithheader.size<1\n";
    }

    QStringList headerlist=pointlistwithheader[0].split(' ',Qt::SkipEmptyParts);
    if(headerlist.size()<2) {
        std::cerr<<"ERROR:headerlist.size<1\n";
    }
    unsigned int clienttype=headerlist[0].toUInt();

    int useridx=headerlist[1].toUInt();
    int isBegin=headerlist[2].toUInt();

    QStringList pointlist=pointlistwithheader;
    pointlist.removeAt(0);
    if(pointlist.size()==0){
        std::cerr<<"ERROR:pointlist.size=0\n";
        return;
    }
    XYZ point1,point2;

    QStringList pointlist_1=pointlist[0].split(' ',Qt::SkipEmptyParts);
    point1.x=pointlist_1[1].toFloat();
    point1.y=pointlist_1[2].toFloat();
    point1.z=pointlist_1[3].toFloat();
    for(int i=0;i<pointlist.size();i++)
    {
        if(pointlist[i]=="$"){
            QStringList pointlist_2=pointlist[i-1].split(' ',Qt::SkipEmptyParts);
            point2.x=pointlist_2[1].toFloat();
            point2.y=pointlist_2[2].toFloat();
            point2.z=pointlist_2[3].toFloat();
            break;
        }
    }

    auto addnt=convertMsg2NT(pointlist,clienttype,useridx,1);
    auto segs=NeuronTree__2__V_NeuronSWC_list(addnt).seg;
//    segs[0].printInfo();

//    bool flag;
//    if(fabs(point1.x-segs[0].row[0].x)<1e-4&&fabs(point1.y-segs[0].row[0].y)<1e-4&&fabs(point1.z-segs[0].row[0].z)<1e-4)
//        flag=true;
//    else
//        flag=false;

    mutex.lock();
//    segments.append(NeuronTree__2__V_NeuronSWC_list(addnt).seg[0]);
    bool isNeedReverse = false;
    if(segs.size()==2){
        int comparedIndex=0;
        if(isBegin==1){
            comparedIndex=segs[0].row.size()-1;
        }else if(isBegin==0){
            comparedIndex=0;
        }

        auto it=findseg(segments.seg.begin(),segments.seg.end(),segs[1]);
//        it->printInfo();
        if(it!=segments.seg.end())
        {
            int index = -1;
            double mindist = 5;
            for(int i=0;i<it->row.size();i++){
                double dist=distance(segs[0].row[comparedIndex].x,it->row[i].x,segs[0].row[comparedIndex].y,it->row[i].y,segs[0].row[comparedIndex].z,it->row[i].z);
                if(dist<mindist)
                {
                    mindist=dist;
                    index=i;
                }
            }
            if(index == -1){
                qDebug()<<"INFO:cannot find nearest point in first connected seg";
                qDebug()<<segs[0].row[comparedIndex].x<<" "<<segs[0].row[comparedIndex].y<<" "<<segs[0].row[comparedIndex].z;
                it->printInfo();
            }
            else{
                segs[0].row[comparedIndex].x=it->row[index].x;
                segs[0].row[comparedIndex].y=it->row[index].y;
                segs[0].row[comparedIndex].z=it->row[index].z;

                if(comparedIndex==0)
                    isNeedReverse=true;
            }
            if(isNeedReverse)
                reverseSeg(segs[0]);
        }
        else
        {
            std::cerr<<"INFO:not find connected seg ,"<<msg.toStdString()<<std::endl;
        }
    }

    if(segs.size()==3){
//        set<size_t> segIds1;
//        set<size_t> segIds2;
//        int firstIndex = -1;
//        int firstEndIndex = -1;
        auto it=findseg(segments.seg.begin(),segments.seg.end(),segs[1]);
//        it->printInfo();
        if(it!=segments.seg.end())
        {
            int index = -1;
            double mindist = 5;
            for(int i=0;i<it->row.size();i++){
                double dist=distance(segs[0].row[segs[0].row.size()-1].x,it->row[i].x,segs[0].row[segs[0].row.size()-1].y,it->row[i].y,segs[0].row[segs[0].row.size()-1].z,it->row[i].z);
                if(dist<mindist)
                {
                    mindist=dist;
                    index=i;
                }
            }
            if(index == -1){
                qDebug()<<"INFO:cannot find nearest point in first connected seg";
                qDebug()<<segs[0].row[segs[0].row.size()-1].x<<" "<<segs[0].row[segs[0].row.size()-1].y<<" "<<segs[0].row[segs[0].row.size()-1].z;
                it->printInfo();
            }
            else{
                segs[0].row[segs[0].row.size()-1].x=it->row[index].x;
                segs[0].row[segs[0].row.size()-1].y=it->row[index].y;
                segs[0].row[segs[0].row.size()-1].z=it->row[index].z;
//                float xLabel = it->row[index].x;
//                float yLabel = it->row[index].y;
//                float zLabel = it->row[index].z;
//                QString gridKeyQ = QString::number(xLabel) + "_" + QString::number(yLabel) + "_" + QString::number(zLabel);
//                string gridKey = gridKeyQ.toStdString();
//                map<string, set<size_t>> wholeGrid2SegIDMap = getWholeGrid2SegIDMap(segments);
//                segIds1 = wholeGrid2SegIDMap[gridKey];
//                firstIndex = index;
//                firstEndIndex = it->row.size()-1;
            }
        }
        else
        {
            std::cerr<<"INFO:not find connected seg ,"<<msg.toStdString()<<std::endl;
        }

        it=findseg(segments.seg.begin(),segments.seg.end(),segs[2]);
//        it->printInfo();
        if(it!=segments.seg.end())
        {
            int index = -1;
            double mindist = 5;
            for(int i=0;i<it->row.size();i++){
                double dist=distance(segs[0].row[0].x,it->row[i].x,segs[0].row[0].y,it->row[i].y,segs[0].row[0].z,it->row[i].z);
                if(dist<mindist)
                {
                    mindist=dist;
                    index=i;
                }
            }
            if(index == -1){
                qDebug()<<"INFO:cannot find nearest point in second connected seg";
                qDebug()<<segs[0].row[0].x<<" "<<segs[0].row[0].y<<" "<<segs[0].row[0].z;
                it->printInfo();
            }
            else{
                segs[0].row[0].x=it->row[index].x;
                segs[0].row[0].y=it->row[index].y;
                segs[0].row[0].z=it->row[index].z;
//                float xLabel = it->row[index].x;
//                float yLabel = it->row[index].y;
//                float zLabel = it->row[index].z;
//                QString gridKeyQ = QString::number(xLabel) + "_" + QString::number(yLabel) + "_" + QString::number(zLabel);
//                string gridKey = gridKeyQ.toStdString();
//                map<string, set<size_t>> wholeGrid2SegIDMap = getWholeGrid2SegIDMap(segments);
//                segIds2 = wholeGrid2SegIDMap[gridKey];
            }
        }
        else
        {
            std::cerr<<"INFO:not find connected seg ,"<<msg.toStdString()<<std::endl;
        }

//        if(segIds1.size()==1 && segIds2.size()==1 && firstIndex==firstEndIndex && firstEndIndex!=-1 )
//            isNeedReverse = true;
//        if(segIds1.size()==1 && segIds2.size()>1)
//            isNeedReverse = true;
//        if(isNeedReverse)
//            reverseSeg(segs[0]);
    }

    segments.append(segs[0]);

    mutex.unlock();

    qDebug()<<"server addseg";
//    for(int i=0;i<segments.seg.size();i++){
//        segments.seg[i].printInfo();
//    }
}

void Operation::addmanysegs(const QString msg){
    QStringList pointlistwithheader=msg.split(',',Qt::SkipEmptyParts);
    if(pointlistwithheader.size()<1){
        std::cerr<<"ERROR:pointlistwithheader.size<1\n";
    }

    QStringList headerlist=pointlistwithheader[0].split(' ',Qt::SkipEmptyParts);
    if(headerlist.size()<2) {
        std::cerr<<"ERROR:headerlist.size<2\n";
    }
    unsigned int clienttype=headerlist[0].toUInt();
    int useridx=headerlist[1].toUInt();

    QStringList pointlist=pointlistwithheader;
    pointlist.removeAt(0);
    if(pointlist.size()==0){
        std::cerr<<"ERROR:pointlist.size=0\n";
        return;
    }

    auto addnt=convertMsg2NT(pointlist,clienttype,useridx,1);
    auto segs=NeuronTree__2__V_NeuronSWC_list(addnt).seg;

    mutex.lock();

    for(auto seg:segs){
        segments.append(seg);
    }

    mutex.unlock();

    qDebug()<<"server addmanysegs";
}

void Operation::delseg(const QString msg)
{
    QStringList pointlistwithheader=msg.split(',',Qt::SkipEmptyParts);
    if(pointlistwithheader.size()<1){
        std::cerr<<"ERROR:pointlistwithheader.size<1\n";
    }

    QStringList headerlist=pointlistwithheader[0].split(' ',Qt::SkipEmptyParts);
    if(headerlist.size()<2) {
        std::cerr<<"ERROR:headerlist.size<1\n";
    }
    int useridx=headerlist[1].toUInt();
    unsigned int clienttype=headerlist[0].toUInt();
    unsigned int isMany=0;
    if(headerlist.size()>=6)
        isMany=headerlist[5].toUInt();

    QStringList pointlist=pointlistwithheader;
    pointlist.removeAt(0);
    if(pointlist.size()==0){
        std::cerr<<"ERROR:pointlist.size=0\n";
        return;
    }
    auto delnt=convertMsg2NT(pointlist,clienttype,useridx,isMany);
    auto delsegs=NeuronTree__2__V_NeuronSWC_list(delnt).seg;

    int count=0;
    mutex.lock();
    for(int i=0;i<delsegs.size();i++){
        auto it=findseg(segments.seg.begin(),segments.seg.end(),delsegs[i]);
        if(it!=segments.seg.end())
        {
            segments.seg.erase(it);
            if(count<5)
                qDebug()<<"server delseg";
            count++;
        }
        else
            std::cerr<<"INFO:not find del seg ,"<<msg.toStdString()<<std::endl;

    }

    mutex.unlock();
}

void Operation::connectseg(const QString msg){
    qDebug()<<msg;
    QStringList pointlistwithheader=msg.split(',',Qt::SkipEmptyParts);
    if(pointlistwithheader.size()<1){
        std::cerr<<"ERROR:pointlistwithheader.size<1\n";
    }

    QStringList headerlist=pointlistwithheader[0].split(' ',Qt::SkipEmptyParts);
    if(headerlist.size()<2) {
        std::cerr<<"ERROR:headerlist.size<1\n";
    }
    unsigned int clienttype=headerlist[0].toUInt();
    int useridx=headerlist[1].toUInt();

    QStringList pointlist=pointlistwithheader;
    pointlist.removeAt(0);
    if(pointlist.size()==0){
        std::cerr<<"ERROR:pointlist.size=0\n";
        return;
    }

    QStringList specPointsInfo1=pointlist[0].split(' ',Qt::SkipEmptyParts);
    QStringList specPointsInfo2=pointlist[1].split(' ',Qt::SkipEmptyParts);
    XYZ p1=XYZ(specPointsInfo1[0].toFloat(), specPointsInfo1[1].toFloat(), specPointsInfo1[2].toFloat());
    XYZ p2=XYZ(specPointsInfo2[0].toFloat(), specPointsInfo2[1].toFloat(), specPointsInfo2[2].toFloat());
    pointlist.removeAt(0);
    pointlist.removeAt(0);

    auto segnt=convertMsg2NT(pointlist,clienttype,useridx,1);
    auto connectsegs=NeuronTree__2__V_NeuronSWC_list(segnt).seg;
//    for(int i=0;i<connectsegs.size();i++){
//        connectsegs[i].printInfo();
//    }

    vector<segInfoUnit> segInfo;

    QMutexLocker locker(&mutex);
    for(int i=0;i<connectsegs.size();i++){
        auto it=findseg(segments.seg.begin(),segments.seg.end(),connectsegs[i]);
        if(it!=segments.seg.end())
        {
            //父子关系逆序
            if (it->row.begin()->data[6] != 2) // Sort the node numbers of involved segments
            {
                int nodeNo = 1;
                for (vector<V_NeuronSWC_unit>::iterator it_unit = it->row.begin();
                     it_unit != it->row.end(); it_unit++)
                {
                    it_unit->data[0] = nodeNo;
                    it_unit->data[6] = nodeNo + 1;
                    ++nodeNo;
                }
                (it->row.end() - 1)->data[6] = -1;
            }

            //构造segInfo
            if(segInfo.size()==0){
                double mindist=5;
                vector<V_NeuronSWC_unit>::iterator it_res=it->row.end();
                for (vector<V_NeuronSWC_unit>::iterator it_unit = it->row.begin();
                     it_unit != it->row.end(); it_unit++)
                {
                    //if (p1.x == it_unit->data[2] && p1.y == it_unit->data[3] && p1.z == it_unit->data[4])
                    double dist=distance(p1.x,it_unit->data[2],p1.y,it_unit->data[3],p1.z,it_unit->data[4]);
                    if(dist<mindist)
                    {
                        mindist=dist;
                        it_res=it_unit;
                    }
                }
                if(it_res==it->row.end()){
                    qDebug()<<"cannot find nearest point in first to be connected seg";
                    qDebug()<<p1.x<<" "<<p1.y<<" "<<p1.z;
                    it->printInfo();
                }
                else{
                    //---------------------- Get seg IDs
                    //qDebug() << nodeOnStroke->at(j).seg_id << " " << nodeOnStroke->at(j).parent << " " << p.x() << " " << p.y();
                    qDebug()<<p1.x<<" "<<p1.y<<" "<<p1.z;
                    qDebug()<<it_res->data[2]<<" "<<it_res->data[3]<<" "<<it_res->data[4];
                    segInfoUnit curSeg;
                    curSeg.head_tail = it_res->data[6];
                    curSeg.segID = it-segments.seg.begin();
                    curSeg.nodeCount = it->row.size();
                    curSeg.refine = false;
                    curSeg.branchID = it->branchingProfile.ID;
                    curSeg.paBranchID = it->branchingProfile.paID;
                    curSeg.hierarchy = it->branchingProfile.hierarchy;
                    segInfo.push_back(curSeg);
                }

            }
            else if(segInfo.size()==1){
                double mindist=5;
                vector<V_NeuronSWC_unit>::iterator it_res=it->row.end();
                for (vector<V_NeuronSWC_unit>::iterator it_unit = it->row.begin();
                     it_unit != it->row.end(); it_unit++)
                {
                    double dist=distance(p2.x,it_unit->data[2],p2.y,it_unit->data[3],p2.z,it_unit->data[4]);
                    if(dist<mindist)
                    {
                        mindist=dist;
                        it_res=it_unit;
                    }
                }

                if(it_res==it->row.end()){
                    qDebug()<<"cannot find nearest point in second to be connected seg";
                    qDebug()<<p2.x<<" "<<p2.y<<" "<<p2.z;
                    it->printInfo();
                }
                else{
                    //---------------------- Get seg IDs
                    //qDebug() << nodeOnStroke->at(j).seg_id << " " << nodeOnStroke->at(j).parent << " " << p.x() << " " << p.y();
                    qDebug()<<p2.x<<" "<<p2.y<<" "<<p2.z;
                    qDebug()<<it_res->data[2]<<" "<<it_res->data[3]<<" "<<it_res->data[4];
                    segInfoUnit curSeg;
                    curSeg.head_tail = it_res->data[6];
                    curSeg.segID = it-segments.seg.begin();
                    curSeg.nodeCount = it->row.size();
                    curSeg.refine = false;
                    curSeg.branchID = it->branchingProfile.ID;
                    curSeg.paBranchID = it->branchingProfile.paID;
                    curSeg.hierarchy = it->branchingProfile.hierarchy;
                    segInfo.push_back(curSeg);
                    qDebug()<<"second connect seg found";
                }

            }

        }

        else{
            std::cerr<<"INFO:not find connect seg ,"<<msg.toStdString()<<std::endl;
//            mutex.unlock();
            return;
        }
    }

    simpleConnectExecutor(segments, segInfo);

    if (segments.seg[segInfo[0].segID].to_be_deleted)
    {
        qDebug()<<"enter tracedNeuron.seg[segInfo[0]]";
        vector<V_NeuronSWC> connectedSegDecomposed = decompose_V_NeuronSWC(segments.seg[segInfo[1].segID]);
        qDebug()<<"connectedSegDecomposed_size: "<<connectedSegDecomposed.size();
        for (vector<V_NeuronSWC>::iterator addedIt = connectedSegDecomposed.begin(); addedIt != connectedSegDecomposed.end(); ++addedIt)
            segments.seg.push_back(*addedIt);

        segments.seg[segInfo[1].segID].to_be_deleted = true;
        segments.seg[segInfo[1].segID].on = false;

    }
    else if (segments.seg[segInfo[1].segID].to_be_deleted)
    {
        qDebug()<<"enter tracedNeuron.seg[segInfo[1]]";
        vector<V_NeuronSWC> connectedSegDecomposed = decompose_V_NeuronSWC(segments.seg[segInfo[0].segID]);
        for (vector<V_NeuronSWC>::iterator addedIt = connectedSegDecomposed.begin(); addedIt != connectedSegDecomposed.end(); ++addedIt)
            segments.seg.push_back(*addedIt);

        segments.seg[segInfo[0].segID].to_be_deleted = true;
        segments.seg[segInfo[0].segID].on = false;
    }

    std::vector<V_NeuronSWC>::iterator iter = segments.seg.begin();
    while (iter != segments.seg.end())
        if (iter->to_be_deleted)
            iter = segments.seg.erase(iter);
        else
            ++iter;

//    auto addnt=convertMsg2NT(pointlist,clienttype,useridx,0);
//    segments.append(NeuronTree__2__V_NeuronSWC_list(addnt).seg[0]);
    qDebug()<<"server connectseg";
}

void Operation::splitseg(const QString msg){
    QStringList pointlistwithheader=msg.split(',',Qt::SkipEmptyParts);
    if(pointlistwithheader.size()<1){
        std::cerr<<"ERROR:pointlistwithheader.size<1\n";
    }

    QStringList headerlist=pointlistwithheader[0].split(' ',Qt::SkipEmptyParts);
    if(headerlist.size()<2) {
        std::cerr<<"ERROR:headerlist.size<1\n";
    }
    int useridx=headerlist[1].toUInt();
    unsigned int clienttype=headerlist[0].toUInt();

    QStringList pointlist=pointlistwithheader;
    pointlist.removeAt(0);
    if(pointlist.size()==0){
        std::cerr<<"ERROR:pointlist.size=0\n";
        return;
    }

    auto tempnt=convertMsg2NT(pointlist,clienttype,useridx,1);
    auto segs=NeuronTree__2__V_NeuronSWC_list(tempnt).seg;

    if(segs.size()<=2)
        return;

    XYZ point1,point2;

    QMutexLocker locker(&mutex);
    auto it=findseg(segments.seg.begin(),segments.seg.end(),segs[0]);
    if(it!=segments.seg.end())
    {
        point1.x=it->row[0].x;
        point1.y=it->row[0].y;
        point1.z=it->row[0].z;
        point2.x=it->row[it->row.size()-1].x;
        point2.y=it->row[it->row.size()-1].y;
        point2.z=it->row[it->row.size()-1].z;
        segments.seg.erase(it);
    }

    else
    {
        std::cerr<<"INFO:not find del seg ,"<<msg.toStdString()<<std::endl;
        return;
    }

    for(int i=1;i<segs.size();i++){
        if(distance(segs[i].row[0].x,point1.x,segs[i].row[0].y,point1.y,segs[i].row[0].z,point1.z)<0.3){
            segs[i].row[0].x=point1.x;
            segs[i].row[0].y=point1.y;
            segs[i].row[0].z=point1.z;
        }
        if(distance(segs[i].row[0].x,point2.x,segs[i].row[0].y,point2.y,segs[i].row[0].z,point2.z)<0.3){
            segs[i].row[0].x=point2.x;
            segs[i].row[0].y=point2.y;
            segs[i].row[0].z=point2.z;
            if(segs[i].row[0].parent!=-1){
                reverse(segs[i].row.begin(),segs[i].row.end());
                //父子关系逆序
                int nodeNo = 1;
                for (vector<V_NeuronSWC_unit>::iterator it_unit = segs[i].row.begin();
                     it_unit != segs[i].row.end(); it_unit++)
                {
                    it_unit->data[0] = nodeNo;
                    it_unit->data[6] = nodeNo + 1;
                    ++nodeNo;
                }
                (segs[i].row.end() - 1)->data[6] = -1;
            }
        }
        if(distance(segs[i].row[segs[i].row.size()-1].x,point2.x,segs[i].row[segs[i].row.size()-1].y,point2.y,segs[i].row[segs[i].row.size()-1].z,point2.z)<0.3){
            segs[i].row[segs[i].row.size()-1].x=point2.x;
            segs[i].row[segs[i].row.size()-1].y=point2.y;
            segs[i].row[segs[i].row.size()-1].z=point2.z;
        }
        if(distance(segs[i].row[segs[i].row.size()-1].x,point1.x,segs[i].row[segs[i].row.size()-1].y,point1.y,segs[i].row[segs[i].row.size()-1].z,point1.z)<0.3){
            segs[i].row[segs[i].row.size()-1].x=point1.x;
            segs[i].row[segs[i].row.size()-1].y=point1.y;
            segs[i].row[segs[i].row.size()-1].z=point1.z;
            if(segs[i].row[0].parent!=-1){
                reverse(segs[i].row.begin(),segs[i].row.end());
                //父子关系逆序
                int nodeNo = 1;
                for (vector<V_NeuronSWC_unit>::iterator it_unit = segs[i].row.begin();
                     it_unit != segs[i].row.end(); it_unit++)
                {
                    it_unit->data[0] = nodeNo;
                    it_unit->data[6] = nodeNo + 1;
                    ++nodeNo;
                }
                (segs[i].row.end() - 1)->data[6] = -1;
            }
        }
        segments.append(segs[i]);
    }

    qDebug()<<"server splitseg";
}

void Operation::retypesegment(const QString msg)
{
    QStringList pointlistwithheader=msg.split(',',Qt::SkipEmptyParts);
    if(pointlistwithheader.size()<1){
        std::cerr<<"ERROR:pointlistwithheader.size<1\n";
    }

    QStringList headerlist=pointlistwithheader[0].split(' ',Qt::SkipEmptyParts);
    if(headerlist.size()<3) {
        std::cerr<<"ERROR:headerlist.size<1\n";
    }

    unsigned int clienttype=headerlist[0].toUInt();
    int useridx=headerlist[1].toUInt();
    unsigned int newcolor=headerlist[2].toUInt();
    unsigned int isMany=0;
    if(headerlist.size()>=7)
        isMany=headerlist[6].toUInt();

    QStringList pointlist=pointlistwithheader;
    pointlist.removeAt(0);
    if(pointlist.size()==0){
        std::cerr<<"ERROR:pointlist.size=0\n";
        return;
    }

    auto retypent=convertMsg2NT(pointlist,clienttype,useridx,isMany);
    auto retypesegs=NeuronTree__2__V_NeuronSWC_list(retypent).seg;

    int count=0;

    QMutexLocker locker(&mutex);
    for(int i=0;i<retypesegs.size();i++){
        auto it=findseg(segments.seg.begin(),segments.seg.end(),retypesegs[i]);
        if(it==segments.seg.end()){
            std::cerr<<"INFO:not find retype seg ,"<<msg.toStdString()<<std::endl;
            //            mutex.unlock();
            return;
        }
        int now=QDateTime::currentMSecsSinceEpoch();
        for(auto &unit:it->row){
            unit.type=newcolor;
            unit.level=now-unit.timestamp;
            unit.creatmode=useridx*10+clienttype;
        }
        if(count<5)
            qDebug()<<"server retypesegment";
        count++;
    }
}

void Operation::addmarkers(const QString msg)
{
    qDebug()<<msg;
    QStringList pointlistwithheader=msg.split(',',Qt::SkipEmptyParts);
    if(pointlistwithheader.size()<1){
        std::cerr<<"ERROR:pointlistwithheader.size<1\n";
    }

    QStringList headerlist=pointlistwithheader[0].split(' ',Qt::SkipEmptyParts);
    if(headerlist.size()<2) {
        std::cerr<<"ERROR:headerlist.size<1\n";
    }

    unsigned int clienttype=headerlist[0].toUInt();
    int useridx=headerlist[1].toUInt();

    QStringList pointlist=pointlistwithheader;
    pointlist.removeAt(0);
    if(pointlist.size()==0){
        std::cerr<<"ERROR:pointlist.size=0\n";
        return;
    }

    CellAPO marker;
    marker.name="";
    marker.comment="";
    marker.orderinfo="";

    QMutexLocker locker(&mutex);
    for(auto &msg:pointlist){
        auto markerinfo=msg.split(' ',Qt::SkipEmptyParts);
        if(markerinfo.size()!=6) continue;
        marker.color.r=markerinfo[0].toUInt();
        marker.color.g=markerinfo[1].toUInt();
        marker.color.b=markerinfo[2].toUInt();
        marker.x=markerinfo[3].toDouble();
        marker.y=markerinfo[4].toDouble();
        marker.z=markerinfo[5].toDouble();

//        for(auto it=markers.begin();it!=markers.end(); ++it)
//        {
//            if(fabs(it->x-marker.x)<1&&fabs(it->y-marker.y)<1&&fabs(it->z-marker.z)<1)
//            {
//                qDebug()<<"the marker has already existed";
////                mutex.unlock();
//                return;
//            }
//        }

        markers.append(marker);
        qDebug()<<"server addmarker";
    }
}

void Operation::delmarkers(const QString msg)
{
    QStringList pointlistwithheader=msg.split(',',Qt::SkipEmptyParts);
    if(pointlistwithheader.size()<1){
        std::cerr<<"ERROR:pointlistwithheader.size<1\n";
    }

    QStringList headerlist=pointlistwithheader[0].split(' ',Qt::SkipEmptyParts);
    if(headerlist.size()<2) {
        std::cerr<<"ERROR:headerlist.size<1\n";
    }

    unsigned int clienttype=headerlist[0].toUInt();
    int useridx=headerlist[1].toUInt();

    QStringList pointlist=pointlistwithheader;
    pointlist.removeAt(0);
    if(pointlist.size()==0){
        std::cerr<<"ERROR:pointlist.size=0\n";
        return;
    }
    CellAPO marker;
    int idx=-1;

    QMutexLocker locker(&mutex);
    for(auto &msg:pointlist){
        auto markerinfo=msg.split(' ',Qt::SkipEmptyParts);
        if(markerinfo.size()!=6) continue;
        marker.color.r=markerinfo[0].toUInt();
        marker.color.g=markerinfo[1].toUInt();
        marker.color.b=markerinfo[2].toUInt();
        marker.x=markerinfo[3].toDouble();
        marker.y=markerinfo[4].toDouble();
        marker.z=markerinfo[5].toDouble();
//        if(isSomaExists&&sqrt((marker.x-somaCoordinate.x)*(marker.x-somaCoordinate.x)+
//                (marker.y-somaCoordinate.y)*(marker.y-somaCoordinate.y)+
//                (marker.z-somaCoordinate.z)*(marker.z-somaCoordinate.z))<1)
//        {
//            qDebug()<<"cannot delete the soma marker";
////            mutex.unlock();
//            return;
//        }
        idx=findnearest(marker,markers);
        if(idx!=-1) {
            markers.removeAt(idx);
            qDebug()<<"server delmarker";
        }
        else{
            std::cerr<<"find marker failed."+msg.toStdString()+"\n";
        }
    }
}

void Operation::retypemarker(const QString msg){
    QStringList pointlistwithheader=msg.split(',',Qt::SkipEmptyParts);
    if(pointlistwithheader.size()<1){
        std::cerr<<"ERROR:pointlistwithheader.size<1\n";
    }

    QStringList headerlist=pointlistwithheader[0].split(' ',Qt::SkipEmptyParts);
    if(headerlist.size()<2) {
        std::cerr<<"ERROR:headerlist.size<1\n";
    }

    unsigned int clienttype=headerlist[0].toUInt();
    int useridx=headerlist[1].toUInt();

    QStringList pointlist=pointlistwithheader;
    pointlist.removeAt(0);
    if(pointlist.size()==0){
        std::cerr<<"ERROR:pointlist.size=0\n";
        return;
    }
    CellAPO marker;
    int idx=-1;

    QMutexLocker locker(&mutex);
    for(auto &msg:pointlist){
        auto markerinfo=msg.split(' ',Qt::SkipEmptyParts);
        if(markerinfo.size()!=6) continue;
        marker.color.r=markerinfo[0].toUInt();
        marker.color.g=markerinfo[1].toUInt();
        marker.color.b=markerinfo[2].toUInt();
        marker.x=markerinfo[3].toDouble();
        marker.y=markerinfo[4].toDouble();
        marker.z=markerinfo[5].toDouble();
//        if(isSomaExists&&sqrt((marker.x-somaCoordinate.x)*(marker.x-somaCoordinate.x)+
//                                 (marker.y-somaCoordinate.y)*(marker.y-somaCoordinate.y)+
//                                 (marker.z-somaCoordinate.z)*(marker.z-somaCoordinate.z))<1)
//        {
//            qDebug()<<"cannot delete the soma marker";
//            return;
//        }
        idx=findnearest(marker,markers);
        if(idx!=-1) {
            markers[idx].color.r=marker.color.r;
            markers[idx].color.g=marker.color.g;
            markers[idx].color.b=marker.color.b;
            qDebug()<<"server retypemarker";
        }
        else{
            std::cerr<<"find marker failed."+msg.toStdString()+"\n";
        }
    }
}

void Operation::simpleConnectExecutor(V_NeuronSWC_list& segments, vector<segInfoUnit>& segInfo)
{

    qDebug()<<"begin to simpleConnectExecutor";
    // This method is the "executor" of Renderer_gl1::simpleConnect(), MK, May, 2018

    //////////////////////////////////////////// HEAD TAIL CONNECTION ////////////////////////////////////////////
    if ((segInfo.at(0).head_tail == -1 || segInfo.at(0).head_tail == 2) && (segInfo.at(1).head_tail == -1 || segInfo.at(1).head_tail == 2))
    {
        segInfoUnit mainSeg, branchSeg;
        if (segInfo.at(0).nodeCount >= segInfo.at(1).nodeCount)
        {
            mainSeg = segInfo.at(0);
            branchSeg = segInfo.at(1);
            qDebug() << "primary seg length:" << mainSeg.nodeCount << "   primary seg orient:" << mainSeg.head_tail;
            qDebug() << "secondary seg length:" << branchSeg.nodeCount << "   secondary seg orient:" << branchSeg.head_tail;
        }
        else
        {
            mainSeg = segInfo.at(1);
            branchSeg = segInfo.at(0);
            qDebug() << "primary seg length:" << mainSeg.nodeCount << "   primary seg orient:" << mainSeg.head_tail;
            qDebug() << "secondary seg length:" << branchSeg.nodeCount << "   secondary seg orient:" << branchSeg.head_tail;
        }

        double assignedType;
        assignedType = segments.seg[segInfo.at(0).segID].row[0].type;
        segments.seg[mainSeg.segID].row[0].seg_id = mainSeg.segID;
        //        qDebug()<<"zll___debug__mainSeg.head_tail"<<mainSeg.head_tail;
        if (mainSeg.head_tail == -1)
        {
            //            qDebug()<<"(zll-debug)branchSeg.head_tail="<<branchSeg.head_tail;
            if (branchSeg.head_tail == -1) // head to head
            {
                for (vector<V_NeuronSWC_unit>::iterator itNextSeg = segments.seg[branchSeg.segID].row.end() - 1;
                     itNextSeg >= segments.seg[branchSeg.segID].row.begin(); --itNextSeg)
                {
                    itNextSeg->seg_id = branchSeg.segID;
                    segments.seg[mainSeg.segID].row.push_back(*itNextSeg);
                }
            }
            else if (branchSeg.head_tail == 2) // head to tail
            {
                for (vector<V_NeuronSWC_unit>::iterator itNextSeg = segments.seg[branchSeg.segID].row.begin();
                     itNextSeg != segments.seg[branchSeg.segID].row.end(); ++itNextSeg)
                {
                    itNextSeg->seg_id = branchSeg.segID;
                    segments.seg[mainSeg.segID].row.push_back(*itNextSeg);
                }
            }
            segments.seg[branchSeg.segID].to_be_deleted = true;
            segments.seg[branchSeg.segID].on = false;

            // sorting the new segment here, and reassign the root node to the new tail
            size_t nextSegNo = 1;
            for (vector<V_NeuronSWC_unit>::iterator itSort = segments.seg[mainSeg.segID].row.begin();
                 itSort != segments.seg[mainSeg.segID].row.end(); ++itSort)
            {
                itSort->data[0] = nextSegNo;
                itSort->data[6] = itSort->data[0] + 1;
                ++nextSegNo;
            }
            (segments.seg[mainSeg.segID].row.end() - 1)->data[6] = -1;
        }
        else if (mainSeg.head_tail == 2)
        {
            std::reverse(segments.seg[mainSeg.segID].row.begin(), segments.seg[mainSeg.segID].row.end());
            //            qDebug()<<"zll___debug__2_branchSeg.head_tail"<<branchSeg.head_tail;
            if (branchSeg.head_tail == -1) // tail to head
            {
                for (vector<V_NeuronSWC_unit>::iterator itNextSeg = segments.seg[branchSeg.segID].row.end() - 1;
                     itNextSeg >= segments.seg[branchSeg.segID].row.begin(); itNextSeg--)
                {
                    itNextSeg->seg_id = branchSeg.segID;
                    segments.seg[mainSeg.segID].row.push_back(*itNextSeg);
                }
            }
            else if (branchSeg.head_tail == 2) // tail to tail
            {
                for (vector<V_NeuronSWC_unit>::iterator itNextSeg = segments.seg[branchSeg.segID].row.begin();
                     itNextSeg != segments.seg[branchSeg.segID].row.end(); itNextSeg++)
                {
                    itNextSeg->seg_id = branchSeg.segID;
                    segments.seg[mainSeg.segID].row.push_back(*itNextSeg);
                }
            }
            segments.seg[branchSeg.segID].to_be_deleted = true;
            segments.seg[branchSeg.segID].on = false;

            // sorting the new segment here, and reassign the root node to the new tail
            std::reverse(segments.seg[mainSeg.segID].row.begin(), segments.seg[mainSeg.segID].row.end());
            size_t nextSegNo = 1;
            for (vector<V_NeuronSWC_unit>::iterator itSort = segments.seg[mainSeg.segID].row.begin();
                 itSort != segments.seg[mainSeg.segID].row.end(); itSort++)
            {
                itSort->data[0] = nextSegNo;
                itSort->data[6] = itSort->data[0] + 1;
                ++nextSegNo;
            }
            (segments.seg[mainSeg.segID].row.end() - 1)->data[6] = -1;
        }

        // correcting types, based on the main segment type
        for (vector<V_NeuronSWC_unit>::iterator reID = segments.seg[mainSeg.segID].row.begin();
             reID != segments.seg[mainSeg.segID].row.end(); ++reID)
        {
            reID->seg_id = mainSeg.segID;
            reID->type = assignedType;
            //            qDebug()<<"zll_debug"<<reID->type;
        }
    }
    //////////////////////////////////////////// END of [HEAD TAIL CONNECTION] ////////////////////////////////////////////

    //////////////////////////////////////////// BRANCHING CONNECTION ////////////////////////////////////////////
    if ((segInfo.at(0).head_tail != -1 && segInfo.at(0).head_tail != 2) ^ (segInfo.at(1).head_tail != -1 && segInfo.at(1).head_tail != 2))
    {
        segInfoUnit mainSeg, branchSeg;
        if (segInfo.at(0).head_tail == -1 || segInfo.at(0).head_tail == 2)
        {
            mainSeg = segInfo.at(1);
            branchSeg = segInfo.at(0);
            qDebug() << "primary seg length:" << mainSeg.nodeCount << "   primary seg orient:" << mainSeg.head_tail;
            qDebug() << "secondary seg length:" << branchSeg.nodeCount << "   secondary seg orient:" << branchSeg.head_tail;
        }
        else
        {
            mainSeg = segInfo.at(0);
            branchSeg = segInfo.at(1);
            qDebug() << "primary seg length:" << mainSeg.nodeCount << "   primary seg orient:" << mainSeg.head_tail;
            qDebug() << "secondary seg length:" << branchSeg.nodeCount << "   secondary seg orient:" << branchSeg.head_tail;
        }

        double assignedType;
        assignedType = segments.seg[segInfo.at(0).segID].row[0].type;
        segments.seg[mainSeg.segID].row[0].seg_id = mainSeg.segID;
        if (branchSeg.head_tail == 2) // branch to tail
        {
            std::reverse(segments.seg[branchSeg.segID].row.begin(), segments.seg[branchSeg.segID].row.end());
            size_t branchSegLength = segments.seg[branchSeg.segID].row.size();
            size_t mainSegLength = segments.seg[mainSeg.segID].row.size();
            segments.seg[mainSeg.segID].row.insert(segments.seg[mainSeg.segID].row.end(), segments.seg[branchSeg.segID].row.begin(), segments.seg[branchSeg.segID].row.end());
            size_t branchN = mainSegLength + 1;
            for (vector<V_NeuronSWC_unit>::iterator itNextSeg = segments.seg[mainSeg.segID].row.end() - 1;
                 itNextSeg != segments.seg[mainSeg.segID].row.begin() + ptrdiff_t(mainSegLength - 1); --itNextSeg)
            {
                itNextSeg->n = branchN;
                itNextSeg->seg_id = mainSeg.segID;
                itNextSeg->parent = branchN - 1;
                ++branchN;
            }
            (segments.seg[mainSeg.segID].row.end() - 1)->parent = (segments.seg[mainSeg.segID].row.begin() + ptrdiff_t(mainSeg.head_tail - 2))->n;
            segments.seg[branchSeg.segID].to_be_deleted = true;
            segments.seg[branchSeg.segID].on = false;
        }
        else if (branchSeg.head_tail == -1) // branch to head
        {
            size_t branchSegLength = segments.seg[branchSeg.segID].row.size();
            size_t mainSegLength = segments.seg[mainSeg.segID].row.size();
            segments.seg[mainSeg.segID].row.insert(segments.seg[mainSeg.segID].row.end(), segments.seg[branchSeg.segID].row.begin(), segments.seg[branchSeg.segID].row.end());
            size_t branchN = mainSegLength + 1;
            for (vector<V_NeuronSWC_unit>::iterator itNextSeg = segments.seg[mainSeg.segID].row.end() - 1;
                 itNextSeg != segments.seg[mainSeg.segID].row.begin() + ptrdiff_t(mainSegLength - 1); --itNextSeg)
            {
                itNextSeg->n = branchN;
                itNextSeg->seg_id = mainSeg.segID;
                itNextSeg->parent = branchN - 1;
                ++branchN;
            }
            (segments.seg[mainSeg.segID].row.end() - 1)->parent = (segments.seg[mainSeg.segID].row.begin() + ptrdiff_t(mainSeg.head_tail - 2))->n;
            segments.seg[branchSeg.segID].to_be_deleted = true;
            segments.seg[branchSeg.segID].on = false;
        }

        // correcting types, based on the main segment type
        for (vector<V_NeuronSWC_unit>::iterator reID = segments.seg[mainSeg.segID].row.begin();
             reID != segments.seg[mainSeg.segID].row.end(); ++reID)
        {
            reID->seg_id = mainSeg.segID;
            reID->type = assignedType;
        }
    }
    //////////////////////////////////////////// END of [BRANCHING CONNECTION] ////////////////////////////////////////////

    return;
}