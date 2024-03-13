#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Page Page;
struct Page {
	void *value;
	Page *next;
};

typedef struct {
    int (*function)(void*, void*);
    void* valueA;
    void* valueB;
} compFunction;
int execute(compFunction af) {
    return af.function(af.valueA, af.valueB);
}

Page* initLRU(int size) {
	if (size < 1) {
		return NULL;
	}

	Page *firstPage = malloc(sizeof(firstPage));
	firstPage->value = NULL;
	firstPage->next = NULL;

	Page *currentPage = firstPage;
	for (int i = 1; i < size; i++) {
		Page *nextPage = malloc(sizeof(nextPage));
		currentPage->next = nextPage;
		currentPage = nextPage;
	}

	return firstPage;
}

void DumpLRUFrom(Page* fromPage) {
	int pageIndex = 0; 
	do {
		if (fromPage->value != NULL) {
			printf("page %d: %d\n", pageIndex, *(int*)fromPage->value);
		} else {
			printf("page %d: no value\n", pageIndex);
		}
		pageIndex++;
		fromPage = fromPage->next;
	} while (fromPage);
}

void add(Page** firstPage, void* value, int (*compInt)(void*, void*)) {
	Page *currentPage = *firstPage;
	Page *previousPage = NULL;

	while (currentPage) {
		if (currentPage->value == NULL || currentPage->next == NULL) {
			Page* newPage = malloc(sizeof(newPage));
			newPage->value = malloc(sizeof(value));
			memcpy(newPage->value, value, sizeof(value));
			newPage->next = *firstPage;

			*firstPage = newPage;
			while (currentPage && currentPage->next != NULL) {
				previousPage = currentPage;
				currentPage = currentPage->next;
			}
			previousPage->next = NULL;
			free(currentPage);
			break;
		} else if (currentPage->value != NULL) {
			compFunction cf = {
				compInt,
				currentPage->value,
				value
			};
			if (execute(cf)) {
				if (previousPage) {
					previousPage->next = currentPage->next;
					currentPage->next = *firstPage;
				}
				*firstPage = currentPage;
				break;
			}
		} 
		previousPage = currentPage;
		currentPage = currentPage->next;
	}
}

Page* get(Page** firstPage, void* value, int (*compInt)(void*, void*)) {
	Page *pageHasValue = NULL;
	Page *previousPage = NULL;

	Page *currentPage = malloc(sizeof(currentPage));
	currentPage = *firstPage;
	do {
		if (currentPage->value != NULL) {
			compFunction cf = {
				compInt,
				currentPage->value,
				value
			};
			if (execute(cf)) {
				if (previousPage) {
					previousPage->next = currentPage->next;
					currentPage->next = *firstPage;

					*firstPage = currentPage;
				}
				pageHasValue = *firstPage;
				break;
			}
		}
		previousPage = currentPage;
		currentPage = currentPage->next;
	} while (pageHasValue == NULL && currentPage != NULL);	
	return pageHasValue;
}

int compInt(void* valueA, void* valueB) {
	return *(int *)valueA == *(int *)valueB;
}

int main() {
   printf("Hello, World!\n");
   Page* lru = initLRU(5);

   DumpLRUFrom(lru);
   int valuesToCache[] = {1, 2, 3, 2, 3, 4, 5, 4, 1, 3, 7, 6};

   int length = sizeof(valuesToCache) / sizeof(valuesToCache[0]);
   for (int i = 0; i < length; i++) {
	int toCache = valuesToCache[i];
	printf("-- add %d\n", toCache);
	add(&lru, &toCache, compInt);
	DumpLRUFrom(lru);
   }
   
   int valuesToRefresh[] = {4, 1, 1};
   length = sizeof(valuesToRefresh) / sizeof(valuesToRefresh[0]);
   for (int i = 0; i < length; i++) {
	int toRefresh = valuesToRefresh[i];
   	printf("-- refresh %d\n", toRefresh);
	Page *rr = get(&lru, &toRefresh, compInt);
   	DumpLRUFrom(lru);
   }

   int valA = 3;
   int valB = 3;
   compFunction cf = {
	compInt,
	&valA,
	&valB
   };
   int test = execute(cf);
   printf("-- test cmp: %d\n", test);


   return 0;
}
