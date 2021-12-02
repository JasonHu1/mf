#ifndef __OTBR_OBJECT_H__
#define __OTBR_OBJECT_H__
#ifdef __cplusplus
extern "C"{
#endif//__cplusplus
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dbus/dbus.h>
#ifdef __cplusplus
}
#endif//__cplusplus
#include <string>

namespace tuya{
    namespace otbr{
class DbusInitiator{
#define OTBR_DBUS_ADD_ON_MESH_PREFIX_METHOD "AddOnMeshPrefix"
#define OTBR_DBUS_REMOVE_ON_MESH_PREFIX_METHOD "RemoveOnMeshPrefix"
#define OTBR_DBUS_PERMIT_UNSECURE_JOIN_METHOD "PermitUnsecureJoin"
#define OTBR_DBUS_JOINER_START_METHOD "JoinerStart"
#define OTBR_DBUS_JOINER_STOP_METHOD "JoinerStop"
#define OTBR_DBUS_ADD_EXTERNAL_ROUTE_METHOD "AddExternalRoute"
#define OTBR_DBUS_REMOVE_EXTERNAL_ROUTE_METHOD "RemoveExternalRoute"
#define OTBR_DBUS_ATTACH_ALL_NODES_TO_METHOD "AttachAllNodesTo"

/***************thread network property**************************/
#define OTBR_DBUS_PROPERTY_MESH_LOCAL_PREFIX "MeshLocalPrefix"
#define OTBR_DBUS_PROPERTY_LEGACY_ULA_PREFIX "LegacyULAPrefix"
#define OTBR_DBUS_PROPERTY_LINK_MODE "LinkMode"
#define OTBR_DBUS_PROPERTY_DEVICE_ROLE "DeviceRole"
#define OTBR_DBUS_PROPERTY_NETWORK_NAME "NetworkName"
#define OTBR_DBUS_PROPERTY_PANID "PanId"
#define OTBR_DBUS_PROPERTY_EXTPANID "ExtPanId"
#define OTBR_DBUS_PROPERTY_CHANNEL "Channel"
#define OTBR_DBUS_PROPERTY_NETWORK_KEY "NetworkKey"
#define OTBR_DBUS_PROPERTY_CCA_FAILURE_RATE "CcaFailureRate"
#define OTBR_DBUS_PROPERTY_LINK_COUNTERS "LinkCounters"
#define OTBR_DBUS_PROPERTY_IP6_COUNTERS "Ip6Counters"
#define OTBR_DBUS_PROPERTY_SUPPORTED_CHANNEL_MASK "LinkSupportedChannelMask"
#define OTBR_DBUS_PROPERTY_RLOC16 "Rloc16"
#define OTBR_DBUS_PROPERTY_EXTENDED_ADDRESS "ExtendedAddress"
#define OTBR_DBUS_PROPERTY_ROUTER_ID "RouterID"
#define OTBR_DBUS_PROPERTY_LEADER_DATA "LeaderData"
#define OTBR_DBUS_PROPERTY_NETWORK_DATA_PRPOERTY "NetworkData"
#define OTBR_DBUS_PROPERTY_STABLE_NETWORK_DATA_PRPOERTY "StableNetworkData"
#define OTBR_DBUS_PROPERTY_LOCAL_LEADER_WEIGHT "LocalLeaderWeight"
#define OTBR_DBUS_PROPERTY_CHANNEL_MONITOR_SAMPLE_COUNT "ChannelMonitorSampleCount"
#define OTBR_DBUS_PROPERTY_CHANNEL_MONITOR_ALL_CHANNEL_QUALITIES "ChannelMonitorAllChannelQualities"
#define OTBR_DBUS_PROPERTY_CHILD_TABLE "ChildTable"
#define OTBR_DBUS_PROPERTY_NEIGHBOR_TABLE_PROEPRTY "NeighborTable"
#define OTBR_DBUS_PROPERTY_PARTITION_ID_PROEPRTY "PartitionID"
#define OTBR_DBUS_PROPERTY_INSTANT_RSSI "InstantRssi"
#define OTBR_DBUS_PROPERTY_RADIO_TX_POWER "RadioTxPower"
#define OTBR_DBUS_PROPERTY_EXTERNAL_ROUTES "ExternalRoutes"
#define OTBR_DBUS_PROPERTY_ACTIVE_DATASET_TLVS "ActiveDatasetTlvs"
#define OTBR_DBUS_PROPERTY_RADIO_REGION "RadioRegion"

    public:
        DbusInitiator();//直接在构造函数中完成与daemon的链接与注册
        ~DbusInitiator(); 
        int init();
        DBusMessage *method_call(std::string Properity);
        DBusMessageIter* dbus_iter_enter_recurse(DBusMessageIter *iter,int *iterType);
        int query(std::string Properity,uint16_t*aValue);
    private:
        DBusConnection *mConnection;
        std::string const mServiceName ="io.openthread.BorderRouter.wpan0";
        std::string const mObjectPath = "/io/openthread/BorderRouter/wpan0";
        std::string const mInterfaceName ="io.openthread.BorderRouter";
        std::string const mDbusStandardInterfaceProperties="org.freedesktop.DBus.Properties";
};
    }//namespace otbr
}//namespace tuya

#endif//__OTBR_OBJECT_H__