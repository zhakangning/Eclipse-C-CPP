/*
 * dataHandler.cpp
 *
 *  Created on: 2018年1月4日
 *      Author: zhakangning
 */

#include "dataHandler.h"

string getMD5(const string &srcStr)
{
	string cmd = "echo -n '";
	cmd += srcStr;
	cmd += "'|md5sum|cut -d ' ' -f1";
	FILE *file = popen(cmd.c_str(), "r");
	if(!file){
		cout<<"popen error.\n";
		return "";
	}

	char buf[256];
	string md5str;
	while(fgets(buf, sizeof(buf),file)!=NULL){
		md5str += string(buf);
	}
	pclose(file);
	return md5str.substr(0, md5str.length() - 1);
}

string getAuthorization(const string &data_recv, const string &method, const string &uri)
{

	string nonce = subStr(data_recv, "nonce=\"", 48);
	cout<<"nonce = "<<nonce<<endl;

	string srcStr = "admin:DX-231T-D:cdsfITC123";
	string h1 = getMD5(srcStr);
	cout<<"h1 = "<<h1<<endl;
	srcStr = method+":"+uri;
	string h2 = getMD5(srcStr);
	cout<<"h2 = "<<h2<<endl;
	srcStr = h1+":"+nonce+":"+h2;
	string response = getMD5(srcStr);

	string authorization = "Digest username=\"admin\",realm=\"DX-231T-D\",nonce=\"\",uri=\"\",response=\"\"";
	unsigned long pos = authorization.find("nonce=")+7;
	authorization.insert(pos, nonce);
	pos = authorization.find("uri=", pos)+5;
	authorization.insert(pos, uri);
	authorization.insert(authorization.size()-1, response);

	return authorization;
}

//long类型转换字符串，兼容int转字符串
string long2str(long n)
{
	char num[32];
	snprintf(num, 32, "%ld", n);
	return string(num);
}

string float2str(float n)
{
	char num[32];
	snprintf(num, 32, "%f", n);
	return string(num);
}

/*
 * 在一个字符串中找到标志字符串，插入指定字符串，如果没有找到标志字符串，就追加到最后
 * src:源字符串
 * flag:指定字符串内容
 * subStr:将要插入源字符串的内容
 */
void insertStr(string &src, const string &flag, const string &subStr)
{
	src.insert(src.find(flag)+flag.size(), subStr);
}

//取出源字符串中指定内容后面的字串，n表示取字串的个数
//如果原字符串中找不到指定的字符串，就返回空字符串
string subStr(const string &src, const string &flag, const unsigned long &n)
{
	return src.substr(src.find(flag)+flag.size(), n);
}

/*
 * 取出源字符串中两个指定字符串之间的内容
 */
string subStr(const string &src, const string &head, const string &end)
{
	unsigned long pos = src.find(head)+head.size();
	return src.substr(pos, src.find(end, pos)-pos);
}

/*
 * 获取当前系统时间
 * 格式为：2018-01-18T08:37:04
 * 如果获取时间出错，将返回：1970-01-01T07:59:59
 */
string getTime()
{
	time_t t = time(0);
	struct tm *tm = localtime(&t);
	char buf[32];
	//年
	sprintf(buf, "%d-", tm->tm_year+1900);

	//月
	if(10 > tm->tm_mon) sprintf(buf + 5, "0%d-", tm->tm_mon+1);
	else sprintf(buf + 5, "%d-", tm->tm_mon+1);

	//日
	if(10 > tm->tm_mday) sprintf(buf + 8, "0%dT", tm->tm_mday);
	else sprintf(buf + 8, "%dT", tm->tm_mday);

	//时
	if(10 > tm->tm_hour) sprintf(buf + 11, "0%d:", tm->tm_hour);
	else sprintf(buf + 11, "%d:", tm->tm_hour);

	//分
	if(10 > tm->tm_min) sprintf(buf + 14, "0%d:", tm->tm_min);
	else sprintf(buf + 14, "%d:", tm->tm_min);

	//秒
	if(10 > tm->tm_sec) sprintf(buf + 17, "0%d", tm->tm_sec);
	else sprintf(buf + 17, "%d", tm->tm_sec);

	return string(buf);
}

string convertTimeNum2str(const time_t &nTime)
{
	struct tm *tm = localtime(&nTime);
	char buf[32];
	//年
	sprintf(buf, "%d-", tm->tm_year+1900);
	//月
	if(10 > tm->tm_mon) sprintf(buf + 5, "0%d-", tm->tm_mon+1);
	else sprintf(buf + 5, "%d-", tm->tm_mon+1);
	//日
	if(10 > tm->tm_mday) sprintf(buf + 8, "0%dT", tm->tm_mday);
	else sprintf(buf + 8, "%dT", tm->tm_mday);
	//时
	if(10 > tm->tm_hour) sprintf(buf + 11, "0%d:", tm->tm_hour);
	else sprintf(buf + 11, "%d:", tm->tm_hour);
	//分
	if(10 > tm->tm_min) sprintf(buf + 14, "0%d:", tm->tm_min);
	else sprintf(buf + 14, "%d:", tm->tm_min);
	//秒
	if(10 > tm->tm_sec) sprintf(buf + 17, "0%d", tm->tm_sec);
	else sprintf(buf + 17, "%d", tm->tm_sec);
	return string(buf);
}

string getFileNameFromCMD(const string &cmd)
{
	string name = cmd.substr(cmd.find(" ")+1);
	unsigned long pos = 0;
	while(1){
		pos = name.find("/");
		if(pos == string::npos) break;

		name = name.substr(pos+1);
	}
	return name;
}
