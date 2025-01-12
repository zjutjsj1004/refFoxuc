USE [QPNativeWebDB]
GO
/****** 对象:  Table [dbo].[JiaoChengQuanXian]    脚本日期: 02/23/2013 21:14:08 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[JiaoChengQuanXian](
	[JiaoChengQuanXianID] [int] IDENTITY(1,1) NOT NULL,
	[NewsID] [int] NOT NULL,
	[UserID] [int] NOT NULL,
	[ShengQingDate] [datetime] NOT NULL CONSTRAINT [DF__JiaoCheng__Sheng__245D67DE]  DEFAULT (getdate()),
	[ShengQingIP] [nchar](15) COLLATE Chinese_PRC_CI_AS NOT NULL CONSTRAINT [DF__JiaoCheng__Sheng__25518C17]  DEFAULT ('000.000.000.000'),
	[ShengHeDate] [datetime] NOT NULL CONSTRAINT [DF__JiaoCheng__Sheng__2645B050]  DEFAULT (getdate()),
	[ShengHeiIP] [nchar](15) COLLATE Chinese_PRC_CI_AS NOT NULL CONSTRAINT [DF__JiaoCheng__Sheng__2739D489]  DEFAULT ('000.000.000.000'),
	[ShengHeiRen] [nchar](15) COLLATE Chinese_PRC_CI_AS NOT NULL CONSTRAINT [DF__JiaoCheng__Sheng__282DF8C2]  DEFAULT ('000.000.000.000'),
	[isShengHe] [tinyint] NOT NULL CONSTRAINT [DF__JiaoCheng__isShe__29221CFB]  DEFAULT ((0)),
 CONSTRAINT [PK_JiaoChengQuanXian] PRIMARY KEY CLUSTERED 
(
	[JiaoChengQuanXianID] ASC
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY]
) ON [PRIMARY]
