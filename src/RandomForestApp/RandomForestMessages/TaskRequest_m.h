//
// Generated file, do not edit! Created by nedtool 5.3 from src/RandomForestApp/RandomForestMessages/TaskRequest.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __TASKREQUEST_M_H
#define __TASKREQUEST_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0503
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



// cplusplus {{
#include "veins/modules/messages/WaveShortMessage_m.h"
// }}

/**
 * Class generated from <tt>src/RandomForestApp/RandomForestMessages/TaskRequest.msg:7</tt> by nedtool.
 * <pre>
 * packet TaskRequest extends WaveShortMessage
 * {
 *     int virtualServerId;
 *     int loadBalancerId;
 *     int taskCounter;
 *     double computationWork;
 * }
 * </pre>
 */
class TaskRequest : public ::WaveShortMessage
{
  protected:
    int virtualServerId;
    int loadBalancerId;
    int taskCounter;
    double computationWork;

  private:
    void copy(const TaskRequest& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const TaskRequest&);

  public:
    TaskRequest(const char *name=nullptr, short kind=0);
    TaskRequest(const TaskRequest& other);
    virtual ~TaskRequest();
    TaskRequest& operator=(const TaskRequest& other);
    virtual TaskRequest *dup() const override {return new TaskRequest(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getVirtualServerId() const;
    virtual void setVirtualServerId(int virtualServerId);
    virtual int getLoadBalancerId() const;
    virtual void setLoadBalancerId(int loadBalancerId);
    virtual int getTaskCounter() const;
    virtual void setTaskCounter(int taskCounter);
    virtual double getComputationWork() const;
    virtual void setComputationWork(double computationWork);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const TaskRequest& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, TaskRequest& obj) {obj.parsimUnpack(b);}


#endif // ifndef __TASKREQUEST_M_H

