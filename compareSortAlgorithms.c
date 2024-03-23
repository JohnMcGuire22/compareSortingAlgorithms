#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

int extraMemoryAllocated;

void* Alloc(size_t sz)
{
	extraMemoryAllocated += sz;
	size_t* ret = malloc(sizeof(size_t) + sz);
	*ret = sz;
	printf("Extra memory allocated, size: %ld\n", sz);
	return &ret[1];
}

void DeAlloc(void* ptr)
{
	size_t* pSz = (size_t*)ptr - 1;
	extraMemoryAllocated -= *pSz;
	printf("Extra memory deallocated, size: %ld\n", *pSz);
	free((size_t*)ptr - 1);
}

size_t Size(void* ptr)
{
	return ((size_t*)ptr)[-1];
}

void heapify(int pData[], int n, int i) {
	// Find largest
	int largest = i;
	int left = 2 * i + 1;
	int right = 2 * i + 2;
	int temp;

	if (left < n && pData[left] > pData[largest])
		largest = left;

	if (right < n && pData[right] > pData[largest])
		largest = right;

	if (largest != i) {
		temp = pData[i];
		pData[i] = pData[largest];
		pData[largest] = temp;
		heapify(pData, n, largest);
	}
}

// implements heap sort
// extraMemoryAllocated counts bytes of memory allocated
void heapSort(int pData[], int n)
{
	int temp;
	for (int i = n / 2 - 1; i >= 0; i--)
		heapify(pData, n, i);

	for (int i = n - 1; i >= 0; i--) {
		temp = pData[0];
		pData[0] = pData[i];
		pData[i] = temp;
		heapify(pData, i, 0);
	}
}

void merge(int pData[], int l, int mid, int r) {
	int bot = mid - l + 1; //idk why i named them bot and top tbh
	int top = r - mid;

	int* temp1 = (int*)Alloc(sizeof(int) * bot);
	int* temp2 = (int*)Alloc(sizeof(int) * top);

	for (int i = 0; i < bot; i++) //fill temp arrays
		temp1[i] = pData[l + i];
	for (int j = 0; j < top; j++)
		temp2[j] = pData[mid + 1 + j];

	int i = 0, j = 0, k = l;
	while (i < bot && j < top) {
		if (temp1[i] <= temp2[j]) {
			pData[k] = temp1[i]; //lower number goes into original array, i increments
			i++;
		}
		else {
			pData[k] = temp2[j]; //lower number goes into original array, j increments
			j++;
		}
		k++; //next element of original array
	}

	while (i < bot) { //remaining elements pasted on the back end
		pData[k] = temp1[i];
		k++;
		i++;
	}
	while (j < top) { // ^above comment
		pData[k] = temp2[j];
		k++;
		j++;
	}

	DeAlloc(temp1);
	DeAlloc(temp2);
}

// implement merge sort
// extraMemoryAllocated counts bytes of extra memory allocated
void mergeSort(int pData[], int l, int r)
{
	if (l < r) {
		int mid = (l + r) / 2;
		mergeSort(pData, l, mid);
		mergeSort(pData, mid + 1, r);
		merge(pData, l, mid, r);
	}
}

// implement insertion sort
// extraMemoryAllocated counts bytes of memory allocated
void insertionSort(int* pData, int n)
{
	for (int i = 1; i < n; i++) {
		int move = pData[i];
		int j = i - 1;

		while (move < pData[j] && j >= 0) {
			pData[j + 1] = pData[j];
			j--;
		}
		pData[j + 1] = move;
	}
}

// implement bubble sort
// extraMemoryAllocated counts bytes of extra memory allocated
void bubbleSort(int* pData, int n)
{
	int i = 0;
	int temp;
	for (i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (pData[i] < pData[j]) {
				temp = pData[i];
				pData[i] = pData[j];
				pData[j] = temp;
			}
		}
	}
}

// implement selection sort
// extraMemoryAllocated counts bytes of extra memory allocated
void selectionSort(int* pData, int n)
{
	int temp;
	int lowest;
	for (int i = 0; i < n; i++) {
		lowest = i;
		for (int j = i; j < n; j++) {
			if (pData[lowest] > pData[j])
				lowest = j;
			if (j == n && lowest != i) {
				temp = pData[i];
				pData[i] = pData[lowest];
				pData[lowest] = temp;
			}
		}
	}
}

// parses input file to an integer array
int parseData(char* inputFileName, int** ppData)
{
	FILE* inFile = fopen(inputFileName, "r");
	int dataSz = 0;
	int i, n, * data;
	*ppData = NULL;

	if (inFile)
	{
		fscanf(inFile, "%d\n", &dataSz);
		*ppData = (int*)Alloc(sizeof(int) * dataSz);
		// Implement parse data block
		if (*ppData == NULL)
		{
			printf("Cannot allocate memory\n");
			exit(-1);
		}
		for (i = 0; i < dataSz; ++i)
		{
			fscanf(inFile, "%d ", &n);
			data = *ppData + i;
			*data = n;
		}
		fclose(inFile);
	}

	return dataSz;
}

// prints first and last 100 items in the data array
void printArray(int pData[], int dataSz)
{
	int i, sz = dataSz - 100;
	printf("\tData:\n\t");
	for (i = 0; i < 100; ++i)
	{
		printf("%d ", pData[i]);
	}
	printf("\n\t");

	for (i = sz; i < dataSz; ++i)
	{
		printf("%d ", pData[i]);
	}
	printf("\n\n");
}

int main(void)
{
	clock_t start, end;
	int i;
	double cpu_time_used;
	char* fileNames[] = { "input1.txt", "input2.txt", "input3.txt" };

	for (i = 0; i < 3; ++i)
	{
		int* pDataSrc, * pDataCopy;
		int dataSz = parseData(fileNames[i], &pDataSrc);

		if (dataSz <= 0)
			continue;

		pDataCopy = (int*)Alloc(sizeof(int) * dataSz);

		printf("---------------------------\n");
		printf("Dataset Size : %d\n", dataSz);
		printf("---------------------------\n");

		printf("Selection Sort:\n");
		memcpy(pDataCopy, pDataSrc, dataSz * sizeof(int));
		extraMemoryAllocated = 0;
		start = clock();
		selectionSort(pDataCopy, dataSz);
		end = clock();
		cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
		printf("\truntime\t\t\t: %.1lf\n", cpu_time_used);
		printf("\textra memory allocated\t: %d\n", extraMemoryAllocated);
		printArray(pDataCopy, dataSz);

		printf("Insertion Sort:\n");
		memcpy(pDataCopy, pDataSrc, dataSz * sizeof(int));
		extraMemoryAllocated = 0;
		start = clock();
		insertionSort(pDataCopy, dataSz);
		end = clock();
		cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
		printf("\truntime\t\t\t: %.1lf\n", cpu_time_used);
		printf("\textra memory allocated\t: %d\n", extraMemoryAllocated);
		printArray(pDataCopy, dataSz);

		printf("Bubble Sort:\n");
		memcpy(pDataCopy, pDataSrc, dataSz * sizeof(int));
		extraMemoryAllocated = 0;
		start = clock();
		bubbleSort(pDataCopy, dataSz);
		end = clock();
		cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
		printf("\truntime\t\t\t: %.1lf\n", cpu_time_used);
		printf("\textra memory allocated\t: %d\n", extraMemoryAllocated);
		printArray(pDataCopy, dataSz);

		printf("Merge Sort:\n");
		memcpy(pDataCopy, pDataSrc, dataSz * sizeof(int));
		extraMemoryAllocated = 0;
		start = clock();
		mergeSort(pDataCopy, 0, dataSz - 1);
		end = clock();
		cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
		printf("\truntime\t\t\t: %.1lf\n", cpu_time_used);
		printf("\textra memory allocated\t: %d\n", extraMemoryAllocated);
		printArray(pDataCopy, dataSz);

		printf("Heap Sort:\n");
		memcpy(pDataCopy, pDataSrc, dataSz * sizeof(int));
		extraMemoryAllocated = 0;
		start = clock();
		heapSort(pDataCopy, dataSz - 1);
		end = clock();
		cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
		printf("\truntime\t\t\t: %.1lf\n", cpu_time_used);
		printf("\textra memory allocated\t: %d\n", extraMemoryAllocated);
		printArray(pDataCopy, dataSz);

		DeAlloc(pDataCopy);
		DeAlloc(pDataSrc);
	}

}
