#ifndef __DEVICE_DEFINE_H__
#define __DEVICE_DEFINE_H__
#ifdef __cplusplus
extern "C"{
#endif//__cplusplus
//c-style
#include "tuya_cloud_com_defs.h"
#ifdef __cplusplus
}
#endif//__cplusplus
#include <list>
#include <memory>
#include <map>
#include <mutex>
#include <string>
#include <vector>
#include <iostream>
#include <src/app/util/af-types.h>
#include "Attributes.h"
#include "attribute-id.h"
#include "attribute-type.h"
#include "cluster-id.h"
#include <lib/core/PeerId.h>
/*
typedef enum{
    JOIN_STAGE_ANNOUNCE              = 0x0100,
    JOIN_STAGE_BASIC_INFO_READ       = 0x0200,
    JOIN_STAGE_BASIC_INFO_SUBCRIBE   = 0x0400,
    JOIN_STAGE_UTILITY_READ          = 0x0800,
    JOIN_STAGE_APP_SUBCRIBE          = 0x1000,
    JOIN_STAGE_FINISH                = 0x2000,
}JOINSTAGE_E;


第一步，怕设备因故重启，收到dnssd首先就是一次性订阅ep0的basic info cluster中的StartUp/ShutDown/Leave/ReachableChanged 事件，并创建class device节点条目。
此时class device节点只有nodeID一个信息，这个时候gw还没有子设备的权限，需要等到手机设置好gw的权限厚，网关才可以继续下面的读和订阅操作

第二步，一次性读取ep0中basic info的所有attribute属性，得到的内容写入mRootEndpoint.

第三步，一次性读ep0的 descriptor partlist/devicetypeList/server clusterList/client clusterList，得到的内容写入mRootEndpoint.
	ep0会有一些Utility cluster，就是之前zdo哪些cluster， 比如Access Control Cluster，先读到，后面怎么用再说

第四步，根据ep0的partlist个数和内容，循环读取epx的 descriptor partlist/devicetypeList/server clusterList/client clusterList

上面四个步骤是基本操作，下面要根据每个ep的devicetype 来决定订阅哪些属性

第五步，根据devicetype去分别订阅应用epx上的属性上报，只订阅不读每个ep的attribute

第六步，持久化存储，并上报给tuya cloud

每次来信息都需要遍历，不太好，加网的状态机过程，要endpointtype元素作为上下文context传递，这样就不用每次遍历了。

需要一个分发器来swtich-case处理设备处在每个状态要执行的动作。
*/
namespace tuya {
    namespace app{
        #define JOIN_STAGE_ANNOUNCE_STATUS_STARUP       0x01
        #define JOIN_STAGE_ANNOUNCE_STATUS_SHUTDOWN     0x02
        #define JOIN_STAGE_ANNOUNCE_STATUS_LEAVE        0x04
        #define JOIN_STAGE_ANNOUNCE_STATUS_REACHABLE    0x08
        /*
        状态迁移顺序：
        1.怕设备因故重启，收到底层dnssd发现的服务提供节点置 JOIN_STAGE_ANNOUNCE ，首先订阅基本四个基本事件，开启配网状态迁移
        2.四个基本事件订阅成功置 JOIN_STAGE_BASIC_INFO_SUBCRIBE ，然后从ep0开始，先读工具性的类似zdo的clustef信息，比如读ep0 descriptor的四个属性来创建cluster条目表
        3.ep0 descriptor读取成功置 JOIN_STAGE_UTILITY_READ ，根据此读取ep0的其他工具管理性的属性来进一步了解和标识节点，比如basic information
        4.读到ep0 basic information成功置 JOIN_STAGE_BASIC_INFO_READ ， 开始根据产品功能特性去订阅需要接收的周期上报
        5.epx的功能属性订阅成功了置 JOIN_STAGE_APP_SUBCRIBE ，进行云端注册和持久化存储
        6.云端注册成功，完成后置 JOIN_STAGE_SUCCESS 。
        7.本地持久化存储，完成置 JOIN_STAGE_FINISH 。

        */
        typedef enum{
            JOIN_STAGE_ANNOUNCE              = 0x00,
            JOIN_STAGE_BASIC_INFO_SUBCRIBE   = 0x02,
            JOIN_STAGE_UTILITY_READ          = 0x03,
            JOIN_STAGE_BASIC_INFO_READ       = 0x01,
            JOIN_STAGE_APP_SUBCRIBE          = 0x04,
            JOIN_STAGE_SUCCESS               = 0x05,
            JOIN_STAGE_FINISH                = 0x06,
        }JOINSTAGE_E;
        class Attribute{
            public:
                Attribute(chip::AttributeId inAttrId,EmberAfAttributeType type):mAttributeId(inAttrId),mAttributeType(type){
                }
                Attribute(chip::AttributeId inAttrId,EmberAfAttributeType type,std::string inAttributeData):mAttributeId(inAttrId),mAttributeType(type),mAttributeVal(inAttributeData){
                }
                ~Attribute(){}
                chip::AttributeId mAttributeId;
                EmberAfAttributeType mAttributeType;
                std::string mAttributeVal;//不管啥类型，底层都转成字符串保存吧,可是像list这种复合类型没法处理
        };//class Attribute
        class Cluster{
            public:
                Cluster(chip::ClusterId inClusterId,EmberAfClusterMask mask):mClusterId(inClusterId),mMask(mask){
                }
                ~Cluster(){}
                chip::ClusterId mClusterId;
                std::vector<Attribute>mAttributes;//vector效率好,线程安全
                EmberAfClusterMask mMask;//client or server
                int AddOrUpdateAttributeData(chip::AttributeId inAttrId,EmberAfAttributeType datatype,std::string val){
                    int ret=0;
                    for(auto it=mAttributes.begin();it!=mAttributes.end();it++){
                        if(inAttrId==it->mAttributeId){
                            it->mAttributeVal = val;
                            return ret;
                        }
                    }
                    mAttributes.push_back(Attribute(inAttrId,datatype,val));//new Cluster(inClusterId,inAttributeData)
                    return ret;
                }
                int AddOrUpdateAttributeData(Attribute *inAttrData){                    
                    return AddOrUpdateAttributeData(inAttrData->mAttributeId,inAttrData->mAttributeType,inAttrData->mAttributeVal);
                }
                Attribute* FindAttributeData(chip::AttributeId inAttributeId){
                    for(unsigned int i=0;i<mAttributes.size();i++){
                        if(inAttributeId == mAttributes.at(i).mAttributeId )
                            return &mAttributes.at(i);
                    }
                    return nullptr;
                }
        };//class Cluster
        class EndpointType{
            public:
                EndpointType(chip::EventNumber ep):mEndpointNumber(ep){}
                ~EndpointType(){}
                chip::EventNumber mEndpointNumber;
                std::vector<Cluster> mClusters;
                std::vector<chip::DeviceTypeId> mDevicetypes;
                /*
                *保存descriptor client/server cluster 返回的列表
                */
                int CreateClusterElements(EmberAfClusterMask mask,std::vector<chip::ClusterId> clusterlist){
                    for(auto it=clusterlist.begin();it!=clusterlist.end();it++){
                        mClusters.push_back(Cluster(*(it),mask));
                    }
                    return 0;
                }
                /*ep cluster这些属于产品固有数据，产品出厂就不会变化，所以他们的条目只有更新与查找，并没有添加和删除。
                */
                int UpdateClusterData(chip::ClusterId inClusterId,Attribute*inAttributeData){
                    int ret=-1;
                    for(auto it=mClusters.begin();it!=mClusters.end();it++){
                        if(inClusterId==it->mClusterId){
                            ret=it->AddOrUpdateAttributeData(inAttributeData);
                            return ret;
                        }
                    }
                    PR_ERR("Clusterid=0x%04x is not in descriptor client/server list",inClusterId);
                    return ret;
                }
                Cluster* FindClusterData(chip::ClusterId inClusterId){
                    for(unsigned int i=0;i<mClusters.size();i++){
                        if(inClusterId == mClusters.at(i).mClusterId )
                            return &mClusters.at(i);
                    }
                    return nullptr;
                }
        };//class EndpointType
        class Node{
            public:
                //节点没有上报完整的时候，这过程就是临时堆对象，该读的读完，该订阅的订阅到了之后再添加deviceMap
                //根据dnssd过来的信息创建对象
                Node(std::string eui):mEui64(eui){

                }
                Node(chip::NodeId nodeId):mNodeId(nodeId){
                    
                }
                Node(chip::NodeId nodeId,chip::FabricId fabric):mNodeId(nodeId),mFabricId(fabric){
                    
                }
                ~Node(){}
                /*
                *保存ep0 descriptor cluster partlist的返回的列表
                */
                int AddEndpointTypeElements(std::vector<chip::EventNumber> eplist){
                    for(auto it=eplist.begin();it!=eplist.end();it++){
                        mEndpointTypes.push_back(EndpointType(*it));
                    }
                    return 0;
                }
                EndpointType*FindEndpointTypeData(chip::EventNumber ep){
                    for(unsigned int i=0;i<mEndpointTypes.size();i++){
                        if(ep == mEndpointTypes.at(i).mEndpointNumber )
                            return &mEndpointTypes.at(i);
                    }
                    PR_ERR("EndpointNumber=%d is not in descriptor partlist",ep);
                    return nullptr;
                }
                uint8_t GetJoinStage(void) const 
                {
                    return (uint8_t)((mJoinStage & 0xFF000000)>>24);
                }
                void SetJoinStage(uint8_t stage)
                {
                    mJoinStage = ((uint32_t)stage)<<24;
                }
                uint8_t GetJoinStatus(void) const 
                {
                    return (uint8_t)((mJoinStage & 0x0000FF00)>>8);
                }
                void SetJoinStatus(uint8_t status)
                {
                    mJoinStage = mJoinStage|(0x0000FF00&(((uint32_t)status)<<8));
                }
                uint8_t GetJoinReCount(void) const 
                {
                    return (uint8_t)(mJoinStage &0x000000FF);
                }
                void JoinReCountIncrease(void) 
                {
                    uint8_t cnt=(uint8_t)(mJoinStage &0x000000FF);
                    if(cnt<0xff){
                        mJoinStage++;
                    }
                }
            public:
                std::vector<EndpointType> mEndpointTypes;
                EndpointType mRootEndpoint= EndpointType(0);// 这是ep0上的Utility cluster，要经常用要单独列出来，就是代替之前zdo那些cluster的， 比如Access Control Cluster
                std::vector<uint64_t> mEndpointNumber;
                uint64_t mTimeStamp;//节点最新的上报时间戳
                chip::NodeId mNodeId;
                chip::FabricId mFabricId;
                std::string mEui64;
                bool mOnoffLine;////0:正在加网，1:remove,2:offline,3:online 日常运行中，设备列表的变化不会很大，flash删除，ram不删除，下次加入能快点。
                chip::Inet::IPAddress mAddress = chip::Inet::IPAddress::Any;
            private:
                uint32_t mJoinStage;
        };//class Node
        class Devices{
            public:
                Devices() {}
                ~Devices() {}
                void addDeviceNode(std::string mac,std::shared_ptr<Node> device) {
                    std::lock_guard<std::mutex> lock(mDevMapMutex);
                    mDeviceMap.insert(std::make_pair(mac,device));
                }
                /*
                固件升级若这些固有属性数据发生变化，考虑到属性有减少的情况，这时需要删掉整个节点了重新添加
                */
                void RemoveDeviceNode(std::string mac){
                    std::lock_guard<std::mutex> lock(mDevMapMutex);
                    mDeviceMap.erase(mac);
                }
                std::shared_ptr<Node> findDeviceByDidEx(std::string mac) {
                    auto it = mDeviceMap.find(mac);
                    if(it == mDeviceMap.end()) {
                        return nullptr;
                    }
                    return it->second;
                }
                Node * findDeviceByDid(std::string mac) {
                    std::shared_ptr<Node> device = findDeviceByDidEx(mac);
                    if(device) {
                        return  device.get();
                    }
                    else {
                        return nullptr;
                    }
                }
                Node * findDeviceByNodeId(chip::NodeId nodeid) {
                    for(auto it=mDeviceMap.begin();it!=mDeviceMap.end();it++){
                        std::shared_ptr<Node> node = it->second;
                        if(node->mNodeId==nodeid){
                            return  node.get();
                        }
                        return nullptr;
                    }
                   
                }
                std::map<std::string,std::shared_ptr<Node>> getDeviceMap() {
                    return mDeviceMap;
                };
            private:
                //<MAC,data> mac与nodeID的关系由下面传输管理来维护，应用层只用唯一且不变的mac来索引节点
                std::map<std::string,std::shared_ptr<Node>> mDeviceMap;
                std::mutex mDevMapMutex;//std::lock_guard<std::mutex> lock(mMutex);
        };//class Devices
    }//namespace app
}//namespace tuya
#endif//__DEVICE_DEFINE_H__