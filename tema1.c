/* Ciuciu Ioan Dionisie 333 CA */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"hash.h"

struct List {
	char *word;
	struct List *next;
	int wordNumber;
};

struct Hashtable {
	struct List **hashTable;
	unsigned int size;
};

/* Creates a hashtable */
struct Hashtable *create(int size)
{
	int i = 0;
	struct Hashtable *table;

	if (size < 1)
		return NULL;

	table = (struct Hashtable *)malloc(sizeof(struct Hashtable));
	if (table == NULL)
		return NULL;

	table->hashTable = (struct List **)malloc(sizeof(struct List *) * size);
	if (table->hashTable == NULL)
		return NULL;

	for (i = 0; i < size; ++i)
		table->hashTable[i] = NULL;

	table->size = size;
	return table;
}

/* Opens a new file and writes a string in it*/
int writeToFile(FILE *output, char *file, const char *word)
{
	output = fopen(file, "a");
	if (output == NULL) {
		fprintf(stderr, "%s", "Error oppening file!\n");
		return -1;
	}

	fprintf(output, "%s", word);
	fclose(output);
}

/* Search for a word in the Hashtable. */
struct List *findWord(struct Hashtable *table, const char *word)
{
	struct List *list;
	unsigned int hashValue = hash(word, table->size);

	for (list = table->hashTable[hashValue]; list != NULL;
	     list = list->next) {
		if (strcmp(word, list->word) == 0)
			return list;
	}
	return NULL;
}

/* Check if the word was found */
int find(struct Hashtable *table, const char *word)
{
	if (findWord(table, word) != NULL)
		return 1;
	return 0;
}

/* Removes a word from the Hashtable */
void removeWord(struct Hashtable *table, const char *word)
{
	unsigned int hashValue = hash(word, table->size);
	struct List *current = NULL;
	struct List *previous = NULL;

	current = table->hashTable[hashValue];

	for (; current != NULL; current = current->next) {
		if (strcmp(current->word, word) == 0) {
			if (previous == NULL)
				table->hashTable[hashValue] = current->next;
			else
				previous->next = current->next;
			free(current->word);
			free(current);
			current = NULL;
			break;
		}
		previous = current;
	}
}
/* Clears the elements of the hashtable */
int clear(struct Hashtable *table)
{
	unsigned int i;
	struct List *current = NULL;
	struct List *temp = NULL;

	for (i = 0; i < table->size; ++i) {
		current = table->hashTable[i];
		while (current != NULL) {
			table->hashTable[i] = table->hashTable[i]->next;
			free(current->word);
			free(current);
			current = table->hashTable[i];
		}
	}
}

/* Adds a word in the Hashtable */
int add(struct Hashtable *table, const char *word)
{
	unsigned int hashValue = hash(word, table->size);
	struct List *current = NULL, *newNode = NULL, *found = NULL;

	newNode = malloc(sizeof(struct List));
	newNode->word = malloc(strlen(word) * sizeof(char *));
	found = findWord(table, word);
	if (found == NULL) {
		newNode->next = NULL;
		strcpy(newNode->word, word);
		current = table->hashTable[hashValue];

		if (current == NULL) {
			current = newNode;
			table->hashTable[hashValue] = current;
		} else {
			while (current->next != NULL)
				current = current->next;
			current->next = newNode;
		}
	}
}

/* Prints a bucket from a specified index, at stdout */
void printBucket(struct Hashtable *table, int index)
{
	struct List *current = NULL;

	current = table->hashTable[index];

	if (table->hashTable[index] != NULL)
		for (; current != NULL; current = current->next) {
			if (current->next != NULL)
				printf("%s ", current->word);
			else
				printf("%s", current->word);
		}
}

/* Prints a bucket from a specified index, in an output file */
void printBucketInFile(struct Hashtable *table, int index, FILE *output,
		       char *file)
{
	struct List *current = NULL;

	current = table->hashTable[index];

	if (table->hashTable[index] != NULL)
		for (; current != NULL; current = current->next)
			if (current->next != NULL) {
				writeToFile(output, file, current->word);
				writeToFile(output, file, " ");
			} else
				writeToFile(output, file, current->word);
	writeToFile(output, file, "\n");
}

/* Verifies if a bucket is empty (has no words) */
int checkEmptyBucket(struct Hashtable *table, int index)
{
	if (table->hashTable[index] == NULL)
		return 1;
	return 0;
}

/* Prints the Hashtable at stdout */
void print(struct Hashtable *table)
{
	struct List *list;
	unsigned int i;

	for (i = 0; i < table->size; ++i)
		if (checkEmptyBucket(table, i) == 0) {
			list = table->hashTable[i];
			for (; list != NULL; list = list->next) {
				if (list->next != NULL)
					printf("%s ", list->word);
				else
					printf("%s\n", list->word);
			}
		}
}

/* Prints the Hashtable in an output file */
void printInFile(struct Hashtable *table, FILE *output, char *file)
{
	struct List *list;
	unsigned int i;

	for (i = 0; i < table->size; ++i)
		if (checkEmptyBucket(table, i) == 0) {
			list = table->hashTable[i];
			for (; list != NULL; list = list->next) {
				writeToFile(output, file, list->word);
				writeToFile(output, file, " ");
			}
			writeToFile(output, file, "\n");
		}
}

/* Halves the hashtable, and the words are redistributed */
void resizeHalve(struct Hashtable *table)
{
	struct List *element;
	struct Hashtable *newTable;
	unsigned int i;

	if (table->size % 2 == 1)
		newTable = create((table->size - 1) / 2);
	else
		newTable = create(table->size / 2);

	for (i = 0; i < table->size; ++i)
		if (checkEmptyBucket(table, i) == 0) {
			element = table->hashTable[i];
			for (; element != NULL; element = element->next)
				add(newTable, element->word);
		}
	clear(table);
	*table = *newTable;
}

/* Doubles the size of the Hashtable */
void resizeDouble(struct Hashtable *table)
{
	struct List *element;
	struct Hashtable *newTable = create(2 * table->size);
	unsigned int i;

	for (i = 0; i < table->size; ++i) {
		if (checkEmptyBucket(table, i) == 0) {
			element = table->hashTable[i];
			for (; element != NULL; element = element->next)
				add(newTable, element->word);
		}
	}
	clear(table);
	*table = *newTable;
}

/* Verifies if a character is digit */
int isNumeric(char *number)
{
	while (*number != '\0') {
		if (*number < '0' || *number > '9')
			return 0;
		number++;
	}

	return 1;
}

/* Used for coding style, for removing the brackets (line 392) */
void doNothing(char *x)
{
	x = "Nothing";
}

/* Parse function */
int readFromFile(char *buf, char *line, FILE *input, FILE *output,
		 struct Hashtable *table, int size, char *type)
{
	char *getWord, *writeToOutput, *nothing;
	unsigned int i, k, j, found = 0;
	int index;
	struct List *temp;

	if (strcmp(type, "s") == 0)
		buf = fgets(line, 20000, stdin);
	else if (strcmp(type, "f") == 0)
		buf = fgets(line, 20000, input);

	if (buf == NULL) {
		fprintf(stderr, "Empty file");
		return -1;
	}
	while (buf) {
		if (strncmp(line, "add", 3) == 0) {
			getWord = strtok(line, "\n");
			if (getWord == NULL) {
				fprintf(stderr, "Wrong command");
				return -1;
			}
			getWord = strtok(line, " ");
			if (getWord == NULL) {
				fprintf(stderr, "Wrong command");
				return -1;
			}
			getWord = strtok(NULL, " ");
			if (getWord == NULL) {
				fprintf(stderr, "Wrong command");
				return -1;
			}
			add(table, getWord);
		} else if (strncmp(line, "remove", 6) == 0) {
			getWord = strtok(line, "\n");
			getWord = strtok(line, " ");
			getWord = strtok(NULL, " ");
			removeWord(table, getWord);
		} else if (strncmp(line, "find", 4) == 0) {
			getWord = strtok(line, "\n");
			getWord = strtok(line, " ");
			getWord = strtok(NULL, " ");
			found = find(table, getWord);

			writeToOutput = getWord;
			getWord = strtok(NULL, " ");
			if (getWord)
				while (getWord) {
					if (found == 1)
						writeToFile
						    (output, getWord, "True\n");
					else
						writeToFile
						    (output,
						     getWord, "False\n");
					getWord = strtok(NULL, " ");
				}
			else {
				if (find(table, writeToOutput)
				    == 1)
					printf("True\n");
				else
					printf("False\n");
			}
		} else if (strncmp(line, "clear", 5) == 0)
			clear(table);
		else if (strncmp(line, "print_bucket", 12) == 0) {
			getWord = strtok(line, "\n");
			getWord = strtok(line, " ");
			getWord = strtok(NULL, " ");

			if (isNumeric(getWord) == 0) {
				fprintf(stderr, "%s",
					"Invalid index for print_bucket!\n");
				return -1;
			}
			index = 0;
			index = atoi(getWord);
			getWord = strtok(NULL, " ");
			if (getWord)
				while (getWord) {
					printBucketInFile(table,
							  index,
							  output, getWord);
					getWord = strtok(NULL, " ");
			} else {
				printBucket(table, index);
				printf("\n");
			}

		} else if (strncmp(line, "print", 5) == 0) {
			getWord = strtok(line, "\n");
			getWord = strtok(line, " ");
			getWord = strtok(NULL, " ");

			if (getWord)
				while (getWord) {
					printInFile(table, output, getWord);
					getWord = strtok(NULL, " ");
			} else
				print(table);
		} else if (strncmp(line, "resize double", 13) == 0) {
			resizeDouble(table);
			printf("\n");
		} else if (strncmp(line, "resize halve", 12) == 0) {
			resizeHalve(table);
			printf("\n");
		} else if (strcmp(line, "\n") == 0)
			doNothing(nothing);
		else {
			fprintf(stderr, "BAD PARAMS!");
			return -1;
		}
		if (strcmp(type, "s") == 0)
			buf = fgets(line, 20000, stdin);
		else if (strcmp(type, "f") == 0)
			buf = fgets(line, 20000, input);
	}
}

/* Deallocates a Hashtable */
void freeTable(struct Hashtable *table)
{
	int i;
	struct List *temp;

	for (i = 0; i < table->size; ++i) {
		while (table->hashTable[i]) {
			temp = table->hashTable[i];
			table->hashTable[i] = table->hashTable[i]->next;
			free(temp->word);
			free(temp);
		}
		if (table->hashTable[i])
			free(table->hashTable[i]);
	}
	if (table->hashTable)
		free(table->hashTable);
	if (table)
		free(table);
}

int main(int argc, char *argv[])
{
	char *getWord, *writeToOutput, *doNothing, *type, *buf;
	unsigned int i, k, j, found = 0, index;
	unsigned int size;
	FILE *input, *output;
	char *line = malloc(20000 * sizeof(char));
	struct Hashtable *table;
	struct List *temp;

	if (argc < 2) {
		fprintf(stderr, "%s", "Invalid number of arguments!\n");
		return -1;
	}

	size = atoi(argv[1]);
	table = create(size);

	if (size <= 0) {
		fprintf(stderr, "%s", "Invalid size!\n");
		return -1;
	}

	if (argc == 2) {
		type = "s";
		readFromFile(buf, line, input, output, table, size, type);
	} else {
		type = "f";
		for (i = 2; i < argc; ++i) {
			input = fopen(argv[i], "r");
			if (input == NULL)
				continue;
			readFromFile(buf, line, input, output, table, size,
				     type);
			fclose(input);
		}
	}

	if (line)
		free(line);
	freeTable(table);
}
