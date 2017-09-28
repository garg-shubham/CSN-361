#include <bits/stdc++.h>
using namespace std;
int main()
{
	string data,key;
	// scanf("%s",&data);
	// scanf("%s",&key);
	cin>>data;
	cin>>key;
	int n = data.size();
	int k = key.size();
	string s;
	s = data;
	for(int i=0;i<k-1;i++)
		data += '0';
	for(int i=0;i<n;i++)
	{
		if(data[i]=='0')
			continue;
		for(int j=0;j<k;j++)
		{
			if(data[i+j]==key[j])
				data[i+j]='0';
			else
				data[i+j]='1';
		}
	}
	for(int i=n;i<data.size();i++)
		s += data[i];
	//printf("%s\n",s);
	cout<<s<<"\n";
	cout<<key<<"\n";
}