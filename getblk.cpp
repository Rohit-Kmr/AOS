#include<iostream>
#define total_buffer 20 

using namespace std;

class Buffer{
		public:
		int num;
		int status;		//0-free,1-locked,-1-delayed write
		int status2;		//0-invaild data, 1-vaild data
		Buffer *next;
		Buffer *prev;
		Buffer *nextfree;
		Buffer *prevfree;
		
		Buffer()
		{
			status=0;
			next=NULL;
			prev=NULL;
			nextfree=NULL;
			prevfree=NULL;
			cout<<"hello ";
		}		
	   };


int main()
{	
	// Creating hash table
	Buffer *HashHead[4],*HashEnd[4];
	for(int i=0;i<4;i++)
	{
		HashEnd[i]=NULL;
		HashHead[i]=NULL;
	}
	for(int i=0;i<total_buffer;i++)
	{
		Buffer *temp=(Buffer *)malloc(sizeof(Buffer));
		int hashnum=i%4;
		temp->num=i;
		if(HashHead[hashnum]==NULL)
		{
			HashEnd[hashnum]=temp;
			HashHead[hashnum]=temp;
		}
		else
		{
			temp->prev=HashEnd[hashnum];
			HashEnd[hashnum]->next=temp;
			HashEnd[hashnum]=temp;
		}
	}
	
	Buffer *temp=(Buffer *)malloc(sizeof(Buffer));
	temp=HashEnd[0];
	while(temp!=NULL)
	{
		cout<<temp->num<<endl;	
		temp=temp->prev;
	}

	temp=HashEnd[1];
	while(temp!=NULL)
	{
		cout<<temp->num<<endl;	
		temp=temp->prev;
	}

	temp=HashEnd[2];
	while(temp!=NULL)
	{
		cout<<temp->num<<endl;	
		temp=temp->prev;
	}

	temp=HashEnd[3];
	while(temp!=NULL)
	{
		cout<<temp->num<<endl;	
		temp=temp->prev;
	}
	return 1;
}
