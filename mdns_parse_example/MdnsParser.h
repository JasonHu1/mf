#ifndef __MDNS_PARSER_H__
#define __MDNS_PARSER_H__
#ifdef __cplusplus
extern "C"{
#endif//__cplusplus
//c-style header files
#ifdef __cplusplus
}
#endif//__cplusplus
#include <cstdint>
#include <limits>

#include "lib/support/logging/CHIPLogging.h"
#include <inet/IPAddress.h>
#include <inet/InetInterface.h>
#include <inet/InetLayer.h>
#include <lib/core/CHIPError.h>
#include <lib/core/Optional.h>
#include <lib/core/PeerId.h>
#include <lib/support/BytesToHex.h>
#include <lib/mdns/Resolver.h>
#include <lib/mdns/MinimalMdnsServer.h>
#include <lib/mdns/minimal/Parser.h>
#include <lib/mdns/minimal/core/BytesRange.h>
#include <lib/mdns/minimal/RecordData.h>

using namespace mdns::Minimal;
using namespace chip::Mdns;

class MdnsMessage : public mdns::Minimal::ParserDelegate      // parses queries
{
    public:
        MdnsMessage(ResolverDelegate * delegate, chip::Inet::InterfaceId interfaceId,
                       const BytesRange & packet) :
        mDelegate(delegate), mPacketRange(packet)
        {
            mInterfaceId           = interfaceId;
            mNodeData.mInterfaceId = interfaceId;
        }
        ~MdnsMessage(){};
        // ParserDelegate
        void OnHeader(ConstHeaderRef & header) override ;
        void OnResource(ResourceType type, const ResourceData & data) override ;
        void OnQuery(const QueryData & data) override;
        void OnComplete(void);
    private:
        ResolverDelegate * mDelegate = nullptr;
        DiscoveredNodeData mDiscoveredNodeData;
        ResolvedNodeData mNodeData;
        chip::Inet::InterfaceId mInterfaceId;
        BytesRange mPacketRange;

        bool mValid       = false;
        bool mHasNodePort = false;
        bool mHasIP       = false;
        bool mOperationalSrvFlag = false;
        void OnCommissionableNodeSrvRecord(SerializedQNameIterator name, const SrvRecord & srv);
        void OnOperationalSrvRecord(SerializedQNameIterator name, const SrvRecord & srv);

        void OnDiscoveredNodeIPAddress(const chip::Inet::IPAddress & addr);
        void OnOperationalIPAddress(const chip::Inet::IPAddress & addr);
};
class MdnsParser : public chip::Mdns::MdnsPacketDelegate // receive query packets
{
public:
    MdnsParser()
    {
        GlobalMinimalMdnsServer::Instance().SetQueryDelegate(this);
        GlobalMinimalMdnsServer::Instance().SetResponseDelegate(this);
    }
    ~MdnsParser() {}
    static std::shared_ptr<MdnsParser> create();
    static std::shared_ptr<MdnsParser> Instance();
    CHIP_ERROR SetResolverDelegate(ResolverDelegate * delegate);

    // Service advertiser
    CHIP_ERROR Start(chip::Inet::InetLayer * inetLayer, uint16_t port);
    // MdnsPacketDelegate
    void OnMdnsPacketData(const BytesRange & data, const chip::Inet::IPPacketInfo * info) override;
private:
    ResolverDelegate * mDelegate = nullptr;
    BytesRange mPacketRange;
    /// Advertise available records configured within the server
    ///
    /// Usable as boot-time advertisement of available SRV records.
    void AdvertiseRecords();

    /// Determine if advertisement on the specified interface/address is ok given the
    /// interfaces on which the mDNS server is listening
    bool ShouldAdvertiseOn(const chip::Inet::InterfaceId id, const chip::Inet::IPAddress & addr);

};
#endif//__MDNS_PARSER_H__