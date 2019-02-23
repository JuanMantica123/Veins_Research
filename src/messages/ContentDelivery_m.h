//
// Generated file, do not edit! Created by nedtool 5.1 from src/messages/ContentDelivery.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __CONTENTDELIVERY_M_H
#define __CONTENTDELIVERY_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0501
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



// cplusplus {{
#include "veins/modules/messages/WaveShortMessage_m.h"
// }}

/**
 * Class generated from <tt>src/messages/ContentDelivery.msg:7</tt> by nedtool.
 * <pre>
 * packet ContentDelivery extends WaveShortMessage
 * {
 *     string carId;
 *     int rsuId;
 * }
 * </pre>
 */
class ContentDelivery : public ::WaveShortMessage
{
  protected:
    ::omnetpp::opp_string carId;
    int rsuId;

  private:
    void copy(const ContentDelivery& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const ContentDelivery&);

  public:
    ContentDelivery(const char *name=nullptr, short kind=0);
    ContentDelivery(const ContentDelivery& other);
    virtual ~ContentDelivery();
    ContentDelivery& operator=(const ContentDelivery& other);
    virtual ContentDelivery *dup() const override {return new ContentDelivery(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual const char * getCarId() const;
    virtual void setCarId(const char * carId);
    virtual int getRsuId() const;
    virtual void setRsuId(int rsuId);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const ContentDelivery& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, ContentDelivery& obj) {obj.parsimUnpack(b);}


#endif // ifndef __CONTENTDELIVERY_M_H

