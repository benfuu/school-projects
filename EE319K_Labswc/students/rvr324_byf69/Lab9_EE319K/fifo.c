//fifo.c
unsigned long static PutI;	//0 to 8, points to empty location to put data 
unsigned long static GetI; //0 to 8, points to the oldest data
long static FIFO[9];				//initialize an 8 element array for FIFO
void Fifo_Init(void){
	PutI = GetI = 8;				//empty the queue
}
int	Fifo_Put(char data){
	if(PutI==(GetI+1)%9) return 0; //if the PutI conflicts with GetI location, FIFO is full, so return a failure
	FIFO[PutI] = data;			//not full, so put the data into the address of PutI
	if(PutI==0)	PutI = 8;		//wrap PutI if PutI<0
	PutI = (PutI-1);			//if not, just decrement PutI
	return	1;								//return a success
}
int Fifo_Get(long *point){
	if(PutI == GetI) return 0;			//check for empty buffer
	*point = FIFO[GetI];			//get the data from GetI location
	if(GetI==0)	GetI = 8;		//wrap GetI if GetI<0
	GetI = GetI-1;		//decrement the GetI pointer
	return	1;					//return success
}
