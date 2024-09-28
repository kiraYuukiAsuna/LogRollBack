#pragma once
#include "basic_c_fun/basic_surf_objs.h"
#include "neuron_editing/v_neuronswc.h"


inline double distance(const CellAPO&m1, const CellAPO&m2) {
    return sqrt(
        (m1.x - m2.x) * (m1.x - m2.x) +
        (m1.y - m2.y) * (m1.y - m2.y) +
        (m1.z - m2.z) * (m1.z - m2.z)
    );
}

inline double distance(const double x1, const double x2, const double y1, const double y2, const double z1,
                       const double z2) {
    return sqrt(
        (x1 - x2) * (x1 - x2) +
        (y1 - y2) * (y1 - y2) +
        (z1 - z2) * (z1 - z2)
    );
}

inline double getSegLength(V_NeuronSWC&seg) {
    int size = seg.row.size();
    double sum = 0;
    for (int i = 0; i < size - 1; i++) {
        sum += distance(seg.row[i].x, seg.row[i + 1].x, seg.row[i].y, seg.row[i + 1].y, seg.row[i].z, seg.row[i + 1].z);
    }
    return sum;
}


inline NeuronTree convertMsg2NT(QStringList pointlist,int client,int user, int isMany, int mode=0)
{
    NeuronTree newTempNT;
    newTempNT.listNeuron.clear();
    newTempNT.hashNeuron.clear();
    int cnt=pointlist.size();
    int timestamp=QDateTime::currentMSecsSinceEpoch();
    if(isMany==0){
        for(int i=0;i<cnt;i++)
        {
            NeuronSWC S;
            QStringList nodelist=pointlist[i].split(' ',Qt::SkipEmptyParts);
            if(nodelist.size()<4) return NeuronTree();
            S.n=i+1;
            S.type=nodelist[0].toUInt();

            S.x=nodelist[1].toFloat();
            S.y=nodelist[2].toFloat();
            S.z=nodelist[3].toFloat();
            switch (mode) {
            case 0:S.r=user*10+client;break;
            case 1:S.r=user;break;
            case 2:S.r=client;break;
            }

            if(i==0) S.pn=-1;
            else S.pn=i;
            S.timestamp=timestamp;
            newTempNT.listNeuron.push_back(S);
            newTempNT.hashNeuron.insert(S.n,newTempNT.listNeuron.size());
        }
    }
    else if(isMany==1){
        int index=0;
        for(int i=0;i<cnt;i++)
        {
            if(pointlist[i]!="$"){
                NeuronSWC S;
                QStringList nodelist=pointlist[i].split(' ',Qt::SkipEmptyParts);
                if(nodelist.size()<4) return NeuronTree();
                S.n=i+1;
                S.type=nodelist[0].toUInt();

                S.x=nodelist[1].toFloat();
                S.y=nodelist[2].toFloat();
                S.z=nodelist[3].toFloat();
                switch (mode) {
                case 0:S.r=user*10+client;break;
                case 1:S.r=user;break;
                case 2:S.r=client;break;
                }

                if(index==0) S.pn=-1;
                else S.pn=i;
                S.timestamp=timestamp;
                newTempNT.listNeuron.push_back(S);
                newTempNT.hashNeuron.insert(S.n,newTempNT.listNeuron.size());
                index++;
            }
            else{
                index=0;
            }

        }
    }

    return newTempNT;
}


inline vector<V_NeuronSWC>::iterator findseg(vector<V_NeuronSWC>::iterator begin,vector<V_NeuronSWC>::iterator end,const V_NeuronSWC seg)
{
    vector<V_NeuronSWC>::iterator result=end;
    double mindist=5;
    const std::vector<V_NeuronSWC_unit>::size_type cnt=seg.row.size();

    while(begin!=end)
    {
        if(begin->row.size()==cnt)
        {
            double dist=0;
            for(std::vector<V_NeuronSWC_unit>::size_type i=0;i<cnt;i++)
            {
                auto node=begin->row.at(i);
                dist+=sqrt(
                           pow(node.x-seg.row[i].x,2)
                          +pow(node.y-seg.row[i].y,2)
                          +pow(node.z-seg.row[i].z,2)
                           );
            }
            if(dist/cnt<mindist)
            {
                mindist=dist/cnt;
                result=begin;
            }

            dist=0;
            for(std::vector<V_NeuronSWC_unit>::size_type i=0;i<cnt;i++)
            {
                auto node=begin->row.at(i);
                dist+=sqrt(
                           pow(node.x-seg.row[cnt-i-1].x,2)
                          +pow(node.y-seg.row[cnt-i-1].y,2)
                          +pow(node.z-seg.row[cnt-i-1].z,2)
                           );
            }
            if(dist/cnt<mindist)
            {
                mindist=dist/cnt;
                result=begin;
            }
        }
        begin++;
    }
    return result;
}

inline void reverseSeg(V_NeuronSWC& seg){
    reverse(seg.row.begin(), seg.row.end());
    for(int i=0; i<seg.row.size(); i++){
        seg.row[i].n=i+1;
        seg.row[i].parent=i+2;
    }
    seg.row[seg.row.size()-1].parent=-1;
}

inline int findnearest(const CellAPO &m,const QList<CellAPO> &markers)
{
    int index=-1;
    double thres=1;
    for(int i=0;i<markers.size();i++){
        if(distance(m,markers[i])<thres){
            index=i;
        }
    }
    return index;
}