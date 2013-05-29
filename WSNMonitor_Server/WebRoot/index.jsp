<%@ page language="java" errorPage="/error.jsp" pageEncoding="UTF-8" contentType="text/html;charset=UTF-8"%>
<%@ include file="/includes/taglibs.jsp"%>
<%
	String path = request.getContextPath();
	String basePath = request.getScheme() + "://"
			+ request.getServerName() + ":" + request.getServerPort()
			+ path + "/";
%>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
	<base href="<%=basePath %>">
    <title>Admin Console</title>
	<meta name="menu" content="home" />    
</head>

<body>

<h1>Welcome!</h1>

<div style="text-align:center"><img src="<%=basePath %>/images/androidpn.png" border="0" /></div>

</body>
</html>
