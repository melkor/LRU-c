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

	Page *firstPage = malloc(sizeof(*firstPage));
	firstPage->value = NULL;
	firstPage->next = NULL;

	Page *currentPage = firstPage;
	for (int i = 1; i < size; i++) {
		Page *nextPage = malloc(sizeof(*nextPage));
		nextPage->value = NULL;
		nextPage->next = NULL;
		currentPage->next = nextPage;
		currentPage = nextPage;
	}

	return firstPage;
}

void destroy(Page* pages) {
	while (pages->next) {
		Page *previousPage = pages;
		if (pages->next) {
			Page *currentPage = pages->next;
			while (currentPage->next) {
				previousPage = currentPage;
				currentPage = currentPage->next;
			}
			previousPage->next = NULL;
			if (currentPage->value) {
				free(currentPage->value);
			}
			free(currentPage);
		}
	}
	if (pages->value) {
		free(pages->value);
	}
	free(pages);
}

void dumpPagesFrom(Page* fromPage, char* (*format)(void*) ) {
	int pageIndex = 0; 
	do {
		if (fromPage->value != NULL) {
			char *toPrint = format(fromPage->value);
			printf("page %d: %s\n", pageIndex, toPrint);
			free(toPrint);
		} else {
			printf("page %d: no value\n", pageIndex);
		}
		pageIndex++;
		fromPage = fromPage->next;
	} while (fromPage);
}

void add(Page** firstPage, size_t size, void* value, int (*cmpFunc)(void*, void*)) {
	Page *currentPage = *firstPage;
	Page *previousPage = NULL;

	while (currentPage) {
		if (currentPage->value == NULL || currentPage->next == NULL) {
			Page* newPage = malloc(sizeof(*newPage));
			newPage->value = malloc(size);
			memcpy(newPage->value, value, size);
			newPage->next = *firstPage;

			*firstPage = newPage;
			while (currentPage && currentPage->next != NULL) {
				previousPage = currentPage;
				currentPage = currentPage->next;
			}
			previousPage->next = NULL;
			if (currentPage->value) {
				free(currentPage->value);
			}
			free(currentPage);
			break;
		} else if (currentPage->value != NULL) {
			cmpFunction cf = {
				cmpFunc,
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

Page* get(Page** firstPage, void* value, int (*cmpFunc)(void*, void*)) {
	if (cmpFunc == NULL) {
		return NULL;
	}
	
	Page *pageHasValue = NULL;
	Page *previousPage = NULL;
	Page *currentPage = *firstPage;
	do {
		if (currentPage->value != NULL) {
			cmpFunction cf = {
				cmpFunc,
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
	size_t valueSize;
	int (*cmpFunction)(void*, void*);
	char* (*fmtFunction)(void*);
};

Page* read(LRU *lru, void* value) {
	return get(&lru->Pages, value, lru->cmpFunction);
}

void cache(LRU *lru, void* value) {
	add(&lru->Pages, lru->valueSize, value, lru->cmpFunction);
}

void dump(LRU *lru) {
	dumpPagesFrom(lru->Pages, lru->fmtFunction);
}

void clear(LRU *lru) {
	destroy(lru->Pages);
	free(lru);
}

LRU* initLRU(int cacheSize, size_t valueSize, int(*cmpFunction)(void*, void*), char*(*fmtFunction)(void*)) {
	if (cacheSize < 1) {
		return NULL;
	}

	LRU *lru = malloc(sizeof(*lru));
	lru->Pages = initPage(cacheSize);
	lru->cmpFunction = cmpFunction;
	lru->fmtFunction = fmtFunction;
	lru->valueSize = valueSize;

	return lru;
}


int compInt(void* valueA, void* valueB) {
	return *(int *)valueA == *(int *)valueB;
}

char* formatInt(void* value) {
	if (value == NULL) {
		return "value not initialized";
	}
	
	char* buff = malloc(255*sizeof(*buff));
	sprintf(buff, "%d", *(int*)value);
	return buff;
}

int compStr(void* valueA, void* valueB) {
	return strcmp((char *)valueA, (char *)valueB) == 0;
}

char* formatStr(void* value) {
	if (value == NULL) {
		return "value not initialized";
	}
	
	char* buff = malloc(255*sizeof(*buff));
	memcpy(buff, value, 255*sizeof(*buff)); 
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
	add(&pages, sizeof(toCache), &toCache, compInt);
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
   destroy(pages);

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
   LRU *lru = initLRU(5, sizeof(int), compInt, formatInt);
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

   clear(lru);

   printf("\n\n- test LRU of char[]\n\n");
   LRU *lruStr = initLRU(5, 255*sizeof(char), compStr, formatStr);

   char testString[] = "coucou";
   printf("\n-- cache %s\n", testString);
   cache(lruStr, testString);
   dump(lruStr);

   char testString2[] = "guigui";
   printf("\n-- cache %s\n", testString2);
   cache(lruStr, testString2);
   dump(lruStr);

   char testString3[] = "tavu";
   printf("\n-- cache %s\n", testString3);
   cache(lruStr, testString3);
   dump(lruStr);

   printf("\n-- read %s\n", testString);
   read(lruStr, testString);
   dump(lruStr);

   clear(lruStr);
   return 0;
}
