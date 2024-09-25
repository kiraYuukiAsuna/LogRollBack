#pragma once

#include "basic_c_fun/basic_surf_objs.h"
#include "neuron_editing/v_neuronswc.h"

struct segInfoUnit {
    segInfoUnit() { hierarchy = 0; }
    long segID;
    long head_tail;
    long nodeCount;
    bool refine;

    int branchID, paBranchID;
    int hierarchy;
};

class Operation {
public:
    V_NeuronSWC_list segments;
    QList<CellAPO> markers;

    QMutex mutex;

public:
    void addseg(const QString msg);//加线
    void addmanysegs(const QString msg); //加很多线
    void delseg(const QString msg);//减线
    void addmarkers(const QString msg);//加marker
    void delmarkers(const QString msg);//删marker
    void retypemarker(const QString msg);//marker改颜色
    void connectseg(const QString msg);//连线
    void retypesegment(const QString msg);//线改颜色
    void splitseg(const QString msg);//break seg


    void simpleConnectExecutor(V_NeuronSWC_list&segments, vector<segInfoUnit>&segInfo);
};
