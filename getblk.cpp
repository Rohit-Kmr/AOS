#include<iostream>
#define total_buffer 20 
#define num_hash 4

using namespace std;

class Buffer{
		
		int num;
		int status;		//0-free,1-locked,-1-delayed write
		int valid;		//0-invaild data, 1-vaild data
		public:
		Buffer *next;
		Buffer *prev;
		Buffer *nextfree;
		Buffer *prevfree;
		
		Buffer()
		{
			status=0;
			valid=0;
			next=NULL;
			prev=NULL;
			nextfree=NULL;
			prevfree=NULL;
		}	
		
		Buffer(int nu,int s1=0,int s2=0,Buffer *n=NULL,Buffer *p=NULL,Buffer *nf=NULL,Buffer *pf=NULL)
		{
			num=nu;
			status=s1;
			status2=s2;
			next=n;
			prev=p;
			nextfree=nf;
			prevfree=pf;
		}

		void setnum(int n)
		{
			num=n;
		}	

		void setstatus(int s1,int s2)
		{
			status=s1;
			status=s2;
		}
		int getnum()
		{
			return num;
		}
	
		int getstatus1()
		{
			return status;
		}
		
		int getstatus2()
		{
			return status;
		}
	};


Buffer *insert_in_Hash(Buffer *Hash,Buffer *node)
{
	if(Hash==NULL)
	{
		Hash=node;
		node->next=node;
		node->prev=node;
	}
	else
	{
		node->prev=Hash;
		node->next=Hash->next;
		Hash->next=node;
		node->next->prev=node;
		Hash=node;
	}
	return Hash;
}

Buffer *insert_in_free(Buffer *free,Buffer *node)
{
	if(free==NULL)
	{
		free=node;
		node->nextfree=node;
		node->prevfree=node;
	}
	else
	{
		node->prevfree=free;
		node->nextfree=free->nextfree;
		free->nextfree=node;
		node->nextfree->prevfree=node;
		free=node;
	}
	return free;
}


void remove_from_hash(Buffer *Hashque[],int newblock,Buffer *node)
{
	int temp=node->getnum();
	if(Hashque[temp%num_hash]==NULL)
	{
		return;
	}
	node->prev->next=node->next;
	node->next->prev=node->prev;

	if(Hashque[temp%num_hash]->next==Hashque[temp%num_hash])
	{
		Hashque[temp%num_hash]=NULL;
	}	
	node->setnum(newblock);
	
	Hashque[newblock%num_hash]=insert_in_Hash(Hashque[newblock%num_hash], node);
}

Buffer *remove_from_free(Buffer *free,Buffer *node)
{	
	if(free==NULL)
	{
		return free;
	}
	if(free==node);
	{
		if(free->nextfree==free)
		{
			free=NULL;
		}
		else
		{
			free=free->nextfree;
		}
	}
	Buffer *temp;
	temp=free->nextfree;
	while( temp!=free && temp !=node)
	{
		temp=temp->nextfree;
	}	
	temp->prevfree->nextfree=temp->nextfree;
	temp->nextfree->prevfree=temp->prevfree;
	temp->nextfree=NULL;
	temp->prevfree=NULL;
	return free;
}


Buffer *search(Buffer *Hashque[],int blocknum)
{
	Buffer *temp,*head;
	temp=Hashque[blocknum%num_hash];
	head=temp;
	if(temp->getnum()==blocknum)
	{
		return temp;
	}
	else
	{
		temp=temp->next;
		while(temp!=head)
		{
			if(temp->getnum()==blocknum)
			{
				return temp;
			}
			temp=temp->next;
		}
	}
	return NULL;
}

Buffer *getblk(Buffer *Hash[],Buffer *free,int Blocknum)
{
	Buffer *temp,*reqBuf;
	reqBuf=NULL;
	while(reqBuf==NULL)
	{
		temp=search(Hash,Blcoknum);
		if(temp!=NULL)
		{
			if(temp->getstatus1()
	
}

void DisplayBuf(Buffer *Hash[],Buffer *free)
{
	Buffer *temp;
	for(int i=0;i<num_hash;i++)
	{
		cout<<"Hash "<<i<<" :- ";
		temp=Hash[i]->next;
		while(temp!= Hash[i])
		{
			cout<<temp->getnum()<<" ";	
			temp=temp->next;
		}	
		cout<<temp->getnum()<<endl;
	}
	cout<<endl;
	temp=free->nextfree;
	cout<<"free list :- ";
	while(temp!= free)
	{
		cout<<temp->getnum()<<" ";
		temp=temp->nextfree;
	}
	cout<<temp->getnum()<<endl;
}

int main()
{	
	// Creating hash table and Free list
	Buffer *Hash[num_hash],*free;
	for(int i=0;i<num_hash;i++)
	{
		Hash[i]=NULL;
	}
	free=NULL;
	for(int i=0;i<total_buffer;i++)
	{
		Buffer *temp;
		int hashnum=i%num_hash;
		temp=new Buffer(i);
		Hash[hashnum]=insert_in_Hash(Hash[hashnum],temp);
		free=insert_in_free(free,temp);
	}

	remove_from_hash(Hash,23,search(Hash,12));
		
	free=remove_from_free(free,search(Hash,14));

	DisplayBuf(Hash,free);

	return 1;
}
