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
                Cluster(chip::ClusterId inClusterId,Attribute*inAttributeData){
                    mClusterId = inClusterId;
                    mAttributes.push_back(inAttributeData);
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
                Attribute* FindAttributeData(chip::AttributeId inAttributeId){
                    for(int i=0;i<mAttributes.size();i++){
                        return &mAttributes.at(i);
                    }
                    return nullptr;
                }
        };//class Cluster
        class EndpointType{
            public:
                EndpointType(int ep):mEndpointNumber(ep){}
                ~EndpointType(){}
                std::vector<int> mEndpointNumber;
                std::vector<Cluster> mClusters;
                std::vector<chip::deviceType> mDevicetypes;
                /*ep cluster attribute这些属于产品固有数据，产品出厂就不会变化，所以他们的条目只有更新与查找，并没有删除。
                */
                int UpdateClusterData(chip::ClusterId inClusterId,Attribute*inAttributeData){
                    int ret=0;
                    for(auto it=mClusters.begin();it!=mClusters.end();it++){
                        if(inClusterId==it->mClusterId){
                            ret=it->UpdateAttributeData(inAttributeData);
                            return ret;
                        }
                    }
                    mClusters.push_back(Cluster(inClusterId,inAttributeData));//new Cluster(inClusterId,inAttributeData)
                    return ret;
                }
                Cluster* FindClusterData(chip::ClusterId inClusterId){
                    for(int i=0;i<mClusters.size();i++){
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
                int UpdateEndpointData(){

                }
                EndpointType*FindEndpointTypeData(int ep){

                }
            public:
                std::vector<EndpointType> mEndpoint;
                std::vector<chip::deviceType> mRootDevicetypes;//这是ep0上的devicetype，每个节点的devicetype与普通cluster放一块存在endpointType中
                uint64_t mTimeStamp;//节点最新的上报时间戳
                chip::PeerId mPeerId;
                chip::Fabricid mFabricId;
                std::string mEui64;
                boot mOnoffLine;//0:remove,1:offline,2:online 日常运行中，设备列表的变化不会很大，flash删除，ram不删除，下次加入能快点。
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