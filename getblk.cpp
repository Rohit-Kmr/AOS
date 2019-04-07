#include<iostream>
#include<thread>
#include<unistd.h>
#include<map>
#include<condition_variable>
#include<random>
#include<mutex>
#include"Buffer.h"

#define total_block 50
#define total_buffer 2
#define num_hash 4
#define total_thread 4

using namespace std;

condition_variable par,req;
condition_variable free_cv,hash_cv;
mutex parm,childm;			//childm used for sync
mutex respm,iom;			//iom used for sync
mutex free_m,hash_m;			// rest of mutex is used for condition variable
map <int , int> req_table;
map <int , int> Lock_table;
int req_thread=-1,req_t=-1,response=-1;
int free_NULL=1; 

Buffer *response_b=NULL,*requ_b=NULL;

unique_lock<mutex> freel(free_m);
unique_lock<mutex> hashl(hash_m);

void child(int process_num)
{ 
/*
	Description: child thread
	
	input:
		process_num: thread number activated
*/
	Buffer *temp;
	int input=-1;
	while(true){
			
		int sat;
		{
			unique_lock<mutex> childl(childm);
			req_t=0;
			if(req_table[process_num]==-1)
			{
				unique_lock<mutex> iol(iom);
				cout<<process_num<<" requests block num :";
				cin>>input;
				iol.unlock();					
				req_table[process_num]=input;
			}
			req_thread=process_num;
			sat=-1;
			response=-1;
			sleep(0.1);
			unique_lock<mutex> respl(respm);
			par.notify_one();
			req.wait(respl, []{return response !=-1;});		
			sat=response;
			temp=response_b;
			response=-1;
			response_b=NULL;
			childl.unlock();
		}
		if(sat<2)
		{
			if(sat==1)
			{	
				free_cv.wait(freel,[]{return free_NULL == 0;});
			}
			else
			{
				int temp1=req_table[process_num];
				hash_cv.wait(hashl,[&temp1]{if(Lock_table.find(temp1)==Lock_table.end())
									return true;
							   else
								return Lock_table[temp1] != 1;});
			}
			continue;
		}
		else
		{
			req_table[process_num]=-1;
			sleep(rand()%5+1);	//work
		}

		{
			unique_lock<mutex> childl(childm);
			req_t=1;
			req_thread=process_num;
			requ_b=temp;
			sleep(0.1);
			unique_lock<mutex> respl(respm);
			par.notify_one();
			req.wait(respl, []{return response !=-1;});	
			childl.unlock();			
			sleep(rand()%5+1);
		}
	}
}

int main()
{	
	// Creating hash table and Free list
	Buffer *Hash[num_hash],*free;
	init(Hash,free,num_hash,total_buffer,Lock_table);
	free_NULL=0;	

	//creating requester threads
	thread t[total_thread];
	int p_status[total_thread];
	int requested[total_thread];
	for( int i=0;i<total_thread;i++)
	{
		req_table.insert(pair<int,int>(i,-1));
		p_status[i]=-1;
		requested[i]=-1;
		t[i]=thread(child,i);
	}
	sleep(0.1);
	//main thread
	
	int temp;
	while(true)
	{	unique_lock<mutex> parl(parm);
		par.wait(parl,[]{ return req_thread != -1;});	
		cout<<endl;
		requested[req_thread]=req_table[req_thread];
		if(req_t==0)
		{
			{
				unique_lock<mutex> iol(iom);
				cout<<req_thread<<" is reqesting buffer :"<<req_table[req_thread]<<endl;
				iol.unlock();
			}
			response_b=getblk(Hash,free,req_table[req_thread],Lock_table);
			if(free==NULL)
			{
				free_NULL=1;
			}
			if(response_b==NULL)
			{
				if(Lock_table.find(req_table[req_thread])==Lock_table.end())
				{
					p_status[req_thread]=1;
					response=1;
				}
				else
				{
					p_status[req_thread]=0;
					response=0;
				}
			}
			else
			{
				p_status[req_thread]=2;
				response=2;
			}
		}
		else
		{
			{
				unique_lock<mutex> iol(iom);
				cout<<req_thread<<" is releasing buffer: "<<requ_b->getnum()<<endl;
				iol.unlock();			
			}	
			free=brlse(free,requ_b,Lock_table);
			free_NULL=0;
			p_status[req_thread]=-1;
			free_cv.notify_all();
			hash_cv.notify_all();
			response=0;
		}	
		temp=req_thread;
		req_thread=-1;
		sleep(0.1);
		req.notify_one();
		unique_lock<mutex> iol(iom);
		DisplayBuf(Hash,free);
		cout<<"Buffer status ( -1 for delayed write, 1 for locked and 0 for free) :"<<endl;
		for(map<int, int>::iterator it=Lock_table.begin();it!=Lock_table.end();it++)
		{
			cout<<"("<<it->first<<","<<it->second<<")  ";
		}
		cout<<endl;
		cout<<"Process status :"<<endl;
		for(int i=0;i<total_thread;i++)
		{
			cout<<"("<<i;
			if(p_status[i]==2)
				cout<<" Locked "<<requested[i];
			else
			{
				if(p_status[i]==1)
					cout<<" Waiting for Free";
				else
				{
					if(p_status[i]==0)
						cout<<" Waiting for "<<requested[i];
					else
						cout<<" Waiting to request";
				}
			}
			cout<<")  ";
		}
		cout<<endl<<endl<<endl;
		iol.unlock();
	}	
	
	return 1;
}
