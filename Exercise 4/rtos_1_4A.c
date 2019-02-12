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
#define  N_TASKS                        2       /* Number of identical tasks                          */

/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/
typedef struct TskC {
    INT8U a;
    INT8U b;
} TaskCount;

OS_STK        TaskStk[N_TASKS][TASK_STK_SIZE];        /* Tasks stacks                                  */
OS_STK        TaskStartStk[TASK_STK_SIZE];
char          TaskData[N_TASKS];                      /* Parameters to pass to each task               */
OS_EVENT     *RandomSem;
OS_EVENT     *taskSem;
INT8U         err;
TaskCount     count;

/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void TaskStart(void *data);        /* Function prototype of Startup task           */
void task_a(void *data);
void task_b(void *data);
INT8U read_value(INT8U task);
void write_value(INT8U task, INT8U value);

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
    RandomSem=OSSemCreate(1);
    taskSem=OSSemCreate(1);
    count.a=0;
    count.b=0;
    OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);
    OSStart();      /* Start multitasking                       */
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
    OSTaskCreate(task_a, NULL, (void *)&TaskStk[0][TASK_STK_SIZE - 1], 1);
    OSTaskCreate(task_b, NULL, (void *)&TaskStk[1][TASK_STK_SIZE - 1], 2);
  // moved for above this line 
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


void task_a(void *data){
    INT8U count_a = 0, count_b;
    for(;;){
        count_b=read_value(2);
        count_a++;
        printf("Task A: The running time of task B is: %4d\n", count_b);
        write_value(1, count_a);
        OSTimeDlyHMSM(0,0,3,0);
    }
}

void task_b(void *data){
    INT8U count_b = 0, count_a;
    for(;;){
        count_a=read_value(1);
        count_b++;
        printf("Task B: The running time of task A is: %4d\n", count_a);
        write_value(2, count_b);
        OSTimeDlyHMSM(0,0,3,0);
    }
}

INT8U read_value(INT8U task){
    INT8U res=0;
    OSSemPend(RandomSem, 0, &err);
    if(task==1){
        res=count.a;
    }
    if(task==2){
        res=count.b;
    }
    OSSemPost(RandomSem);
    return res;
}

void write_value(INT8U task, INT8U value){
    OSSemPend(RandomSem, 0, &err);
    if(task==1){
        count.a=value;
    }
    if(task==2){
        count.b=value;
    }
    OSSemPost(RandomSem);
}
/*function code*/

/*$PAGE*/
/*
*********************************************************************************************************
*                                      NON-TASK FUNCTIONS
*********************************************************************************************************
*/
