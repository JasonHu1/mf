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
step1、收到dnssd就创建class device节点条目，并添加devicemap列表中,此时class device节点只有nodeID一个信息

step2、读ep0 devicetype,写到class device结构中的mRootDevicetypes

step3、读ep0 cluster返回的中列表list，ep0会有一些Utility cluster，就是之前zdo哪些cluster， 比如Access Control Cluster，估计后续得单独处理

step4、读ep0 partlist返回设备的endpoint list，根据回复的partlist创建class EndpointType vector,并挂到class device的成员上

循环读ep0 partlist中的endpoint

step5、读epx devicetype,找到对应的endpointtype元素，写到元素中的成员devicetype

step6、读epx cluster返回，找到对应的endpointtype元素，根据返回的cluster列表来在eptype节点结构中的创建mcluster

step7、读epx partlist返回了，根据回复的partlist创建eptype节点,eptype中还没有普通cluster

step8、读ep的普通attibute就是update,attribute没有一个汇总的列表，只能是按照spec逐个的去读。


每次来信息都需要遍历，不太好，加网的状态机过程，要endpointtype元素作为上下文context传递，这样就不用每次遍历了。
*/
namespace tuya {
    namespace app{
        class Attribute{
            public:
                Attribute(chip::AttributeId inAttrId,EmberAfAttributeType type):mAttributeId(inAttrId),mAttributeType(type){
                }
                Attribute(chip::AttributeId inAttrId,EmberAfAttributeType type,std::string inAttributeData):mAttributeId(inAttrId),mAttributeType(type),mAttribteVal(inAttributeData){
                }
                ~Attribute(){}
                chip::AttributeId mAttributeId;
                EmberAfAttributeType mAttributeType;
                std::string mAttribteVal;//不管啥类型，底层都转成字符串保存吧
        };//class Attribute
        class Cluster{
            public:
                Cluster(chip::ClusterId inClusterId,EmberAfClusterMask mask):mClusterId(inClusterId),mMask(mask){
                }
                ~Cluster(){}
                chip::ClusterId mClusterId;
                std::vector<Attribute>mAttributes;//vector效率好,线程安全
                EmberAfClusterMask mMask;//client or server
                int UpdateAttributeData(chip::AttributeId inAttrId,EmberAfAttributeType datatype,std::string val){
                    int ret=0;
                    for(auto it=mAttributes.begin();it!=mAttributes.end();it++){
                        if(inAttrId==it->mAttributeId){
                            it->mAttribteVal = val;
                            return ret;
                        }
                    }
                    mAttributes.push_back(Attribute(inAttrId,datatype,val));//new Cluster(inClusterId,inAttributeData)
                    return ret;
                }
                int UpdateAttributeData(Attribute *inAttrData){                    
                    return UpdateAttributeData(inAttrData->mAttributeId,inAttrData->mAttributeType,inAttrData->mAttribteVal);
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
                int AddClusterElements(EmberAfClusterMask mask,std::vector<chip::ClusterId> clusterlist){
                    for(auto it=clusterlist.begin();it!=clusterlist.end();it++){
                        mClusters.push_back(Cluster(*(it),mask));
                    }
                    return 0;
                }
                /*ep cluster attribute这些属于产品固有数据，产品出厂就不会变化，所以他们的条目只有更新与查找，并没有删除。
                */
                int UpdateClusterData(chip::ClusterId inClusterId,Attribute*inAttributeData){
                    int ret=-1;
                    for(auto it=mClusters.begin();it!=mClusters.end();it++){
                        if(inClusterId==it->mClusterId){
                            ret=it->UpdateAttributeData(inAttributeData);
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
        class Device{
            public:
                //节点没有上报完整的时候，这过程就是临时堆对象，该读的读完，该订阅的订阅到了之后再添加deviceMap
                //根据dnssd过来的信息创建对象
                Device(chip::PeerId nodeId):mPeerId(nodeId){
                    
                }
                ~Device(){}
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
            public:
                std::vector<EndpointType> mEndpointTypes;
                EndpointType mRootEndpoint;// 这是ep0上的Utility cluster，要经常用要单独列出来，就是代替之前zdo那些cluster的， 比如Access Control Cluster
                uint64_t mTimeStamp;//节点最新的上报时间戳
                chip::PeerId mPeerId;
                chip::FabricId mFabricId;
                std::string mEui64;
                bool mOnoffLine;////0:正在加网，1:remove,2:offline,3:online 日常运行中，设备列表的变化不会很大，flash删除，ram不删除，下次加入能快点。
                chip::Inet::IPAddress mAddress = chip::Inet::IPAddress::Any;
        };//class Device
        class Devices{
            public:
                Devices() {}
                ~Devices() {}
                void addDeviceNode(std::string mac,std::shared_ptr<Device> device) {
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
                std::shared_ptr<Device> findDeviceByDidEx(std::string mac) {
                    auto it = mDeviceMap.find(mac);
                    if(it == mDeviceMap.end()) {
                        return nullptr;
                    }
                    return it->second;
                }
                Device * findDeviceByDid(std::string mac) {
                    std::shared_ptr<Device> device = findDeviceByDidEx(mac);
                    if(device) {
                        return  device.get();
                    }
                    else {
                        return nullptr;
                    }
                }
                std::map<std::string,std::shared_ptr<Device>> getDeviceMap() {
                    return mDeviceMap;
                };
            private:
                std::map<std::string,std::shared_ptr<Device>> mDeviceMap;//<MAC,data> mac与nodeID的关系由下面传输管理来维护，应用层只用唯一且不变的mac来索引节点
                std::mutex mDevMapMutex;//std::lock_guard<std::mutex> lock(mMutex);
        };//class Devices
    }//namespace app
}//namespace tuya
#endif//__DEVICE_DEFINE_H__