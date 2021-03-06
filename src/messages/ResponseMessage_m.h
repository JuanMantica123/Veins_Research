//
// Generated file, do not edit! Created by nedtool 5.3 from src/messages/ResponseMessage.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __RESPONSEMESSAGE_M_H
#define __RESPONSEMESSAGE_M_H

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
 * Class generated from <tt>src/messages/ResponseMessage.msg:7</tt> by nedtool.
 * <pre>
 * packet ResponseMessage extends WaveShortMessage
 * {
 *     int responseValue;
 * }
 * </pre>
 */
class ResponseMessage : public ::WaveShortMessage
{
  protected:
    int responseValue;

  private:
    void copy(const ResponseMessage& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const ResponseMessage&);

  public:
    ResponseMessage(const char *name=nullptr, short kind=0);
    ResponseMessage(const ResponseMessage& other);
    virtual ~ResponseMessage();
    ResponseMessage& operator=(const ResponseMessage& other);
    virtual ResponseMessage *dup() const override {return new ResponseMessage(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getResponseValue() const;
    virtual void setResponseValue(int responseValue);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const ResponseMessage& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, ResponseMessage& obj) {obj.parsimUnpack(b);}


#endif // ifndef __RESPONSEMESSAGE_M_H

