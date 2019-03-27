#include<iostream>
#include<thread>
#include<unistd.h>
#include<map>
#include<condition_variable>
#include<random>

#define total_block 50
#define total_buffer 20 
#define num_hash 4
#define total_thread 1

using namespace std;

condition_variable cv;
map <int , thread::id> Lock_table;
map <thread::id , int> req_table;
thread::id tid;

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
			valid=s2;
			next=n;
			prev=p;
			nextfree=nf;
			prevfree=pf;
		}

		void setnum(int n)
		{
			num=n;
		}	

		void setstatus(int s)
		{
			status=s;
		}
		void setvalid(int v)
		{
			valid=v;
		}
		int getnum()
		{
			return num;
		}
	
		int getstatus()
		{
			return status;
		}
		
		int getvalid()
		{
			return valid;
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

Buffer *insert_in_free_begin(Buffer *free,Buffer *node)
{
	if(free==NULL)
	{
		free=node;
		node->nextfree=node;
		node->prevfree=node;
	}
	else
	{
		Buffer *temp;
		temp=free->nextfree;
		node->prevfree=temp;
		node->nextfree=temp->nextfree;
		temp->nextfree=node;
		node->nextfree->prevfree=node;
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

void remove_from_free(Buffer *free,Buffer *node)
{	
	if(free==NULL)
	{
		return;
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
		temp=search(Hash,Blocknum);
		if(temp!=NULL)
		{
			if(temp->getstatus()==1)
			{
				//sleep
				continue;
			}
			temp->setstatus(1);
			remove_from_free(free,temp);
			return temp;
		}
		else
		{
			if(free==NULL)
			{
				//sleep
				continue;
			}
			temp=free->nextfree;
			remove_from_free(free,temp);
			if(temp->getstatus()==-1)
			{
				//sleep
				temp->setstatus(0);
				continue;
			}
			remove_from_hash(Hash,Blocknum,temp);
			return temp;
		}
	}
}

Buffer *brlse(Buffer *free,Buffer *node)
{
	//wakeup
	//raise execution level
	int release=0;
	if(node->getvalid()==1)		// and not old
	{
		free=insert_in_free(free,node);
	}
	else
	{	
		release=-1;
		free=insert_in_free_begin(free,node);
	}
	//lower execution
	node->setstatus(release);
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

void child()
{
	for(int i=0;i<10;i++)
	{
		int ran=rand()%total_block;
		//request buffer
		req_table[this_thread::get_id()]=ran;
		tid=this_thread::get_id();
		cout<<"request"<<" "<<Lock_table[ran]<<endl;
		while(Lock_table[ran]!=this_thread::get_id())
			sleep(0.1);

		//sleep((rand()%10)*0.1);	//working

		
	}
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
		Lock_table.insert(pair<int,thread::id>(temp->getnum(),0));
	}

	thread t[total_thread];
	for( int i=0;i<total_thread;i++)
	{
		cout<<"t"<<i<<" ";
		req_table.insert(pair<thread::id,int>(t[i].get_id(),0));
		t[i]=thread(child);
	}
	sleep(0.1);
	int f=0;
	for(;;)
	{
		for( int i=0;i<total_thread;i++)
		{
			if(req_table[tid]!=0)
			{
				Lock_table[req_table[tid]]=tid;
				cout<<"getblock";
				f=1;
			}
		}
		if(f==1)
			break;
	}
	
	Buffer *temp1,*temp2,*temp3;

	temp1=getblk(Hash,free,23);
	
	DisplayBuf(Hash,free);

	
	temp2=getblk(Hash,free,12);
	
	DisplayBuf(Hash,free);

	brlse(free,temp1);	

	temp3=getblk(Hash,free,24);
	
	DisplayBuf(Hash,free);

	brlse(free,temp2);
	brlse(free,temp3);
	
	DisplayBuf(Hash,free);

	t[0].join();	
	
	return 1;
}
