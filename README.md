Wireless Sensor Networks(WSN) Monitor System Based On Android
=============================================================

基于Android平台的WSN监控系统包括三部分:
------------------------------------------------------
- [**ZigBee**协议](http://www.zigbee.org/)实现的WSN，由ZigBee协议的传感节点组成，完成数据采集、处理、上传，执行控制命令。
- **JavaWeb**实现的服务端，由RS232通信，数据解析，持久化层，面向移动终端的Web通信等模块组成。web部分包括了HTTP及XMPP协议的推送服务。
- **Android**平台上的WSN监控软件。

WSN部分:
--

### 硬件基础
- 兼容[**TI CC2530DK**](http://www.ti.com.cn/tool/cn/cc2530dk#technicaldocuments)的所有CC2530评估板上
- 运行[**ZigBee协议**](http://www.zigbee.org/)协议栈[**Z-Stack协议栈**](http://www.ti.com.cn/tool/cn/z-stack)的CC2530芯片节点

### 软件实现
- 构建于基于[**ZigBee协议**](http://www.zigbee.org/)实现的[**Z-Stack协议栈**](http://www.ti.com.cn/tool/cn/z-stack)上，在AP层上实现所有功能逻辑。


### 部署方法
- **熟悉IAR以及Z-Stack协议栈忽略该部分**
- 在[IAR Embedd Workbench ID](http://www.iar.com/en/Products/IAR-Embedded-Workbench/8051/)编译调试<br / >
- 用IAR 打开..\WSNMonitorApp\CC2530DB 目录中的WSNMonitorApp.eww项目文件，右键项目名option中找到**C/C++Compile**中
的preprocessor选项，将addtional include dir的选项框中根据项目文件在文件系统中的目录位置，导入Z-Stack的其他层的源文件。
如果直接在pull到源码文件中打开WSNMonitorApp.eww并没有改动过默认的目录结构，就在选项框中直接贴入下面的文件结构即可：<br / >
$PROJ_DIR$<br / >
$PROJ_DIR$\\..\SOURCE
$PROJ_DIR$\\..\ZMAIN\TI2530DB
$PROJ_DIR$\\..\COMPONENTS\MT
$PROJ_DIR$\\..\COMPONENTS\HAL\INCLUDE
$PROJ_DIR$\\..\COMPONENTS\HAL\TARGET\CC2530EB
$PROJ_DIR$\\..\COMPONENTS\OSAL\MCU\CCSOC
$PROJ_DIR$\\..\COMPONENTS\OSAL\INCLUDE
$PROJ_DIR$\\..\COMPONENTS\STACK\AF
$PROJ_DIR$\\..\COMPONENTS\STACK\NWK
$PROJ_DIR$\\..\COMPONENTS\STACK\SEC
$PROJ_DIR$\\..\COMPONENTS\STACK\SAPI
$PROJ_DIR$\\..\COMPONENTS\STACK\SYS
$PROJ_DIR$\\..\COMPONENTS\STACK\ZDO
$PROJ_DIR$\\..\COMPONENTS\ZMAC\F8W
$PROJ_DIR$\\..\COMPONENTS\ZMAC
$PROJ_DIR$\\..\COMPONENTS\SERVICES\SADDR
$PROJ_DIR$\\..\COMPONENTS\SERVICES\SDATA
$PROJ_DIR$\\..\COMPONENTS\MAC\INCLUDE
$PROJ_DIR$\\..\COMPONENTS\MAC\HIGH_LEVEL
$PROJ_DIR$\\..\COMPONENTS\MAC\LOW_LEVEL\srf04
$PROJ_DIR$\\..\COMPONENTS\MAC\LOW_LEVEL\srf04\SINGLE_CHIP
- 编译宏<br / >
ZTOOL_P1<br / >
MT_TASK<br / >
MT_SYS_FUNC<br / >
MT_ZDO_FUNC<br / >
LCD_SUPPORTED=DEBUG
- 之后在IAR workspace下方选择对应节点类型的项目文件，完成编译后，
通过debug按钮使用[**CC Debugger**](http://www.ti.com/tool/cc-debugger)就能将程序烧写进对应节点硬件平台。

### 主要功能:
- ZigBee协议所规定三种不同类型节点的基础功能 **(默认你已了解ZigBee协议和Z-Stack协议栈)**
- EndDevice节点实现了DS18B20传感器的环境温度获取，处理，报告及报告周期设置，报警温度设置，开关控制等。
- 通信功能，RS232，点对点，组播，广播。
- 其他功能参考源码。

服务端:
-------
### 运行环境
- Win7 32bit 
- [**JDK 1.6.0_24**](http://www.oracle.com/technetwork/java/javase/downloads/index.html)
- [**JavaEE 6**](http://www.oracle.com/technetwork/java/javaee/tech/index.html)
- [**Tomcat7.0** for win32](http://tomcat.apache.org/download-70.cgi) 
- [**Mysql 5.0+** for win32] (http://dev.mysql.com/downloads/mysql/5.6.html)
- [**Struts2 v2.2**](http://struts.apache.org/development/2.x/) 、[**Spring v2.5**](http://www.springsource.org/)、[**Hibernate  v3.3**](http://www.hibernate.org/)、[**Androidpn**](http://sourceforge.net/projects/androidpn/)

### Java串口通信配置
- 首先要完成这部分配置！ 
- 与WSN进行RS232通信,由于使用Java实现，windows中运行Java 串口通信，先进行配置。<br/>
  RXTX包下载地址：http://rxtx.qbang.org/pub/rxtx/rxtx-2.1-7-bins-r2.zip<br/>
  下载解压后，其配置环境如下：<br/>
  把下载包中rxtx-2.1-7-bins-r2\Windows\i368-mingw32\rxtxSerial.dll 放到你%java_home%\jre\bin下面（%java_home%\jre\bin\rxtxSerial.dll）和C:\Windows\System32下面(C:\Windows\System32\rxtxSerial.dll)<br/>
  下载包中rxtx-2.1-7-bins-r2\RXTXcomm.jar放到%java_home%\jre\lib\ext下面(%java_home%\jre\lib\ext\RXTXcomm.jar)<br/>
   注：%java_home%为你安装JDK的路径，如：C:\Program Files\Java\jdk1.6.0_23<br/>


### Web部署方法
- web部分所需要的框架 jar包 都已经包含在了webroot/WEB-INF/lib目录下。
- 在%Tomcat HOME%\conf中server.xml文件Host标签增加Context子标签如：
\<Context path="/wsnmonitor" docBase="..\WSNServicePlatform\WebRoot" reloadable="true"/\>。
- path属性表示浏览器中的访问路径，docBase属性表示项目文件在文件系统中的目录。
- 运行%Tomcat HOME%\bin中startup.bat启动tomcat,如果在command line 中打印出该服务端的启动日志，并可以访问http://127.0.0.1:8080/wsnmonitor/index.do,则部署成功。
- 基于Androidpn框架的XMPP协议的推送服务通过整合也部署在Tomcat里
- Hibernate ORM构建的持久层，数据库表会在首次启动服务端程序时自动构建，可以在Spring配置文件中修改


### 主要功能：
- 与WSN里协调器通过RS232数据交换
- 对数据进行解析，并持久化到数据库里
- HTTP 接受来自Android终端的数据请求，并返回对应数据。
- XMPP服务保持与Aandroid终端的持久连接，将WSN中解析到的紧急数据(可设定)实时推送给终端。
- 更多参见源码。



Android终端:
-------
### 运行环境
- Android 2.2及以上

### 相关框架
> libs<br>
> ├ [**fastjson.jar**](https://github.com/alibaba/fastjson)<br>
> ├ [**ormlite-android-4.41**](http://ormlite.com/)<br>
> ├ ormlite-core-4.41<br>
> ├ [**universal-image-loader-1.7.0-with-src.jar**](https://github.com/nostra13/Android-Universal-Image-Loader?source=cc)<br>

### 运行方法
- 根据服务端的ip，修改URL


### 主要功能(所有功能都基于与服务端)：
- WSN数据，节点信息查询
- 实时拓扑结构变化查看
- 节点控制
- 推送消息接收等功能
- 推送规则设置









