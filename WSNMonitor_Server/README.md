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

### License

[**GNU GENERAL PUBLIC LICENSE**](http://www.gnu.org/licenses/gpl.html)

Version 3, 29 June 2007

Copyright © 2007 Free Software Foundation, Inc. <http://fsf.org/>

Everyone is permitted to copy and distribute verbatim copies of this license document, but changing it is not allowed.
