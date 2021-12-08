#ifndef __TUYA_MDNS_DISCOVERY_H__
#define __TUYA_MDNS_DISCOVERY_H__
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


class MDNSResolve : public chip::Mdns::ResolverDelegate
{
public:
    MDNSResolve(){};
    ~MDNSResolve(){};
    int init(void);
    void OnNodeIdResolved(const chip::Mdns::ResolvedNodeData & nodeData);
    void OnNodeIdResolutionFailed(const chip::PeerId & peerId, CHIP_ERROR error){};
    void OnNodeDiscoveryComplete(const chip::Mdns::DiscoveredNodeData & nodeData);

    static constexpr uint16_t kMdnsPort = 5353;
};

#endif//__TUYA_MDNS_DISCOVERY_H__