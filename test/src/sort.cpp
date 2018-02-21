#include <stdio.h>
#include "define.h"

void printArray(int *a,int len)
{
	int i;
	for(i=0;i<len;i++){
		printf("%d ",a[i]);
	}
	printf("\n");
}

//快速排序
void quickSort1(int *a,int start,int end)
{
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
		while(low < high && a[low] < key){
			low++;
		}
		a[high] = a[low];
	}
	a[low] = key;
	quickSort1(a, start, low-1);
	quickSort1(a, low+1, end);
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
