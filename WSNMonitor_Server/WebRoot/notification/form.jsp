<%@ page language="java" errorPage="/error.jsp" pageEncoding="UTF-8" contentType="text/html;charset=UTF-8"%>
<%@ include file="/includes/taglibs.jsp"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
    <title>Admin Console</title>
	<meta name="menu" content="notification" />    
</head>

<body>

<h1>WSN信息推送</h1>

<%--<div style="background:#eee; margin:20px 0px; padding:20px; width:500px; border:solid 1px #999;">--%>
<div style="margin:20px 0px;">
<form action="notification.do?action=send" method="post" style="margin: 0px;">
<table width="600" cellpadding="4" cellspacing="0" border="0">
<tr>
	<td width="20%">推送方式:</td>
	<td width="80%">
		<input type="radio" name="broadcast" value="Y" checked="checked" />  广播 
        <input type="radio" name="broadcast" value="N" /> 点对点 
	</td>
</tr>
<tr id="trUsername" style="display:none;">
	<td>用户名:</td>
	<td><input type="text" id="username" name="username" value="" style="width:380px;" /></td>
</tr>
<tr>
	<td>标题:</td>
	 <td><input type="text" id="title" name="title" value="WSN监控系统服务平台" style="width:380px;" /></td> 
	<!--<td><input type="text" id="title" name="title" value="绿城物业服务平台" style="width:380px;" /></td>-->
</tr>
<tr>
	<td>消息正文:</td>
	<td><textarea id="message" name="message" style="width:380px; height:80px;" >5号节点，温度超过32.5℃，请注意!!!</textarea></td>
</tr>
<%--
<tr>
	<td>Ticker:</td>
	<td><input type="text" id="ticker" name="ticker" value="" style="width:380px;" /></td>
</tr>
--%>
<tr>
	<td>URI:</td>
	<td><input type="text" id="uri" name="uri" value="" style="width:380px;" />
	    <br/><span style="font-size:0.8em">ex) http:hellowsn.vicp.cc/index.do</span>
	</td>
</tr>
<tr>
	<td>&nbsp;</td>
	<td><input type="submit" value="Submit" /></td>
</tr>
</table> 
</form>
</div>

<script type="text/javascript"> 
//<![CDATA[
 
$(function() {
	$('input[name=broadcast]').click(function() {
		if ($('input[name=broadcast]')[0].checked) {
			$('#trUsername').hide();
		} else {
			$('#trUsername').show();
		}
	});
	
	if ($('input[name=broadcast]')[0].checked) {
		$('#trUsername').hide();
	} else {
		$('#trUsername').show();
	}	
});
 
//]]>
</script>

</body>
</html>
