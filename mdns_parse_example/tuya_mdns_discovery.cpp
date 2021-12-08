#include "tuya_mdns_discovery.h"
#include <iostream>
#include <memory>
#include <string>
// #include <inet/IPAddress.h>
// #include <inet/UDPEndPoint.h>
// #include <lib/core/CHIPError.h>
// #include <system/SystemPacketBuffer.h>
// #include <transport/raw/MessageHeader.h>
#include <transport/raw/PeerAddress.h>
#include <lib/mdns/Resolver.h>
#include "MdnsParser.h"

chip::Inet::InetLayer InetLayer;
int MDNSResolve::init(){
        MdnsParser::create();
        MdnsParser::Instance()->Start(&InetLayer, kMdnsPort);
        MdnsParser::Instance()->SetResolverDelegate(nullptr);
        MdnsParser::Instance()->SetResolverDelegate(this);
        return 0;
} 

void MDNSResolve::OnNodeIdResolved(const chip::Mdns::ResolvedNodeData & nodeData) 
{
    vdbg_trace();
    char addrBuffer[chip::Transport::PeerAddress::kMaxToStringSize];
    nodeData.mAddress.ToString(addrBuffer);
    ChipLogProgress(chipTool, "NodeId Resolution: %" PRIu64 " Address: %s, Port: %" PRIu16, nodeData.mPeerId.GetNodeId(),
                    addrBuffer, nodeData.mPort);
    ChipLogProgress(chipTool, "    Hostname: %s", nodeData.mHostName);

    auto retryInterval = nodeData.GetMrpRetryIntervalIdle();

    if (retryInterval.HasValue())
        ChipLogProgress(chipTool, "   MRP retry interval (idle): %" PRIu32 "ms", retryInterval.Value());

    retryInterval = nodeData.GetMrpRetryIntervalActive();

    if (retryInterval.HasValue())
        ChipLogProgress(chipTool, "   MRP retry interval (active): %" PRIu32 "ms", retryInterval.Value());

    ChipLogProgress(chipTool, "   Supports TCP: %s", nodeData.mSupportsTcp ? "yes" : "no");
}

void MDNSResolve::OnNodeDiscoveryComplete(const chip::Mdns::DiscoveredNodeData & nodeData) {
    std::cout<<" jason "<<__FUNCTION__<<" "<<__LINE__<<std::endl;
}