/* Program:     priority based ready queue
   Author:      Alexander Thoennes
   Date:        August 28, 2017
   File name:   asgn3-thoennesa2.c
   Compile:     cc -o asgn3-thoennesa2 asgn3-thoennesa2.c
   Run:         asng3-thoennesa2
 Description:   This program allows the user the create and 
                interactively change a priority based ready 
                queue that they make. The commands are:
                + id program type priority runtime (this is for adding a process)
                - id                               (this is for the deletion of processes)
                ? id                               (this is for querying processes)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

// Struct for a process
typedef struct PROCESS
{
    int id;
    char program[25];
    int jobType;
    int priority;
    long arrivalTime;
    int execTime;
    struct PROCESS *prev;
    struct PROCESS *next;
}Process;

// struct for a header
// the header sits at the beginning of he list and
// always maintains the initial link to the first node
typedef struct HEADER
{
    Process *firstProcPtr;
    Process *tailPtr;
}Header;

// Prototype functions
void addElement(Header *headPtr, Process *process);
void printList(Header *headPtr);
void setTailPtr(Header *headPtr);
Process* processExists(Header *headPtr, int id);
void queryProcess(Header *headPtr, int id);
void deleteProcess(Header *headPtr, int id);
int canAdd(Header *headPtr, Process *process);

int main()
{
    // header object
    Header head;
    
    // this is a pointer that can only point to a Header object
    Header *headPtr = &head;
    
    // set the pointers to the first and last processes to NULL
    headPtr->firstProcPtr = NULL;
    headPtr->tailPtr = NULL;
    
    // set the pointer used for new processes to NULL
    Process *process = NULL;
    
    // variable used to store the current system time in seconds
    time_t currTime;
    
    char op[2];
    
    // run until the terminating character is entered
    while (op[0] != '#')
    {
        scanf("%s", op);
        
        // if you want to add an element
        if (op[0] == '+')
        {
            // get the current system time
            currTime = time(NULL);
            
            // add process
            process = (Process *) malloc(sizeof(Process));
            
            scanf("%d %s %d %d %d",
                  &process->id,
                  process->program,
                  &process->jobType,
                  &process->priority,
                  &process->execTime);
            
            process->arrivalTime = currTime;
            
            // check to see if you can add a process to the list
            if (canAdd(headPtr, process) == 0)
            {
                // add the element to the list
                addElement(headPtr, process);
            
                // adjust the pointer to the tail
                setTailPtr(headPtr);
            }

        } // if you want ot query an element
        else if (op[0] == '?')
        {
            // get the id of the process you want ot query
            int id;
            scanf("%d", &id);
            
            // query process
            queryProcess(headPtr, id);
        } // if you want to delete a process
        else if (op[0] == '-')
        {
            // get the id of the process you want to delete
            int id;
            scanf("%d", &id);
            
            // delete process
            deleteProcess(headPtr, id);
            
            // adjust the pointer to the tail
            setTailPtr(headPtr);
        }
    }
    
    // print the list
    printList(headPtr);
    
    return 0;
}

/**
 * function that adds elements to the list in ascending order
 * based on the id given to each process
 **/
void addElement(Header *headPtr, Process *process)
{
    // boolean like value, 0 means you haven't found a place and 1 means you have
    int foundPlace = 0;
    
    // current process you start on
    Process *current = headPtr->firstProcPtr;
    
    // process behind current
    Process *prevProc = NULL;
    
    // if list is empty then link header to the process
    if (current == NULL)
    {
        headPtr->firstProcPtr = process;
        process->next = NULL;
        process->prev = NULL; // set this to null for printing backwards
    } // check if the process you want to insert should go behind the first element in the list
    else if (process->priority < current->priority)
    {
        headPtr->firstProcPtr = process;
        process->next = current;
        current->prev = process;
    } // if the process needs to be inserted between two processes or at the end
    else
    {
        // go through the list
        while (current!=NULL)
        {
            // if you found the right place
            if (process->priority < current->priority)
            {
                // foundPlace acts as a boolean
                foundPlace = 1;
                break;
            } // otherwise keep going until the end of the list
            else
            {
                // move the pointer to the previous process up by one
                prevProc = current;
                
                // move the pointer to the current process up by one
                current = current->next;
            }
        }
        
        // if foundPlace == true
        if (foundPlace == 1)
        {
            // handle inserting the process
            process->next = current;
            process->prev = current->prev;
            current->prev->next = process;
            current->prev = process;
        } // otherwise you are inserting the node at the end of the list
        else
        {
            process->next = NULL;
            process->prev = prevProc;
            prevProc->next = process;
        }
    }
}

/**
 * function that goes through the whole list and finds the last 
 * process then has the tailPtr of the header point to that process
 **/
void setTailPtr(Header *headPtr)
{
    // start at the beginning of the list
    Process *processPtr = headPtr->firstProcPtr;
    
    if (processPtr != NULL)
    {
        // go through the list until you reach the last element
        while (processPtr->next != NULL)
        {
            processPtr = processPtr->next;
        }
        
        // have the tailPtr point to that last element
        headPtr->tailPtr = processPtr;
    }
}

/**
 * checks to see if you can add a process to the list
 **/
int canAdd(Header *headPtr, Process *process)
{
    int found = 0;
    
    Process *processPtr = headPtr->firstProcPtr;
    
    //compare all the process id's in the list to the one you want to insert
    while (processPtr != NULL)
    {
        if (processPtr->id == process->id)
        {
            found = 1;
            break;
        }
        else
        {
            processPtr = processPtr->next;
        }
    }
    
    // if found, then print out the error message
    if (found == 1)
    {
        printf("\nERROR - Process id exists - %d, %s, %d, %d, %ld, %d\n",
               processPtr->id,
               processPtr->program,
               processPtr->jobType,
               processPtr->priority,
               processPtr->arrivalTime,
               processPtr->execTime);
    }
    
    return found;

}

/**
 * function that checks to see if a process exists and 
 * then returns the found process (if there is one)
 **/
Process* processExists(Header *headPtr, int id)
{
    int found = 0;
    
    Process *processPtr = headPtr->firstProcPtr;
    
    while (processPtr != NULL)
    {
        if (processPtr->id == id)
        {
            found = 1;
            break;
        }
        else
        {
            processPtr = processPtr->next;
        }
    }
    
    return processPtr;
}

/**
 * function that query's a process
 **/
void queryProcess(Header *headPtr, int id)
{
    Process *processPtr = processExists(headPtr, id);
    
    if (processPtr != NULL)
    {
        printf("%d, %s, %d, %d, %ld, %d\n",
               processPtr->id,
               processPtr->program,
               processPtr->jobType,
               processPtr->priority,
               processPtr->arrivalTime,
               processPtr->execTime);
    }
    else
    {
        printf("\nERROR - Process does not exist.\n");
    }
}

/**
 * function for deleteing a process
 */
void deleteProcess(Header *headPtr, int id)
{
    Process *processPtr = processExists(headPtr, id);
    
    if (processPtr != NULL)
    {
        // if deleteing the first node
        if(headPtr->firstProcPtr->id == processPtr->id)
        {
            headPtr->firstProcPtr = processPtr->next;
            processPtr->next->prev = NULL;
        } // if deleting node between two nodes
        else if (processPtr->prev != NULL && processPtr->next != NULL)
        {
            processPtr->prev->next = processPtr->next;
            processPtr->next->prev = processPtr->prev;
        }
        else
        {
            headPtr->tailPtr->prev->next = NULL;
            processPtr->next = NULL;
        }
        
        // free the memory
        free(processPtr);
    }
    else
    {
        printf("\nERROR - Process does not exist.\n");
    }

}

/**
 * function that prints the doubly linked list both
 * forward and backward
 **/
void printList(Header *headPtr)
{
    printf("\nTERMINATE:\n");
    
    if (headPtr->firstProcPtr != NULL)
    {
        // start at the first process
        Process *processPtr = headPtr->firstProcPtr;
        
        // print the list in ascending order
        printf("\nAscending Order:\n");
        while (processPtr != NULL)
        {
            printf("%d, %s, %d, %d, %ld, %d\n",
                   processPtr->id,processPtr->program,
                   processPtr->jobType,
                   processPtr->priority,
                   processPtr->arrivalTime,
                   processPtr->execTime);
            
            processPtr = processPtr->next;
        }
        
        // set the pointer to the last element
        processPtr = headPtr->tailPtr;
        
        // print the list in descending order
        printf("\nDescending Order:\n");
        while (processPtr != NULL)
        {
            printf("%d, %s, %d, %d, %ld, %d\n",
                   processPtr->id,
                   processPtr->program,
                   processPtr->jobType,
                   processPtr->priority,
                   processPtr->arrivalTime,
                   processPtr->execTime);
            
            processPtr = processPtr->prev;
        }

    }
    else
    {
        printf("\nNo processes in ready queue.\n");
    }
    
    printf("\nEND\n");
}
