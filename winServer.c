// winServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <winsock2.h>//添加winsock头文件
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#pragma comment(lib, "WS2_32.lib")          //add ws2_32.lib

#define MAXLINE 4096
#define BUFFER_SIZE 1024

int main()
{

	WORD    wVersionRequested;//Windows异步套接字的启动命令
	WSADATA wsaData;//储被WSAStartup函数调用后返回的Windows Sockets数据
	int     err, iLen;
	SOCKET sockSrv;//套接字
	wVersionRequested = MAKEWORD(2, 2);

	char filename[100];
	char filepath[100];

	char *buffer;//file buffer
	int fileTrans;
	int lenfilepath;
	FILE *fp;
	int writelength;

	buffer = (char *)malloc(sizeof(char)*BUFFER_SIZE);
	//bzero(buffer,BUFFER_SIZE);
	memset(buffer, 0, sizeof(buffer));

	if (WSAStartup(wVersionRequested, &wsaData) != 0)//初始化WSAStartup
	{
		printf("WSAStartup error!\n");
		return -1;
	}

	if ((sockSrv = socket(AF_INET, SOCK_STREAM, 0))<0) { //建立socket
		printf("socket error!\n");
		return -2;
	}
	SOCKADDR_IN addrSrv;//确定服务器scokaddr_in结构体
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_addr.s_addr = htonl(INADDR_ANY);//点分十进制IP转换
	addrSrv.sin_port = htons(9000);

	if (bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) != 0)//使用bind绑定套接字
	{

		printf("bind error!\n");
		return -3;
	}

	if (listen(sockSrv, 5) != 0)//监听
	{

		printf("listen error!\n");
		return -4;
	}
	printf("Sever build finished ...\n");

	while (1)
	{
		SOCKET temp;
		if ((temp = accept(sockSrv, (SOCKADDR*)NULL, NULL))>0)//使用accept接受连接请求
		{
			memset(filename, '\0', sizeof(filename));
			memset(filepath, '\0', sizeof(filepath));

			lenfilepath = recv(temp, filepath, 100, 0);//使用recv接收传来的数据（文件路径）
			printf("filepath :%s\n", filepath);
			if (lenfilepath<0)
			{
				printf("recv error!\n");
			}
			else
			{
				int i = 0, k = 0;
				for (i = strlen(filepath); i >= 0; i--)
				{
					if (filepath[i] != '/')
					{
						k++;
					}
					else
						break;
				}
				strcpy(filename, filepath + (strlen(filepath) - k) + 1);//客户端输入路径+文件名
			}
			printf("filename :%s\n", filename);
			fp = fopen(filename, "w");//打开文件
			if (fp != NULL)
			{
				while (fileTrans = recv(temp, buffer, BUFFER_SIZE, 0))//使用recv接收文件内容
				{
					if (fileTrans<0)//判断recv函数返回的状态
					{
						printf("recv error!\n");
						break;
					}
					writelength = fwrite(buffer, sizeof(char), fileTrans, fp);//将接收到的内容放入缓冲区后将缓冲区内容写入文件
					if (writelength <fileTrans)
					{
						printf("write error!\n");
						break;
					}
					memset(buffer, '\0', sizeof(buffer));
				}
				printf("recv finished!\n");
				fclose(fp);//关闭文件
			}
			else
			{
				printf("filename is null!\n");

			}
			closesocket(temp);


		}
		else
		{
			printf("accept error!\n");
			closesocket(temp);
		}
	}
	closesocket(sockSrv);//关闭socket
	WSACleanup();//WSACleanup函数
	return 0;

}
