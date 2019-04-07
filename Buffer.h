//creating Buffer.h 

#include<iostream>
#include<map>
#include<random>
#ifndef _Buffer_h
#define _Buffer_h

using namespace std;

int num_hash;
int total_buffer;

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
			/*
				Description: Initialize the Buffer
			
			*/
			status=0;
			valid=0;
			next=NULL;
			prev=NULL;
			nextfree=NULL;
			prevfree=NULL;
		}	
		
		Buffer(int nu,int s1=0,int s2=0,Buffer *n=NULL,Buffer *p=NULL,Buffer *nf=NULL,Buffer *pf=NULL)
		{
			/*
				Description: initialize the Buffer
				input: 
					nu: block number
					s1: status of the block
					s2: content valid or not
					*n: next buffer in the hash
					*p: previous buffer in the hash
					*nf: next free buffer
					*pf: previous free buffer
			*/
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
			/*
				Description: sets the block number to n
				input: 
					n: block number
					
			*/
			num=n;
		}	

		void setstatus(int s)
		{
			/*
				Description: sets the status to s
				input: 
					s:status value
					
			*/
			status=s;
		}
		void setvalid(int v)
		{
			/*
				Description: sets the valid to v
				input: 
					s: valid value
					
			*/
			valid=v;
		}
		int getnum()
		{
			/*
				Description: returns the block number, the buffer is storing content of
				output: 
					block number					
			*/
			return num;
		}
	
		int getstatus()
		{
			/*
				Description: returns the status of buffer
				output: 
					status		
			*/
			return status;
		}
		
		int getvalid()
		{
			/*
				Description: returns the valid status of buffer
				output: 
					valid		
			*/
			return valid;
		}
	};



Buffer *insert_in_Hash(Buffer *Hashq,Buffer *node)
{
/*
	Description: inserts buffer into Hash at the time of initialization
	
	input: 
		Hashq:a Hash queue

		node: Buffer to be added to hash queue
	output:
		the hash queue
*/
	if(Hashq==NULL)
	{
		Hashq=node;
		node->next=node;
		node->prev=node;
	}
	else
	{
		node->prev=Hashq;
		node->next=Hashq->next;
		Hashq->next=node;
		node->next->prev=node;
		Hashq=node;
	}
	return Hashq;
}

Buffer *insert_in_free(Buffer *free,Buffer *node)
{
/*
	Description: inserts buffer into free list
	
	input: 
		free: free list header

		node: Buffer to be added to free list
	output:
		free list header
*/
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
/*
	Description: inserts buffer into begining of free list
	
	input: 
		free: free list header

		node: Buffer to be added to free list
	output:
		free list header
*/
	if(free==NULL)
	{
		free=node;
		node->nextfree=node;
		node->prevfree=node;
	}
	else
	{
		Buffer *temp;
		temp=free;
		node->prevfree=temp;
		node->nextfree=temp->nextfree;
		temp->nextfree=node;
		node->nextfree->prevfree=node;
	}
	return free;
}

void remove_from_hash(Buffer *Hashque[],int newblock,Buffer *node)
{
/*
	Description: removes a buffer from one hashque and insert into another
	
	input: 
		Hashque: all the hash queue headers

		newblock: new block to be copied in buffer

		node: Buffer to be added to free list
*/
	int temp=node->getnum();
	if(Hashque[temp%num_hash]->next==Hashque[temp%num_hash])
	{
		Hashque[temp%num_hash]=NULL;
	}
	if(Hashque[temp%num_hash]==node)
	{
		Hashque[temp%num_hash]=Hashque[temp%num_hash]->next;
	}
	node->prev->next=node->next;
	node->next->prev=node->prev;	
	node->setnum(newblock);

	Hashque[newblock%num_hash]=insert_in_Hash(Hashque[newblock%num_hash], node);
	node->setvalid(0);
}

Buffer *remove_from_free(Buffer *free,Buffer *node)
{	
/*
	Description: removes buffer from the free list
	
	input: 
		free: free list header

		node: Buffer to be added to free list
	output:
		free list header
*/
	if(node!=free)
	{
		node->prevfree->nextfree=node->nextfree;
		node->nextfree->prevfree=node->prevfree;
		node->nextfree=NULL;
		node->prevfree=NULL;
		return free;
	}
	else
	{
		
		if(free->nextfree==free)
		{
			free->nextfree=NULL;
			free->prevfree=NULL;
			free=NULL;
			return NULL;
			
		}
		else
		{
			free=free->prevfree;
			free->nextfree=node->nextfree;
			node->nextfree->prevfree=free;
			node->nextfree=NULL;
			node->prevfree=NULL;
		}
	}
	return free;
}


Buffer *search(Buffer *Hashque[],int blocknum)
{
/*
	Description: searches for a buffer in the hashque
	
	input: 
		hashque: all the hash queue headers

		blocknum: block to be searched in the buffer
	output:
		Buffer if found else NULL
*/

	Buffer *temp,*head;
	temp=Hashque[blocknum%num_hash];
	head=temp;
	if(temp==NULL)
	{
		return NULL;
	}
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

Buffer *getblk(Buffer *Hash[],Buffer* &free,int Blocknum,map <int , int> &Lock_table)
{
/*
	Description: searches for a buffer in the hashque and gets a new one if it is not allocated allocate it else get a free buffer or wait for buffer to be free
	
	input: 
		hash: all the hash queue headers
	
		free: free list header

		blocknum: block to be searched in the buffer
	output:
		Buffer if found else NULL
*/
	Buffer *temp,*reqBuf;
	reqBuf=NULL;
	while(reqBuf==NULL)
	{
		temp=search(Hash,Blocknum);
		if(temp!=NULL)
		{
			if(temp->getstatus()==1)			//5th
			{
			
				return NULL;		//sleep
				//continue;
			}
			temp->setstatus(1);				//1st
			Lock_table[temp->getnum()]=1;
			free=remove_from_free(free,temp);
			return temp;
		}
		else
		{
			if(free==NULL)					//4th
			{
				return NULL;		//sleep
				//continue;
			}
			temp=free->nextfree;
			free=remove_from_free(free,temp);
			if(temp->getstatus()==-1)			//3rd
			{
				//write to disk 
				free=insert_in_free_begin(free,temp);
				temp->setstatus(0);
				Lock_table[temp->getnum()]=0;
				continue;
			}
			Lock_table.erase(temp->getnum());
			remove_from_hash(Hash,Blocknum,temp);		//2nd
			temp->setstatus(1);
			Lock_table[temp->getnum()]=1;
			temp->setvalid(1);				//asuming it reads content
			return temp;
		}
	}
}

Buffer *brlse(Buffer* &free,Buffer *node,map <int , int> &Lock_table)
{
/*
	Description: releases a locked buffer
	
	input: 
		free: free list header

		node: buffer to be released and added to free list
	output:
		free list header
*/
	//wakeup
	//raise execution level
	int not_old=rand()%2;
	int sta;
	if(not_old==1)		// and not old
	{
		sta=1;					// free or delayed write
		free=insert_in_free(free,node);
	}
	else
	{	
		sta=-1;
		free=insert_in_free_begin(free,node);
	}
	//lower execution
	node->setstatus(sta);
	Lock_table[node->getnum()]=sta;
	return free;
}


void DisplayBuf(Buffer *Hash[],Buffer *free)
{
/*
	Description: display the content of Hash queue and free list
	
	input: 
		hash: all the hash queue headers

		free: free list header
*/
	Buffer *temp;
	for(int i=0;i<num_hash;i++)
	{
		cout<<"Hash "<<i<<" :- ";
		if(Hash[i]==NULL)
		{
			cout<<" NULL "<<endl;
			continue;		
		}
		temp=Hash[i]->next;
		while(temp!= Hash[i])
		{
			cout<<temp->getnum()<<" ";	
			temp=temp->next;
		}	
		cout<<temp->getnum()<<endl;
	}
	cout<<endl;
	cout<<"free list :- ";
	if(free==NULL)
	{
		cout<<" NULL "<<endl;
	}	
	else
	{
		temp=free->nextfree;
		while(temp!= free)
		{
			cout<<temp->getnum()<<" ";
			temp=temp->nextfree;
		}
		cout<<temp->getnum()<<endl;
	}
}

void init(Buffer *Hash[],Buffer* &free,int hash,int buf,map <int , int> &Lock_table)
{
	num_hash=hash;
	total_buffer=buf;
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
		Lock_table.insert(pair<int,int>(temp->getnum(),0));
	}
}

#endif
