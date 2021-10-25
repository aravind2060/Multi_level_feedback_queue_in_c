#include <stdio.h>
#include <stdlib.h>


/*total No.of processes store count of all processes*/
float totalNoOfProcesses = 0.0f;
/*To calculate Average turn around time we have to sum all of completion time*/
int completionTime = 0.0;
/*To calculate Completion time current time has to be tracked*/
int currentTime = 0;
/*To calcualte Average Wait time.we have to sum all of wait time*/
int waitTime = 0.0;

/**
 * This data structure helps us to store all the attributes of a process
 * **/
struct ProcessModel
{
    int processNumber;
    int burstTime;
    int balanceTime;
    int completionTime;
    struct ProcessModel *nextProcessModelReference;
};

struct Queue
{
    char *queueName;
    int timeQuantum;
    struct ProcessModel *head;
};

/**To ease printing of status of queue's these global variables are being used*/
struct Queue *globalqueue0;
struct Queue *globalqueue1;
struct Queue *globalfcfs;

/**
 * This methos initializes all queues 
*/
void initializeQueues(struct Queue *queue0, struct Queue *queue1, struct Queue *fcfs,struct Queue *storeCompletedProcess)
{

    queue0->head = NULL;
    queue0->queueName = "Q0";
    queue0->timeQuantum = 8;

    queue1->head = NULL;
    queue1->queueName = "Q1";
    queue1->timeQuantum = 16;

    fcfs->head = NULL;
    fcfs->queueName = "FCFS";
    fcfs->timeQuantum = -1;

    storeCompletedProcess->head=NULL;
    storeCompletedProcess->queueName="copyOfstoreCompletedProcesses";
    storeCompletedProcess->timeQuantum=-1;

    globalqueue0=queue0;
    globalqueue1=queue1;
    globalfcfs=fcfs;
}

/**
 * This method insert a process in queue
 * */
void insertProcessInQueue(struct Queue *sourceQueue, struct ProcessModel *process)
{

    struct ProcessModel *tempProcessModel = sourceQueue->head;

    if (sourceQueue->head == NULL)
    {
        sourceQueue->head = process;
        return;
    }

    while (tempProcessModel->nextProcessModelReference != NULL)
    {
        tempProcessModel = tempProcessModel->nextProcessModelReference;
    }
    tempProcessModel->nextProcessModelReference = process;
}

/**
 * This Method helps to print status of queue
 * 
 * */
void printStatusOfQueue()
{
    for(int i=0;i<3;i++){

    struct Queue *tempQueue;
    if(i==0)    
     tempQueue=globalqueue0;
    else if(i==1)
     tempQueue=globalqueue1;
   else
     tempQueue=globalfcfs;

    printf("%s->", tempQueue->queueName);
    struct ProcessModel *current = tempQueue->head;
    while (current != NULL)
    {
        int burstTime=current->burstTime;
        if(current->balanceTime>0)
          burstTime=current->balanceTime;
        printf("P%d(%d)->", current->processNumber,burstTime );
        current = current->nextProcessModelReference;
    }
    printf("\n");
   }
}

/**
 * This method helps to delete a process from the queue and change the head point to next process
 * */
void deleteCurrentProcessFromQueue(struct Queue *sourceQueue,struct ProcessModel *processTobeDeleted){
    
    struct ProcessModel *tempProcess=sourceQueue->head;
    if(sourceQueue->head==NULL)
     return;
    if(tempProcess->nextProcessModelReference!=NULL)
    {
        sourceQueue->head=tempProcess->nextProcessModelReference; 
    }
    else
      sourceQueue->head=NULL; 
}

/**
 * This method processes complete Queue 
 * where source represent current executing queue
 * and target queue represents next queue to be transfered from source if greater than time quantum
 * and store completedProcess queue is where All finished process are stored
 * */
void completeProcessInQueue(struct Queue *sourceQueue, struct Queue *targetQueue,struct Queue *storeCompletedProcess)
{
     
    if (targetQueue != NULL)
    {

        struct ProcessModel *tempProcess = sourceQueue->head;

        while (tempProcess != NULL)
        {
            int burstTime = tempProcess->burstTime;
            if (tempProcess->balanceTime > 0)
            {
                burstTime = tempProcess->balanceTime;
            }
           printf("P%d(%d)\n",tempProcess->processNumber,burstTime);
           struct ProcessModel *copyProcess = malloc(sizeof(struct ProcessModel));
           copyProcess->nextProcessModelReference = NULL;
           copyProcess->processNumber = tempProcess->processNumber;

            if (burstTime <= sourceQueue->timeQuantum)
            {
                copyProcess->completionTime=currentTime+burstTime;
                currentTime += burstTime;
                copyProcess->burstTime=tempProcess->burstTime;
                copyProcess->balanceTime=0;
                insertProcessInQueue(storeCompletedProcess,copyProcess);
            }
            else
            {
                currentTime += sourceQueue->timeQuantum;

                int balanceTime = burstTime - sourceQueue->timeQuantum;
                tempProcess->balanceTime = balanceTime;

                copyProcess->balanceTime = tempProcess->balanceTime;
                copyProcess->burstTime = tempProcess->burstTime;
                copyProcess->completionTime = tempProcess->completionTime;

                insertProcessInQueue(targetQueue, copyProcess);
            }
            deleteCurrentProcessFromQueue(sourceQueue,tempProcess);
            printStatusOfQueue();
             
            tempProcess = tempProcess->nextProcessModelReference;
        }
    }
    else
    {

        struct ProcessModel *tempProcess = sourceQueue->head;

        while (tempProcess != NULL)
        {
            int burstTime=tempProcess->burstTime;
            if(tempProcess->balanceTime>0)
             burstTime=tempProcess->balanceTime;
            printf("P%d(%d)\n",tempProcess->processNumber,burstTime);

            struct ProcessModel *copyProcess = malloc(sizeof(struct ProcessModel));
            copyProcess->nextProcessModelReference = NULL;
            copyProcess->processNumber=tempProcess->processNumber;
            copyProcess->balanceTime=tempProcess->balanceTime;
            copyProcess->burstTime=tempProcess->burstTime;
            
            tempProcess->completionTime = currentTime + burstTime;
            copyProcess->completionTime=currentTime+burstTime;
            currentTime += burstTime;

            insertProcessInQueue(storeCompletedProcess,copyProcess);
            deleteCurrentProcessFromQueue(sourceQueue,tempProcess);
            printStatusOfQueue();
            tempProcess = tempProcess->nextProcessModelReference;

        }
    }
}
void performMultiLevelFeedback(struct Queue *queue0, struct Queue *queue1, struct Queue *fcfs,struct Queue *storeCompletedProcess)
{

    for (int i = 0; i < 3; i++)
    {
        if (i == 0)
            completeProcessInQueue(queue0, queue1,storeCompletedProcess);
        else if (i == 1)
            completeProcessInQueue(queue1, fcfs,storeCompletedProcess);
        else
            completeProcessInQueue(fcfs, NULL,storeCompletedProcess);
    }
}
/**
 * Wait Time=Turn around time-Burst Time
 * As arrival time is zero so our effective turn around time is completion time
 * */
void calculateAverageWaitTime(struct Queue *storeCompletedProcess)
{
    struct ProcessModel *tempProcess1 = storeCompletedProcess->head;
    while (tempProcess1 != NULL)
    {
        waitTime += abs(tempProcess1->completionTime - tempProcess1->burstTime);
        tempProcess1 = tempProcess1->nextProcessModelReference;
    }
    printf("Average wait time:%f", waitTime / totalNoOfProcesses);
}

/**
 * Turn around time=Completion Time-Arrival Time
 * As arrival time  for all process is zero 
 * so effective turn around time =completion time 
 * */
void calculateAverageTurnAroundTime(struct Queue *storeCompletedProcess)
{

    struct ProcessModel *tempProcess1 = storeCompletedProcess->head;
    while (tempProcess1 != NULL)
    {
        completionTime = completionTime + tempProcess1->completionTime;
        tempProcess1 = tempProcess1->nextProcessModelReference;
    }
    printf("\nAverage turnaround time:%f", completionTime / totalNoOfProcesses);
}

int main()
{

    struct Queue *queue0 = malloc(sizeof(struct Queue));
    struct Queue *queue1 = malloc(sizeof(struct Queue));
    struct Queue *fcfs = malloc(sizeof(struct Queue));
    struct Queue *storeCompletedProcess=malloc(sizeof(struct Queue));

    initializeQueues(queue0, queue1, fcfs,storeCompletedProcess);

    for (int i = 0; i < 3; i++)
    {
        int no_of_processes_in_queue = 0;
        scanf("%d", &no_of_processes_in_queue);
        for (int j = 0; j < no_of_processes_in_queue; j++)
        {

            struct ProcessModel *temp = malloc(sizeof(struct ProcessModel));
            temp->balanceTime = 0;
            temp->completionTime = 0;
            temp->processNumber = totalNoOfProcesses++;
            temp->nextProcessModelReference = NULL;
            scanf("%d", &(temp->burstTime));

            if (i == 0)
            {
                insertProcessInQueue(queue0, temp);
            }
            else if (i == 1)
            {
                insertProcessInQueue(queue1, temp);
            }
            else
            {
                insertProcessInQueue(fcfs, temp);
            }
        }
    }
    printStatusOfQueue();

    performMultiLevelFeedback(queue0, queue1, fcfs,storeCompletedProcess);
    calculateAverageWaitTime(storeCompletedProcess);
    calculateAverageTurnAroundTime(storeCompletedProcess);

    return 0;
}