#include <bits/stdc++.h>
using namespace std;
int main()
{
	string data,key;
	cin>>data>>key;
	int n = data.size();
	int k = key.size();
	for(int i=0;i<=n-k;i++)
	{
		if(data[i]=='0')
			continue;
		for(int j=0;j<k;j++)
		{
			if(data[i+j]==key[j])
				data[i+j] = '0';
			else
				data[i+j] = '1';
		}
	}
	for(int i=0;i<n;i++)
		if(data[i] != '0')
		{
			cout<<"Error\n";
			return 0;
		}
	cout<<"Packet recieved successfully\n";
	return 0;
}