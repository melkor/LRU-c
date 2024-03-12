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
	Page *pageHasValue = NULL;
	Page *currentPage = firstPage;
	int i = 0;
	do {
		if (currentPage->value != NULL && *(currentPage->value) == value) {
			pageHasValue = currentPage;
		}
		currentPage = currentPage->next;
	} while (pageHasValue == NULL && currentPage->next != NULL);	
	return pageHasValue;
}

Page* add(Page* firstPage, int value) {
	Page *currentPage = firstPage;
	Page *previousPage = NULL;
	Page *pageToReturn = NULL;
	while (currentPage->next != NULL) {
		if (currentPage->value == NULL) {
			Page* newPage = malloc(sizeof(Page));
			newPage->value = malloc(sizeof(int));
			*(newPage->value) = value;
			newPage->next = firstPage;
			pageToReturn = newPage;
		} else if (*currentPage->value == value) {
			if (previousPage) {
				previousPage->next = currentPage->next;
				currentPage->next = firstPage;
			}
			pageToReturn = currentPage;
			return pageToReturn;
		} 
		previousPage = currentPage;
		currentPage = currentPage->next;
	}

	previousPage->next = NULL;
	free(currentPage);

	if (!pageToReturn) {
		pageToReturn = malloc(sizeof(Page));
		*(pageToReturn->value) = value;
		pageToReturn->next = firstPage;
	}

	return pageToReturn;
}

int main() {
   printf("Hello, World!\n");
   Page* lru = initLRU(5);
   DumpLRUFrom(lru);
   Page* test = get(lru, 1);
   if (test) {
	printf("page found\n");
   } else {
	printf("1 is not presents into LRU\n");
   }
   printf("-- add 1\n");
   lru = add(lru, 1);
   DumpLRUFrom(lru);
   
   printf("-- add 2\n");
   lru = add(lru, 2);
   DumpLRUFrom(lru);

   printf("-- add 3\n");
   lru = add(lru, 3);
   DumpLRUFrom(lru);

   lru = add(lru, 2);
   printf("-- add 2\n");
   DumpLRUFrom(lru);

   printf("-- add 3\n");
   lru = add(lru, 3);
   DumpLRUFrom(lru);

   printf("-- add 4\n");
   lru = add(lru, 4);
   DumpLRUFrom(lru);

   printf("-- add 5\n");
   lru = add(lru, 5);
   DumpLRUFrom(lru);

   printf("-- add 4\n");
   lru = add(lru, 4);
   DumpLRUFrom(lru);

   printf("-- add 6\n");
   lru = add(lru, 6);
   DumpLRUFrom(lru);
   return 0;
}
