#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

#define mainITM_Port8(n) (*((volatile unsigned char *)(0xE0000000 + 4 * n)))
#define mainITM_Port32(n) (*((volatile unsigned long *)(0xE0000000 + 4 * n)))
#define mainDEMCR (*((volatile unsigned long *)(0xE000EDFC)))
#define mainTRCENA 0x01000000

#define NUM_TASKS 3

#define TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define SCHEDULER_PRIORITY (TASK_PRIORITY + 1)

#define TASK1_EXECUTION_TIME (1000 / portTICK_RATE_MS)
#define TASK2_EXECUTION_TIME (2000 / portTICK_RATE_MS)
#define TASK3_EXECUTION_TIME (3000 / portTICK_RATE_MS)

#define SCHEDULER_PERIOD (1000 / portTICK_RATE_MS)
#define TASK1_PERIOD (4000 / portTICK_RATE_MS)
#define TASK2_PERIOD (6000 / portTICK_RATE_MS)
#define TASK3_PERIOD (8000 / portTICK_RATE_MS)

#define LIST_ITEM_OWNER(list_item) ((struct tcb_t *)list_item->pvOwner)

int fputc(int c, FILE *stream);
unsigned long ulTaskNumber[ configEXPECTED_NO_RUNNING_TASKS ];

struct tcb_t {
    int id;
    int execution_time;
    int period;
    int elapsed_time;
    int wake_time;
    xTaskHandle task_handle;
};

void scheduler(void *parameters);
void task1(void *parameters);
void task2(void *parameters);
void task3(void *parameters);

void update_blocked_queue(void);
void block_task(xListItem *task);
void resume_task(xListItem *task);
xListItem *next_ready_task(void);

struct tcb_t tcbs[NUM_TASKS];
xList ready_queue;
xList blocked_queue;
xListItem *current_task = NULL;

xListItem task1_item, task2_item, task3_item;

void initialize_tcb(struct tcb_t *tcb, int id, int execution_time, int period)
{
    tcb->id = id;
    tcb->execution_time = execution_time;
    tcb->period = period;
    tcb->elapsed_time = 0;
}

void initialize_queues(void)
{
    vListInitialise(&ready_queue);
    vListInitialise(&blocked_queue);

    vListInitialiseItem(&task1_item);
    listSET_LIST_ITEM_OWNER(&task1_item, &tcbs[0]);
    listSET_LIST_ITEM_VALUE(&task1_item, TASK1_PERIOD);

    vListInitialiseItem(&task2_item);
    listSET_LIST_ITEM_OWNER(&task2_item, &tcbs[1]);
    listSET_LIST_ITEM_VALUE(&task2_item, TASK2_PERIOD);

    vListInitialiseItem(&task3_item);
    listSET_LIST_ITEM_OWNER(&task3_item, &tcbs[2]);
    listSET_LIST_ITEM_VALUE(&task3_item, TASK3_PERIOD);

    vListInsert(&ready_queue, &task1_item);
    vListInsert(&ready_queue, &task2_item);
    vListInsert(&ready_queue, &task3_item);
}

int main(void)
{
    xTaskCreate(scheduler, "scheduler", configMINIMAL_STACK_SIZE, NULL, SCHEDULER_PRIORITY, NULL);
    xTaskCreate(task1, "task1", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY, &tcbs[0].task_handle);
    xTaskCreate(task2, "task2", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY, &tcbs[1].task_handle);
    xTaskCreate(task3, "task3", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY, &tcbs[2].task_handle);

    initialize_queues();

    initialize_tcb(&tcbs[0], 1, TASK1_EXECUTION_TIME, TASK1_PERIOD);
    initialize_tcb(&tcbs[1], 2, TASK2_EXECUTION_TIME, TASK2_PERIOD);
    initialize_tcb(&tcbs[2], 3, TASK3_EXECUTION_TIME, TASK3_PERIOD);

    vTaskSuspend(tcbs[0].task_handle);
    vTaskSuspend(tcbs[1].task_handle);
    vTaskSuspend(tcbs[2].task_handle);

    vTaskStartScheduler();
}

void scheduler(void *parameters)
{
    portTickType next_wake_time = xTaskGetTickCount();

    for (;;) {
        update_blocked_queue();

        if (current_task != NULL) {
            if (LIST_ITEM_OWNER(current_task)->elapsed_time >= LIST_ITEM_OWNER(current_task)->execution_time) {
                // if the task has completed, block it until its deadline passes
                block_task(current_task);
            } else {
                // otherwise, reinsert it into the ready queue
                vTaskSuspend(LIST_ITEM_OWNER(current_task)->task_handle);
                vListInsert(&ready_queue, current_task);
            }
        }

        // use the EDF algorithm to select the next task
        current_task = next_ready_task();

        if (current_task != NULL) {
            resume_task(current_task);
        }

        // block the scheduler for 1 time slice
        vTaskDelayUntil(&next_wake_time, SCHEDULER_PERIOD);
    }
}

void update_blocked_queue(void)
{
    int i = 1;
    volatile xListItem *iterator = NULL;

    // check for tasks whose wake times have passed
    for (iterator = blocked_queue.xListEnd.pxNext; i <= listCURRENT_LIST_LENGTH(&blocked_queue); iterator = iterator->pxNext) {
        if (xTaskGetTickCount() >= LIST_ITEM_OWNER(iterator)->wake_time) {
            // if the task's wake time has passed, move it into the ready queue
            vListRemove((xListItem *)iterator);
            vListInsert(&ready_queue, (xListItem *)iterator);
        }

        i += 1;
    }
}

void block_task(xListItem *task)
{
    vTaskSuspend(LIST_ITEM_OWNER(task)->task_handle);

    // set the wake time to the current deadline
    LIST_ITEM_OWNER(task)->wake_time = listGET_LIST_ITEM_VALUE(task);

    // add 1 period to the deadline
    listSET_LIST_ITEM_VALUE(task, listGET_LIST_ITEM_VALUE(task) + LIST_ITEM_OWNER(task)->period);

    // reset the elapsed time
    LIST_ITEM_OWNER(task)->elapsed_time = 0;

    vListInsert(&blocked_queue, task);
}

void resume_task(xListItem *task)
{
    vListRemove(task);

    // add 1 time slice to the elapsed time
    LIST_ITEM_OWNER(task)->elapsed_time += SCHEDULER_PERIOD;

    vTaskResume(LIST_ITEM_OWNER(task)->task_handle);
}

xListItem *next_ready_task(void)
{
    int i = 1;
    volatile xListItem *iterator = NULL;
    volatile xListItem *next_ready_task = NULL;

    if (listLIST_IS_EMPTY(&ready_queue)) {
        return NULL;
    }

    // the ready queue is sorted in increasing order of deadline
    next_ready_task = (xListItem *)ready_queue.xListEnd.pxNext;

    // check for deadline ties in the ready queue
    for (iterator = ready_queue.xListEnd.pxNext; i <= listCURRENT_LIST_LENGTH(&ready_queue); iterator = iterator->pxNext) {
        if (listGET_LIST_ITEM_VALUE(next_ready_task) == listGET_LIST_ITEM_VALUE(iterator)) {
            // in the case of a tie, select the lower-numbered task
            if (LIST_ITEM_OWNER(iterator)->id < LIST_ITEM_OWNER(next_ready_task)->id) {
                next_ready_task = iterator;
            }
        } else {
            break;
        }

        i += 1;
    }

    return (xListItem *)next_ready_task;
}

void task1(void *parameters)
{
    for (;;);
}

void task2(void *parameters)
{
    for (;;);
}

void task3(void *parameters)
{
    for (;;);
}

int fputc(int c, FILE *stream)
{
    if (mainDEMCR & mainTRCENA) {
        while (mainITM_Port32(0) == 0);
        mainITM_Port8(0) = c;
    }

    return c;
}
