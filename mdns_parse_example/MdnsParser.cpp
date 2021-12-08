#include "MdnsParser.h"
#include <lib/mdns/minimal/RecordData.h>
#include <lib/mdns/ServiceNaming.h>
#include <lib/support/CHIPMemString.h>
#include <lib/mdns/TxtFields.h>

using namespace chip;
using namespace chip::Mdns;
using namespace mdns::Minimal;

std::shared_ptr<MdnsParser> gpMdnsParser;

namespace MdnsPrint {

namespace {

__attribute__((__unused__))const char * ToString(mdns::Minimal::QType qtype)
{
    static char buff[32];

    switch (qtype)
    {
    case mdns::Minimal::QType::A:
        return "A";
    case mdns::Minimal::QType::NS:
        return "NS";
    case mdns::Minimal::QType::CNAME:
        return "CNAME";
    case mdns::Minimal::QType::SOA:
        return "SOA";
    case mdns::Minimal::QType::WKS:
        return "WKS";
    case mdns::Minimal::QType::PTR:
        return "PTR";
    case mdns::Minimal::QType::MX:
        return "MX";
    case mdns::Minimal::QType::SRV:
        return "SRV";
    case mdns::Minimal::QType::AAAA:
        return "AAAA";
    case mdns::Minimal::QType::ANY:
        return "ANY";
    case mdns::Minimal::QType::TXT:
        return "TXT";
    default:
        sprintf(buff, "UNKNOWN (%d)!!", static_cast<int>(qtype));
        return buff;
    }
}

__attribute__((__unused__))const char * ToString(mdns::Minimal::QClass qclass)
{
    static char buff[32];

    switch (qclass)
    {
    case mdns::Minimal::QClass::IN_UNICAST:
        return "IN(UNICAST)";
    case mdns::Minimal::QClass::IN:
        return "IN";
    default:
        sprintf(buff, "UNKNOWN (%d)!!", static_cast<int>(qclass));
        return buff;
    }
}

__attribute__((__unused__))const char * ToString(mdns::Minimal::ResourceType type)
{
    static char buff[32];

    switch (type)
    {
    case mdns::Minimal::ResourceType::kAnswer:
        return "ANSWER";
    case mdns::Minimal::ResourceType::kAdditional:
        return "ADDITIONAL";
    case mdns::Minimal::ResourceType::kAuthority:
        return "AUTHORITY";
    default:
        sprintf(buff, "UNKNOWN (%d)!!", static_cast<int>(type));
        return buff;
    }
}

__attribute__((__unused__))bool HasQNamePart(SerializedQNameIterator qname, QNamePart part)
{
    while (qname.Next())
    {
        if (strcmp(qname.Value(), part) == 0)
        {
            return true;
        }
    }
    return false;
}
__attribute__((__unused__))void PrintQName(mdns::Minimal::SerializedQNameIterator it)
{
    while (it.Next())
    {
        printf("%s.", it.Value());
    }
    if (!it.IsValid())
    {
        printf("   (INVALID!)");
    }
    printf("\n");
}

} // namespace

} // namespace MdnsExample

const ByteSpan GetSpan(const mdns::Minimal::BytesRange & range)
{
    return ByteSpan(range.Start(), range.Size());
}

template <class NodeData>
class TxtRecordDelegateImpl : public mdns::Minimal::TxtRecordDelegate
{
public:
    explicit TxtRecordDelegateImpl(NodeData & nodeData) : mNodeData(nodeData) {}
    void OnRecord(const mdns::Minimal::BytesRange & name, const mdns::Minimal::BytesRange & value) override
    {
        std::string sname(reinterpret_cast<const char *>(name.Start()), name.Size());
        std::string svalue(reinterpret_cast<const char *>(value.Start()), value.Size());

        printf("TXT:  '%s' = '%s'\n", sname.c_str(), svalue.c_str());
        FillNodeDataFromTxt(GetSpan(name), GetSpan(value), mNodeData);
    }

private:
    NodeData & mNodeData;
};

void MdnsMessage::OnComplete(void)
{
    // ChipLogProgress(Discovery, "mHasIP=%d,mHasNodePort=%d",mHasIP,mHasNodePort);
    if (mDiscoveredNodeData.IsValid())
    {
        mDiscoveredNodeData.LogDetail();
        mDelegate->OnNodeDiscoveryComplete(mDiscoveredNodeData);
    }
    else if (mHasIP && mHasNodePort)
    {
        mNodeData.LogNodeIdResolved();
        mDelegate->OnNodeIdResolved(mNodeData);
    }else{
    }
}

void MdnsMessage::OnOperationalSrvRecord(SerializedQNameIterator name, const SrvRecord & srv)
{
    mdns::Minimal::SerializedQNameIterator it = srv.GetName();
    if (it.Next())
    {
        Platform::CopyString(mNodeData.mHostName, it.Value());
    }
    printf("mHostName=%s\r\n",mNodeData.mHostName);
    if (!name.Next())
    {
#ifdef MINMDNS_RESOLVER_OVERLY_VERBOSE
        ChipLogError(Discovery, "mDNS packet is missing a valid server name");
#endif
        mHasNodePort = false;
        return;
    }

    if (ExtractIdFromInstanceName(name.Value(), &mNodeData.mPeerId) != CHIP_NO_ERROR)
    {
        ChipLogError(Discovery, "Failed to parse peer id from %s", name.Value());
        mHasNodePort = false;
        return;
    }

    mNodeData.mPort = srv.GetPort();
    printf("mNodeData.mPort=%d\r\n",mNodeData.mPort);
    mHasNodePort    = true;
}

void MdnsMessage::OnCommissionableNodeSrvRecord(SerializedQNameIterator name, const SrvRecord & srv)
{
    // Host name is the first part of the qname
    mdns::Minimal::SerializedQNameIterator it = srv.GetName();
    if (it.Next())
    {
        Platform::CopyString(mDiscoveredNodeData.hostName, it.Value());
    }
    if (name.Next())
    {
        strncpy(mDiscoveredNodeData.instanceName, name.Value(), sizeof(DiscoveredNodeData::instanceName));
    }
    mDiscoveredNodeData.port = srv.GetPort();
    printf("hostName=%s,instanceName=%s\r\n",mDiscoveredNodeData.hostName,mDiscoveredNodeData.instanceName);
}

void MdnsMessage::OnOperationalIPAddress(const chip::Inet::IPAddress & addr)
{
    // TODO: should validate that the IP address we receive belongs to the
    // server associated with the SRV record.
    //
    // This code assumes that all entries in the mDNS packet relate to the
    // same entity. This may not be correct if multiple servers are reported
    // (if multi-admin decides to use unique ports for every ecosystem).
    mNodeData.mAddress = addr;
    mHasIP             = true;
}

void MdnsMessage::OnDiscoveredNodeIPAddress(const chip::Inet::IPAddress & addr)
{
    if (mDiscoveredNodeData.numIPs >= DiscoveredNodeData::kMaxIPAddresses)
    {
        return;
    }
    mDiscoveredNodeData.ipAddress[mDiscoveredNodeData.numIPs]   = addr;
    mDiscoveredNodeData.interfaceId[mDiscoveredNodeData.numIPs] = mInterfaceId;
    mDiscoveredNodeData.numIPs++;
}
void MdnsMessage::OnHeader(ConstHeaderRef & header) {
    return;
    ChipLogProgress(Discovery,"OnHeader : MessageId=%d,QueryCount=%d,AnswerCount=%d,AuthorityCount=%d,AdditionalCount=%d",
                header.GetMessageId(),header.GetQueryCount(),
                header.GetAnswerCount(),header.GetAuthorityCount(),header.GetAdditionalCount());
    return ;
}
void MdnsMessage::OnQuery(const QueryData & data)
{
    return;
    printf("OnQuery %s/%s%s: \r\n",  MdnsPrint::ToString(data.GetType()), MdnsPrint::ToString(data.GetClass()),
           data.RequestedUnicastAnswer() ? " UNICAST" : "");
    MdnsPrint::PrintQName(data.GetName());
}
void MdnsMessage::OnResource(ResourceType type, const ResourceData & data){
    /// Data content is expected to contain:
    /// - A SRV entry that includes the node ID in expected format (fabric + nodeid)
    ///    - Can extract: fabricid, nodeid, port
    ///    - References ServerName
    /// - Additional records tied to ServerName contain A/AAAA records for IP address data
    ChipLogProgress(Discovery, "MdnsParser::OnResource() ResourceType= %s",MdnsPrint::ToString(type));
    ChipLogProgress(Discovery, "MdnsParser::OnResource() RecordType= %s",MdnsPrint::ToString(data.GetType()));
    printf("MdnsParser::OnResource() Name= ");MdnsPrint::PrintQName(data.GetName());
    
    switch (data.GetType())
    {
    case QType::SRV: {
        SrvRecord srv;
        if (!srv.Parse(data.GetData(), mPacketRange))
        {
            ChipLogError(Discovery, "Packet data reporter failed to parse SRV record");
        }
        printf("MdnsParser::OnResource() srv.GetName()= ");MdnsPrint::PrintQName(srv.GetName());
        if (MdnsPrint::HasQNamePart(data.GetName(), kOperationalServiceName))
        {
            OnOperationalSrvRecord(data.GetName(), srv);
        }
        else if (MdnsPrint::HasQNamePart(data.GetName(), kCommissionableServiceName) || MdnsPrint::HasQNamePart(data.GetName(), kCommissionerServiceName))
        {
            OnCommissionableNodeSrvRecord(data.GetName(), srv);
        }
        break;
    }
    case QType::PTR: {
        {
            SerializedQNameIterator qname;
            ParsePtrRecord(data.GetData(), mPacketRange, &qname);
            if (qname.Next())
            {
                strncpy(mDiscoveredNodeData.instanceName, qname.Value(), sizeof(DiscoveredNodeData::instanceName));
            }
        }
        break;
    }
    case QType::TXT:
        if (MdnsPrint::HasQNamePart(data.GetName(), kCommissionableServiceName) || MdnsPrint::HasQNamePart(data.GetName(), kCommissionerServiceName))
        {
            TxtRecordDelegateImpl<DiscoveredNodeData> textRecordDelegate(mDiscoveredNodeData);
            ParseTxtRecord(data.GetData(), &textRecordDelegate);
        }
        else if (MdnsPrint::HasQNamePart(data.GetName(), kOperationalServiceName))
        {
            TxtRecordDelegateImpl<ResolvedNodeData> textRecordDelegate(mNodeData);
            ParseTxtRecord(data.GetData(), &textRecordDelegate);
        }
        break;
    case QType::A: {
        Inet::IPAddress addr;
        if (!ParseARecord(data.GetData(), &addr))
        {
            ChipLogError(Discovery, "Packet data reporter failed to parse A record");
            mHasIP = false;
        }
        else
        {
            if (MdnsPrint::HasQNamePart(data.GetName(), kOperationalServiceName))
            {
                OnOperationalIPAddress(addr);
            }
            else if (MdnsPrint::HasQNamePart(data.GetName(), kCommissionableServiceName) || MdnsPrint::HasQNamePart(data.GetName(), kCommissionerServiceName))
            {
                OnDiscoveredNodeIPAddress(addr);
            }
        }
        break;
    }
    case QType::AAAA: {
        Inet::IPAddress addr;
        if (!ParseAAAARecord(data.GetData(), &addr))
        {
            ChipLogError(Discovery, "Packet data reporter failed to parse AAAA record");
            mHasIP = false;
        }
        else
        {
            if (MdnsPrint::HasQNamePart(data.GetName(), kOperationalServiceName))
            {
                OnOperationalIPAddress(addr);
            }
            else if (MdnsPrint::HasQNamePart(data.GetName(), kCommissionableServiceName) || MdnsPrint::HasQNamePart(data.GetName(), kCommissionerServiceName))
            {
                OnDiscoveredNodeIPAddress(addr);
            }
        }
        break;
    }
    default:
        break;
    }
}

std::shared_ptr<MdnsParser> MdnsParser::create()
{
    if (nullptr == gpMdnsParser)
    {
        std::shared_ptr<MdnsParser> m(new MdnsParser());
        gpMdnsParser = m;
    }
    return gpMdnsParser;
}
std::shared_ptr<MdnsParser> MdnsParser::Instance()
{
    return gpMdnsParser;
}

void MdnsParser::OnMdnsPacketData(const BytesRange & data, const chip::Inet::IPPacketInfo * info)
{
    MdnsMessage Messageer(mDelegate, info->Interface , data);
    if (!ParsePacket(data, &Messageer))
    {
        ChipLogError(Discovery, "Failed to parse mDNS query");
    }else {
        Messageer.OnComplete();
    }
}

CHIP_ERROR MdnsParser::Start(chip::Inet::InetLayer * inetLayer, uint16_t port)
{
    GlobalMinimalMdnsServer::Server().Shutdown();

    // Re-set the server in the response sender in case this has been swapped in the
    // GlobalMinimalMdnsServer (used for testing).
    ReturnErrorOnFailure(GlobalMinimalMdnsServer::Instance().StartServer(inetLayer, port));

    ChipLogProgress(Discovery, "CHIP minimal mDNS started advertising.");

    // AdvertiseRecords();

    return CHIP_NO_ERROR;
}
CHIP_ERROR MdnsParser::SetResolverDelegate(ResolverDelegate * delegate)
{
    mDelegate = delegate;
    return CHIP_NO_ERROR;
}