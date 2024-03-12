#include <stdio.h>
#include <stdlib.h>

typedef struct Page Page;
struct Page {
	int *value;
	Page *next;
};

Page* initLRU(int size) {
	if (size < 1) {
		return NULL;
	}

	Page *firstPage = malloc(sizeof(Page));
	firstPage->value = NULL;
	firstPage->next = NULL;

	Page *currentPage = firstPage;
	for (int i = 1; i < size; i++) {
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
	} while (fromPage);
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
		pageToReturn->value = malloc(sizeof(int));
		*(pageToReturn->value) = value;
		pageToReturn->next = firstPage;
	}

	return pageToReturn;
}

Page* get(Page** firstPage, int value) {
	Page *pageHasValue = NULL;
	Page *previousPage = NULL;

	Page *currentPage = malloc(sizeof(Page));
	currentPage = *firstPage;
	do {
		if (currentPage->value != NULL && *(currentPage->value) == value) {
			previousPage->next = currentPage->next;
			currentPage->next = *firstPage;

			*firstPage = currentPage;
			pageHasValue = *firstPage;
			break;
		}
		previousPage = currentPage;
		currentPage = currentPage->next;
	} while (pageHasValue == NULL && currentPage != NULL);	
	return pageHasValue;
}

int main() {
   printf("Hello, World!\n");
   Page* lru = initLRU(5);

   DumpLRUFrom(lru);

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

   printf("-- add 1\n");
   lru = add(lru, 1);
   DumpLRUFrom(lru);

   printf("-- add 3\n");
   lru = add(lru, 3);
   DumpLRUFrom(lru);

   printf("-- add 6\n");
   lru = add(lru, 6);
   DumpLRUFrom(lru);

   printf("-- add 7\n");
   lru = add(lru, 7);
   DumpLRUFrom(lru);

   printf("-- add 6\n");
   lru = add(lru, 6);
   DumpLRUFrom(lru);

   printf("-- test refresh 4\n");
   Page *rr = get(&lru, 4);
   if (rr) {
   	printf("-- ok 4\n");
   }
   DumpLRUFrom(lru);

   printf("-- test refresh 1\n");
   if (get(&lru, 1)) {
   	printf("-- ok 1\n");
   }
   DumpLRUFrom(lru);

   return 0;
}
