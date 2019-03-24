#include<iostream>
#define total_buffer 20 

using namespace std;

class Buffer{
		public:
		int num;
		int status;		//0-free,1-locked,-1-delayed write

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

Buffer *createhash0()
{
	Buffer *End;
	End=NULL;
	cout<<"world ";
	for(int i=0;i<total_buffer/4;i++)
	{
		Buffer *temp=(Buffer *)malloc(sizeof(Buffer));
		temp->num=i*total_buffer/4;
		if(End==NULL)
		{
			End=temp;
		}
		else
		{
			temp->next=End->next;
			End->next=temp;
			End=temp;
		}
	}
	return End;
}


int main()
{
	cout<<"you ";
	Buffer *End0=NULL;
	End0=createhash0();
	Buffer *temp=(Buffer *)malloc(sizeof(Buffer));
	temp=End0;
	while(temp!=NULL)
	{
		cout<<temp->num<<endl;	
		temp=temp->next;
	}
	return 1;
}

/*
int main()
{
	cout<<"you ";
	Buffer End0;
	Buffer *End1=NULL;
	return 1;
}*/
