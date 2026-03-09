#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100
#define TIME_QUANTUM 5

/* ---------------- JOB STRUCTURE ---------------- */
typedef struct Job {
    int id;
    char name[50];
    int execTime;
    int priority;
    struct Job *next;
} Job;

/* ---------------- ARRIVAL QUEUE (Linked List) ---------------- */
Job *front = NULL, *rear = NULL;

void enqueueJob(int id, char name[], int time, int priority) {
    Job *newJob = (Job*)malloc(sizeof(Job));
    newJob->id = id;
    strcpy(newJob->name, name);
    newJob->execTime = time;
    newJob->priority = priority;
    newJob->next = NULL;

    if (rear == NULL) {
        front = rear = newJob;
    } else {
        rear->next = newJob;
        rear = newJob;
    }
}

Job* dequeueJob() {
    if (front == NULL) return NULL;

    Job *temp = front;
    front = front->next;
    if (front == NULL)
        rear = NULL;

    temp->next = NULL;
    return temp;
}

void displayQueue() {
    Job *temp = front;
    printf("\nArrival Queue:\n");
    while (temp) {
        printf("ID:%d Name:%s Time:%d Priority:%d\n",
               temp->id, temp->name, temp->execTime, temp->priority);
        temp = temp->next;
    }
}

/* ---------------- CIRCULAR QUEUE ---------------- */
Job* cqueue[MAX];
int cqFront = -1, cqRear = -1;

int isCQEmpty() {
    return cqFront == -1;
}

void cqEnqueue(Job *job) {
    if ((cqRear + 1) % MAX == cqFront) {
        printf("Circular Queue Full\n");
        return;
    }

    if (cqFront == -1)
        cqFront = 0;

    cqRear = (cqRear + 1) % MAX;
    cqueue[cqRear] = job;
}

Job* cqDequeue() {
    if (isCQEmpty())
        return NULL;

    Job *temp = cqueue[cqFront];

    if (cqFront == cqRear)
        cqFront = cqRear = -1;
    else
        cqFront = (cqFront + 1) % MAX;

    return temp;
}

/* ---------------- HISTORY (Deque style list) ---------------- */
Job *historyFront = NULL, *historyRear = NULL;

void addHistory(Job *job) {
    job->next = NULL;
    if (historyRear == NULL)
        historyFront = historyRear = job;
    else {
        historyRear->next = job;
        historyRear = job;
    }
}

/* ---------------- SCHEDULER ---------------- */
void runScheduler() {
    int totalTime = 0;

    while (!isCQEmpty()) {
        Job *job = cqDequeue();

        if (job->execTime > TIME_QUANTUM) {
            job->execTime -= TIME_QUANTUM;
            totalTime += TIME_QUANTUM;
            cqEnqueue(job);
        } else {
            totalTime += job->execTime;
            job->execTime = 0;
            addHistory(job);
        }
    }

    printf("\nAll jobs finished.\n");
    printf("Execution Order:\n");

    Job *temp = historyFront;
    while (temp) {
        printf("Job %d (%s)\n", temp->id, temp->name);
        temp = temp->next;
    }

    printf("Total Time Taken: %d ms\n", totalTime);

    /* Priority Analysis */
    int high=0, medium=0, low=0;
    temp = historyFront;

    while (temp) {
        if (temp->priority == 0) high++;
        else if (temp->priority == 1) medium++;
        else low++;
        temp = temp->next;
    }

    printf("Priority Analysis:\n");
    printf("High: %d  Medium: %d  Low: %d\n",
           high, medium, low);
}

/* ---------------- MAIN ---------------- */
int main() {
    int choice, id, time, priority;
    char name[50];

    while (1) {
        printf("\n1.Add Job\n2.Display Arrival Queue\n");
        printf("3.Start Scheduler\n4.Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter ID Name Time(ms) Priority(0-High,1-Med,2-Low):\n");
                scanf("%d %s %d %d", &id, name, &time, &priority);
                enqueueJob(id, name, time, priority);
                break;

            case 2:
                displayQueue();
                break;

            case 3:
                while (front != NULL)
                    cqEnqueue(dequeueJob());
                runScheduler();
                break;

            case 4:
                exit(0);

            default:
                printf("Invalid choice\n");
        }
    }
}
