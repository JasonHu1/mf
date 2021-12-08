matter里面的接收广播的mDns消息并解析出来，这里代码不全，仅是应用层逻辑，需要依赖matter底层的udp。

思路是
MDNSResolve：应用class，拿到已经解析好的数据结构MdnsMessage。
MdnsParser：框架class,把自己注册委托给底层udp接受函数，收到mdns数据后转给MdnsMessage进行具体的解析。
MdnsMessage：数据的载体，负责解析数据和存储数据，并作为载体把数据给到应用class

