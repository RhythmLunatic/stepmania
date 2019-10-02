#ifndef _QUEUE_h
#define _QUEUE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

template<typename T>
class CircularQueue {
	
private:
	int Size;
    int Count;
    int Front;
    int Rear;
	
	
	bool UseDynamicMemory;
	T Default;
	
	T* BufferPtr = NULL;
	
public:
	CircularQueue (T* SetStorageArray,int SizeSet)
	{		
		Size = SizeSet;
		Front=0;
		Rear=0;
		Count=0;
		Default=0;
		
		BufferPtr = SetStorageArray;
		UseDynamicMemory = false;
	}
	
	CircularQueue (int SizeSet)
	{
		Size = SizeSet;
		Front=0;
		Rear=0;
		Count=0;
		Default=0;
		
		BufferPtr = new T[Size]{0};
		UseDynamicMemory = true;
	}
	
	~CircularQueue ()
	{
		if (UseDynamicMemory)
		{
			delete[]BufferPtr;
			BufferPtr = NULL;
		}
	}
	
	
	void Enqueue (const T i)
	{
		return Push(i);
	}
	
	void Push (const T i)
	{
		if (isFull())
			return;
		else
		{
			BufferPtr[Rear++] = i;
			if (Rear >= Size) 
				Rear = 0;
		}
	}
	
	T Dequeue ()
	{
		return Pop();
	}
	
	T Pop()
	{
		if(isEmpty())
			return Default;
		else
		{
			T Value = BufferPtr[Front++];
		
			if(Front>=Size)
				Front=0;
			
			return Value;
		}
	}
	
	T Peek ()
	{
		if (isEmpty ())
			return Default;
		else
			return BufferPtr[Front];
	}
	
	int IndexOf(T* Target, int TargetLength)
	{
		if(GetCount()>=TargetLength)
		{
			for	(int SearchCount=0;SearchCount<=GetCount()-TargetLength;SearchCount++)
			{
				int SearchStart = SearchCount+Front;
				if(SearchStart>=Size)
					SearchStart-=Size;
				
				for (int TargetIndex=0;TargetIndex<TargetLength;TargetIndex++)
				{
					int Index = SearchStart+TargetIndex;
					if(Index>=Size)
						Index-=Size;
					
					if(BufferPtr[Index]!=Target[TargetIndex])
					{
						break;//Not match
					}
					else if (TargetIndex+1>=TargetLength)
					{
						//All target match
						return SearchCount;
					}					
					
				}			
				
			}
		}
		
		return -1;//NotFound
	}
	
	bool isEmpty ()
	{
		return Front == Rear;
	}
	
	int GetCount()
	{
		if(Rear>=Front)		
			return Rear-Front;
		else
			return Rear+Size-Front;
	}
	
	bool isFull ()
	{
		if(Rear>Front)		
			return (Rear+1-Size) == Front;
		else
			return (Rear+1)==Front;
	}
	
	int GetEmptyCount()
	{
		if(isFull ())
			return 0;
		else
			return (Size-1) - GetCount();
	}
	
	void Clear()
	{
		Front=0;
		Rear=0;
	}
	
	
};






#endif