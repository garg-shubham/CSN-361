#include <bits/stdc++.h>
using namespace std;
int main(int argc,char* argv[])
{
	string data,key;
	cin>>data>>key;
	int i = atoi(argv[1]);
	cout<<i<<"\n";
	if(i < data.size())
		if(data[i]=='0')
			data[i]='1';
		else
			data[i]='0';
	cout<<data<<"\n";
	cout<<key<<"\n";
}