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


