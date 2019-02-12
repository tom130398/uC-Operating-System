/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                   WIN32 PORT & LINUX PORT
*                          (c) Copyright 2004, Werner.Zimmermann@fht-esslingen.de
*                 (Similar to Example 1 of the 80x86 Real Mode port by Jean J. Labrosse)
*                                           All Rights Reserved
** *****************************************************************************************************
*       Further modified by     mikael.jakas@puv.fi & 
*                   Jukka.matila@vamk.fi
* *****************************************************************************************************
*                                               EXAMPLE #1
*********************************************************************************************************
*/

#include "includes.h"

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

#define  TASK_STK_SIZE                 512       /* Size of each task's stacks (# of WORDs)            */
#define  N_TASKS                         2       /* Number of identical tasks                          */

#define TASK_A_PRIO 1
#define TASK_B_PRIO 2

/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/

typedef struct TskC {
    INT16U a;
    INT16U b;
} TaskCount;

OS_STK        TaskStk[N_TASKS][TASK_STK_SIZE];        /* Tasks stacks                                  */
OS_STK        TaskStartStk[TASK_STK_SIZE];
char          TaskData[N_TASKS];                      /* Parameters to pass to each task               */
OS_EVENT*     PrintSem;
OS_EVENT*     CountAMail;
OS_EVENT*     CountBMail;

/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void TaskStart(void *data);        /* Function prototype of Startup task           */
void Task_A(void *data);           /* Function prototypes of Task A                */
void Task_B(void *data);           /* Function prototypes of Task B                */
void printCount(char task1, char task2, INT16U count);

/*$PAGE*/
/*
*********************************************************************************************************
*                                                MAIN
*********************************************************************************************************
*/

int  main(void)
{
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);      /* Clear the screen                         */

    OSInit();                                              /* Initialize uC/OS-II                      */

    PrintSem = OSSemCreate(1);
    CountAMail = OSMboxCreate(NULL);
    CountBMail = OSMboxCreate(NULL);

    OSTaskCreate(TaskStart, NULL, &TaskStartStk[TASK_STK_SIZE - 1], 0);
    OSStart();                                             /* Start multitasking                       */
    return 0;
}


/*
*********************************************************************************************************
*                                              STARTUP TASK
*********************************************************************************************************
*/
void  TaskStart(void *pdata)
{
    INT16S key;
    pdata = pdata;                                         /* Prevent compiler warning                 */

    OSTaskCreate(Task_A, NULL, (void *)&TaskStk[TASK_A_PRIO - 1][TASK_STK_SIZE - 1], TASK_A_PRIO);
    OSTaskCreate(Task_B, NULL, (void *)&TaskStk[TASK_B_PRIO - 1][TASK_STK_SIZE - 1], TASK_B_PRIO);

    for (;;) {

        if (PC_GetKey(&key) == TRUE) {                     /* See if key has been pressed              */
            if (key == 0x1B) {                             /* Yes, see if it's the ESCAPE key          */
                exit(0);                               /* End program                              */
            }
        }

        OSTimeDlyHMSM(0, 0, 1, 0);                         /* Wait one second                          */
    }
}

/*
*********************************************************************************************************
*                                                  TASKS
*********************************************************************************************************
*/

void Task_A(void *data)
{
    INT8U err;
    INT16U count_a = 0, count_b, *mail;

    // initial sync
    OSMboxPost(CountAMail, &count_a);
    mail = OSMboxPend(CountBMail, 0, &err);
    count_b = *mail;

    for (;;) {
        // check mail
        mail = OSMboxAccept(CountBMail);
        if (mail != NULL) {
            count_b = *mail;
        }
        count_a++;
        printCount('A', 'B', count_b);

        // clear last message
        OSMboxAccept(CountAMail);
        // send new message
        OSMboxPost(CountAMail, &count_a);

        OSTimeDlyHMSM(0, 0, 2, 0);
    }
} /*function code*/

void Task_B(void *data)
{
    INT8U err;
    INT16U count_a, count_b = 0, *mail;

    // initial sync
    OSMboxPost(CountBMail, &count_b);
    mail = OSMboxPend(CountAMail, 0, &err);
    count_a = *mail;

    for (;;) {
        // check mail
        mail = OSMboxAccept(CountAMail);
        if (mail != NULL) {
            count_a = *mail;
        }
        count_b++;
        printCount('B', 'A', count_a);

        // clear last message
        OSMboxAccept(CountBMail);
        // send new message
        OSMboxPost(CountBMail, &count_b);

        OSTimeDlyHMSM(0, 0, 2, 0);
    }
} /*function code*/

/*$PAGE*/
/*
*********************************************************************************************************
*                                      NON-TASK FUNCTIONS
*********************************************************************************************************
*/

void printCount(char task1, char task2, INT16U count){
    INT8U err;
    OSSemPend(PrintSem, 0, &err);
    printf("Task %c: The running time of task %c is %4d.\n", task1, task2, count);
    OSSemPost(PrintSem);
}

