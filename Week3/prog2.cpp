#include <bits/stdc++.h>
#include <stdlib.h>
using namespace std;
int stoi(string x)
{
	int ans=0;
	int i=0;
	while(i<x.size())
	{
		ans = 10*ans + x[i++]-'0';
	}
	return ans;
}
void dec_to_hex(int x,string &p)
{
	if(x==0) {
		p = "00";
		return;
	}
	string ans = "";
	for(int i=0;i<2;i++)
	{
		int temp = x%16;
		x/=16;
		if(temp<10)
			ans += '0' + temp;
		else
		{
			char p = temp-10 +'A';
			ans += p;
		}
	}
	reverse(ans.begin(),ans.end());
	p= ans;

}
int main()
{
	string ipv4;
	cin>>ipv4;
	string ipv6 = "2002:";
	string temp="";
	int i=0,cnt=0;
	while(i<ipv4.size())
	{
		if(ipv4[i]=='.')
		{
			string p;
			int x = stoi(temp);
			dec_to_hex(x,p);
			ipv6 += p;
			if(cnt&1)
			{
				ipv6 +=':';
			}
			cnt++;
			temp = "";
		}
		else
			temp += ipv4[i];
		i++;
	}
		string p;
		int x = stoi(temp);
		dec_to_hex(x,p);
		ipv6 += p;
		ipv6 += "::1/64";
		cout<<ipv6<<"\n";
}