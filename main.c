#include <stdio.h>
#include <stdlib.h>

typedef struct Page Page;
struct Page {
	int *value;
	Page *next;
};

Page* initLRU(int size) {
	Page *firstPage = malloc(sizeof(Page));
	Page *currentPage = firstPage;
	for (int i = 1; i <= size; i++) {
		Page *nextPage = malloc(sizeof(Page));
		currentPage->next = nextPage;
		currentPage = nextPage;
	}

	return firstPage;
}

void DumpLRUFrom(Page* fromPage) {
	int pageIndex = 0; 
	do {
		if (fromPage->value != NULL) {
			printf("page %d: %d\n", pageIndex, *fromPage->value);
		} else {
			printf("page %d: no value\n", pageIndex);
		}
		pageIndex++;
		fromPage = fromPage->next;
	} while (fromPage->next != NULL);
}

Page* get(Page* firstPage, int value) {
	Page *pageHasValue;
	Page *currentPage = firstPage;
	do {
		if (currentPage->value != NULL && *(currentPage->value) == value) {
			pageHasValue = currentPage;
		}
		currentPage = currentPage->next;
	} while (pageHasValue == NULL || currentPage->next != NULL);	
	return pageHasValue;
}

int main() {
   printf("Hello, World!\n");
   Page* lru = initLRU(5);
   DumpLRUFrom(lru);
   Page* test = get(lru, 1);
   if (!test) {
	printf("1 is not presents into LRU\n");
   } else {
	printf("page found\n");
   }
   return 0;
}
