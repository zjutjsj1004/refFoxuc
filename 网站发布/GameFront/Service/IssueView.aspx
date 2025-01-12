﻿<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="IssueView.aspx.cs" Inherits="Game.Web.Service.IssueView" %>
<%@ Import Namespace="Game.Facade" %>

<%@ Register TagPrefix="qp" TagName="Header" Src="~/Themes/Standard/Common_Header.ascx" %>
<%@ Register TagPrefix="qp" TagName="Footer" Src="~/Themes/Standard/Common_Footer.ascx" %>


<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=EmulateIE7" />

    <link rel="stylesheet" type="text/css" href="/style/layout.css" />
    <link rel="stylesheet" type="text/css" href="/style/global.css" />
    <link rel="stylesheet" type="text/css" href="/style/customer_layout.css" />
    <link rel="stylesheet" type="text/css" href="/style/news_layout.css" />
</head>
<body>

<qp:Header ID="sHeader" runat="server" />

<!--新闻内容开始-->

<div class="main">
    <div class="viewTop">您的位置：首页>>客服中心>>常见问题</div>
    <div class="page"><a id="linLast1" runat="server">上一篇</a><a id="linNext1" runat="server">下一篇</a><a href="/Service/IssueList.aspx" title="返回列表">返回列表</a></div>
	<div class="viewBody">		
		<div class="viewTitle f16 bold" style="border-bottom:dashed 1px #ccc;"><%= title %></div>
		<div class="ViewContent">		　　
		    <%= content %>
		<div class="clear"></div>
	    </div>
    </div>
    <div class="page"><a id="linLast2" runat="server">上一篇</a><a id="linNext2" runat="server">下一篇</a><a href="/Service/IssueList.aspx" title="返回列表">返回列表</a></div>
    <div class="clear"></div>    
</div>
<!--新闻内容结束-->

<qp:Footer ID="sFooter" runat="server" />

</body>
</html>
