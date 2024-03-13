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
} cmpFunction;
int execute(cmpFunction af) {
    return af.function(af.valueA, af.valueB);
}

Page* initPage(int size) {
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

void dumpPagesFrom(Page* fromPage, char* (*format)(void*) ) {
	int pageIndex = 0; 
	do {
		if (fromPage->value != NULL) {
			printf("page %d: %s\n", pageIndex, format(fromPage->value));
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
			cmpFunction cf = {
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
			cmpFunction cf = {
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

typedef struct LRU LRU;
struct LRU {
	Page *Pages;
	int (*cmpFunction)(void*, void*);
	char* (*fmtFunction)(void*);
};

Page* read(LRU *lru, void* value) {
	return get(&lru->Pages, value, lru->cmpFunction);
}

void cache(LRU *lru, void* value) {
	add(&lru->Pages, value, lru->cmpFunction);
}

void dump(LRU *lru) {
	dumpPagesFrom(lru->Pages, lru->fmtFunction);
}

LRU* initLRU(int size, int(*cmpFunction)(void*, void*), char*(*fmtFunction)(void*)) {
	if (size < 1) {
		return NULL;
	}

	LRU *lru = malloc(sizeof(LRU));
	lru->Pages = initPage(size);
	lru->cmpFunction = cmpFunction;
	lru->fmtFunction = fmtFunction;

	return lru;
}


int compInt(void* valueA, void* valueB) {
	return *(int *)valueA == *(int *)valueB;
}

char* formatInt(void* value) {
	char* buff = malloc(255*sizeof(buff));
	sprintf( buff, "%d", *(int*)value);
	return buff;
}

int main() {
   printf("Hello, World!\n");
   Page* pages = initPage(5);

   dumpPagesFrom(pages, formatInt);
   int valuesToCache[] = {1, 2, 3, 2, 3, 4, 5, 4, 1, 3, 7, 6};

   int length = sizeof(valuesToCache) / sizeof(valuesToCache[0]);
   for (int i = 0; i < length; i++) {
	int toCache = valuesToCache[i];
	printf("-- add %d\n", toCache);
	add(&pages, &toCache, compInt);
	dumpPagesFrom(pages, formatInt);
   }
   
   int valuesToRefresh[] = {4, 1, 1};
   length = sizeof(valuesToRefresh) / sizeof(valuesToRefresh[0]);
   for (int i = 0; i < length; i++) {
	int toRefresh = valuesToRefresh[i];
   	printf("-- refresh %d\n", toRefresh);
	Page *rr = get(&pages, &toRefresh, compInt);
   	dumpPagesFrom(pages, formatInt);
   }

   int valA = 3;
   int valB = 3;
   cmpFunction cf = {
	compInt,
	&valA,
	&valB
   };
   int test = execute(cf);
   printf("-- test cmp: %d\n", test);

   printf("\n\n- test LRU\n\n");
   LRU *lru = initLRU(5, compInt, formatInt);
   length = sizeof(valuesToCache) / sizeof(valuesToCache[0]);
   for (int i = 0; i < length; i++) {
	int toCache = valuesToCache[i];
	printf("-- cache %d\n", toCache);
	cache(lru, &toCache);
	dump(lru);
   }

   length = sizeof(valuesToRefresh) / sizeof(valuesToRefresh[0]);
   for (int i = 0; i < length; i++) {
	int toRead = valuesToRefresh[i];
   	printf("-- read %d\n", toRead);
	read(lru, &toRead);
	dump(lru);
   }
   return 0;
}
