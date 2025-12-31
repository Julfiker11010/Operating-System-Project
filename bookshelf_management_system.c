#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_BOOKS 20

// Simple book structure
typedef struct {
    char title[50];
} Book;

Book inventory[MAX_BOOKS];
int book_count = 0;

pthread_mutex_t lock;    
sem_t sort_done;          

void* sort_books(void *arg) {
    pthread_mutex_lock(&lock);

    for (int i = 0; i < book_count - 1; i++) {
        for (int j = 0; j < book_count - i - 1; j++) {
            if (strcmp(inventory[j].title, inventory[j+1].title) > 0) {
                Book temp = inventory[j];
                inventory[j] = inventory[j+1];
                inventory[j+1] = temp;
            }
        }
    }

    printf("\n[Thread] Sorting completed!\n");

    pthread_mutex_unlock(&lock);
    sem_post(&sort_done);  
    return NULL;
}

void add_book(char *title) {
    pthread_mutex_lock(&lock);

    if (book_count < MAX_BOOKS) {
        strcpy(inventory[book_count].title, title);
        book_count++;
        printf("Book added successfully!\n");
    } else {
        printf("Inventory full!\n");
    }

    pthread_mutex_unlock(&lock);
}

void display_books() {
    pthread_mutex_lock(&lock);

    printf("\n--- Book List ---\n");
    if (book_count == 0) {
        printf("No books available.\n");
    } else {
        for (int i = 0; i < book_count; i++) {
            printf("%d. %s\n", i + 1, inventory[i].title);
        }
    }

    pthread_mutex_unlock(&lock);
}

int main() {
    pthread_t sort_thread;
    int choice;
    char title[50];

    pthread_mutex_init(&lock, NULL);
    sem_init(&sort_done, 0, 0);

    while (1) {
        printf("\n--- Simple Book Inventory (OS Project) ---\n");
        printf("1. Add Book\n");
        printf("2. Display Books\n");
        printf("3. Sort Books (Thread)\n");
        printf("4. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 1) {
            printf("Enter book title: ");
            fgets(title, sizeof(title), stdin);
            title[strcspn(title, "\n")] = 0;
            add_book(title);
        }
        else if (choice == 2) {
            display_books();
        }
        else if (choice == 3) {
            printf("Sorting in a separate thread...\n");
            pthread_create(&sort_thread, NULL, sort_books, NULL);
            sem_wait(&sort_done);
            pthread_join(sort_thread, NULL);
            printf("Books sorted successfully!\n");
        }
        else if (choice == 4) {
            printf("Exiting program...\n");
            break;
        }
        else {
            printf("Invalid choice. Try again.\n");
        }
    }

    pthread_mutex_destroy(&lock);
    sem_destroy(&sort_done);
    return 0;
}
