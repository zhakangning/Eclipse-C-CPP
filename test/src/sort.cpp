#include <stdio.h>
#include "define.h"

inline void printArray(int *a,int len)
{
	int i;
	for(i=0;i<len;i++){
		printf("%d ",a[i]);
	}
	printf("\n");
}

//快速排序
void quickSort(int *a,int start,int end)
{
	printArray(a, end-start+1);
	if(start >= end){
		return ;
	}

	int low = start;
	int high = end;
	int key = a[low];

	while(low < high){
		while(low < high && a[high] > key){
			high--;
		}
		a[low] = a[high];
		cout<<"low = "<<low<<" high = "<<high<<endl;
		printArray(a, 5);
		while(low < high && a[low] < key){
			low++;
		}
		a[high] = a[low];
		cout<<"low = "<<low<<" high = "<<high<<endl;
		printArray(a, 5);
	}
	a[low] = key;
	printArray(a, 5);
	quickSort(a, start, low-1);
	quickSort(a, low+1, end);
}

inline void swap(int &n1, int &n2)
{
	int temp = n1;
	n1 = n2;
	n2 = temp;
}

void quickSort(int *a, int len)
{
	printArray(a, len);
	if(len < 2){
		return ;
	}
	int iLeft = 0;
	int iRight = len - 1;
	int keyVal = a[0];
	while(iLeft < iRight){
		while(iLeft < iRight && keyVal < a[iRight]){
			iRight--;
		}
		swap(a[iLeft], a[iRight]);
		cout<<"iLeft = "<<iLeft<<" iRight = "<<iRight<<endl;
		printArray(a, len);
		while(iLeft < iRight && keyVal > a[iLeft]){
			iLeft++;
		}
		swap(a[iLeft], a[iRight]);
		cout<<"iLeft = "<<iLeft<<" iRight = "<<iRight<<endl;
		printArray(a, len);
	}

	quickSort(a, iLeft);
	quickSort(a + iLeft + 1, len - iLeft -1);
}


void insertSort(int *a, int len)
{//直接插入排序
	int i,j,key;
	for(i=1;i<len;i++){
		key=a[i];
		j=i-1;
		while(key<a[j]&&j>=0){
			a[j+1]=a[j];
			j--;
		}
		a[j+1]=key;
	}
}

void selectSort(int *a,int len)
{//直接选择排序
	int i,j,index;
	for(i=0;i<len;i++){
		index=i;
		//找到最小元素的下标
		for(j=i+1;j<len;j++){
			if(a[j]<a[index])
				index=j;
		}
		if(index!=i){//交换位置
			a[index]=a[index]+a[i];
			a[i]=a[index]-a[i];
			a[index]=a[index]-a[i];
		}
	}
}
