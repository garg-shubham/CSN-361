#include <bits/stdc++.h>
using namespace std;

#define HLEN 16

struct pkt{
	string message;
	int len;
};

vector <pkt> packets;
void fragment(string msg,int MTU)
{
	if(MTU < HLEN) 
	{
		cout<<"Fragmentation Not Possible\n";
		return;
	}
	int n = msg.size();
	int effective = MTU - HLEN;
	int sent_bytes = effective/8;
	if(sent_bytes<1)
	{
		cout<<"Fragmentation Not Possible with this size of MTU\n";
		return;
	}
	int num_pac = (n + 8*sent_bytes -1)/(8*sent_bytes);
	for(int i=0;i<num_pac -1;i++)
	{
		pkt* temp = new pkt;
		temp-> len = 8*sent_bytes + HLEN;
		temp->message ="";
		for(int j=0;j<8*sent_bytes;j++)
		{
			temp ->message = temp -> message + msg[8*sent_bytes*i + j];
		}
		packets.push_back(*temp);
	}
	if(num_pac*8*sent_bytes > n)
	{
		string x="";
		int i = 8*sent_bytes*(num_pac-1);
		while(i<n)
		{
			x+=msg[i++];
		}
		pkt* temp = new pkt;
		temp-> len = 8*sent_bytes + HLEN;
		temp->message =x;
		packets.push_back(*temp);
	}
	cout<<"Fragments:\n";
	for(int i=0;i<packets.size();i++)
	{
		cout<<packets[i].message<<"\n";
	}
	return;
}

void reassemble(vector<pkt> v)
{
	if(v.size()==0) return;
	string s = ""; 
	for(int i=0;i<packets.size();i++)
	{
		s+= packets[i].message;
	}
	cout<<"Reassembled messaged:\n"<<s<<"\n";
	return ;
}

int main()
{
	string msg;
	cout<<"Enter message to be fragmented:\n";
	cin>>msg;
	int MTU;
	int length = msg.size();
	cout<<"Enter MTU size (greater than Header Length + 8: "<<HLEN+8<<")\n";
	cin>>MTU;
	fragment(msg,MTU);
	reassemble(packets);
}