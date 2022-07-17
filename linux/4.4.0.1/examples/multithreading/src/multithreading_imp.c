#include "multithreading.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "qurt.h"
#include "hexagon_protos.h"
#include "remote.h"
#include "HAP_farf.h"
#include "HAP_perf.h"
#include "AEEStdErr.h"

/*
 * Macros and Constants
 */

#define ARRAY_SIZE 128*128*16
#define THREAD_NUM_BARRIER 16
#define THREAD_NUM_MUTEX 8
#define STACK_SIZE 1024*4

/*
 * Global Variables and Structures
 */

struct square_sum_args
{
    int *array;
    int result;
};

qurt_barrier_t barrier;
struct thread_info_t {
    qurt_thread_t id;
    char name[10];
    unsigned int prio;
    void *stack_addr;
};
struct thread_info_t thread_info[THREAD_NUM_BARRIER];

qurt_mutex_t testmutex;
qurt_anysignal_t testsignal;
qurt_anysignal_t testsignal2;

/*
 * Function Declarations
 */

void square_sum(void *arg);
void square_sum_l2prefetch(void *arg);

void barrier_thread(void *arg);

void mutex_thread(void *arg);


AEEResult multithreading_open(const char *uri, remote_handle64 *h)
{
    *h = 0x00DEAD00;
    return 0;
}

AEEResult multithreading_close(remote_handle64 h)
{
    return 0;
}


/*
 * This function illustrates the benefit of using multi-threading and L2 Prefetching
 * square_sum function calculates the sum of squares of elements of an array passed to it
 * square_sum_l2prefetch function calculates the sum of squares of elements of an array passed to it with L2 prefetching
 * Creation of multiple threads incurs a small additional overhead but increases the throughput as more instances of the function can be executed simultaneously
 */

int multithreading_parallel_sum(remote_handle64 h)
{
    qurt_thread_t tid1, tid2, tid3, tid4, tid5, tid6;
    qurt_thread_attr_t attr1, attr2, attr3, attr4, attr5, attr6;

    int retcode, status, thread_exit_status, thread_exit_status_1, thread_exit_status_2;
    int time_no_l2prefetch, time_multithreaded, time_l2prefetch, time_multithreaded_l2prefetch;

    void* thread_stack_addr[6];

/*
 * Qurt thread attributes such as the thread name, stack address, stack size and thread priority can be set using the qurt_thread_attr_set_ functions
 * Thread priorities are specified as integers in the range 0 to 255, with the lower numeric values representing higher thread priority
 * Thread priority must be set above QURT_THREAD_ATTR_PRIORITY_DEFAULT(255), i.e., less than 255
 * We initialize all threads with an equal priority value of: QURT_THREAD_ATTR_PRIORITY_DEFAULT/2 (127)
 */
    thread_stack_addr[0] = malloc(STACK_SIZE);
    assert(thread_stack_addr[0]!=NULL);
    qurt_thread_attr_init(&attr1);
    qurt_thread_attr_set_name(&attr1, (char *)"cntr1");
    qurt_thread_attr_set_stack_addr(&attr1, thread_stack_addr[0]);
    qurt_thread_attr_set_stack_size(&attr1, STACK_SIZE);
    qurt_thread_attr_set_priority(&attr1, QURT_THREAD_ATTR_PRIORITY_DEFAULT/2);

    thread_stack_addr[1] = malloc(STACK_SIZE);
    assert(thread_stack_addr[1]!=NULL);
    qurt_thread_attr_init(&attr2);
    qurt_thread_attr_set_name(&attr2, (char *)"cntr2");
    qurt_thread_attr_set_stack_addr(&attr2, thread_stack_addr[1]);
    qurt_thread_attr_set_stack_size(&attr2, STACK_SIZE);
    qurt_thread_attr_set_priority(&attr2, QURT_THREAD_ATTR_PRIORITY_DEFAULT/2);

    thread_stack_addr[2] = malloc(STACK_SIZE);
    assert(thread_stack_addr[2]!=NULL);
    qurt_thread_attr_init(&attr3);
    qurt_thread_attr_set_name(&attr3, (char *)"cntr3");
    qurt_thread_attr_set_stack_addr(&attr3, thread_stack_addr[2]);
    qurt_thread_attr_set_stack_size(&attr3, STACK_SIZE);
    qurt_thread_attr_set_priority(&attr3, QURT_THREAD_ATTR_PRIORITY_DEFAULT/2);

    thread_stack_addr[3] = malloc(STACK_SIZE);
    assert(thread_stack_addr[3]!=NULL);
    qurt_thread_attr_init(&attr4);
    qurt_thread_attr_set_name(&attr4, (char *)"cntr4");
    qurt_thread_attr_set_stack_addr(&attr4, thread_stack_addr[3]);
    qurt_thread_attr_set_stack_size(&attr4, STACK_SIZE);
    qurt_thread_attr_set_priority(&attr4, QURT_THREAD_ATTR_PRIORITY_DEFAULT/2);

    thread_stack_addr[4] = malloc(STACK_SIZE);
    assert(thread_stack_addr[4]!=NULL);
    qurt_thread_attr_init(&attr5);
    qurt_thread_attr_set_name(&attr5, (char *)"cntr5");
    qurt_thread_attr_set_stack_addr(&attr5, thread_stack_addr[4]);
    qurt_thread_attr_set_stack_size(&attr5, STACK_SIZE);
    qurt_thread_attr_set_priority(&attr5, QURT_THREAD_ATTR_PRIORITY_DEFAULT/2);

    thread_stack_addr[5] = malloc(STACK_SIZE);
    assert(thread_stack_addr[5]!=NULL);
    qurt_thread_attr_init(&attr6);
    qurt_thread_attr_set_name(&attr6, (char *)"cntr6");
    qurt_thread_attr_set_stack_addr(&attr6, thread_stack_addr[5]);
    qurt_thread_attr_set_stack_size(&attr6, STACK_SIZE);
    qurt_thread_attr_set_priority(&attr6, QURT_THREAD_ATTR_PRIORITY_DEFAULT/2);


    struct square_sum_args arg_1;
    struct square_sum_args arg_2;
    struct square_sum_args arg_3;
    struct square_sum_args arg_4;
    struct square_sum_args arg_5;
    struct square_sum_args arg_6;

    int *array_1 = (int*) malloc(ARRAY_SIZE * sizeof(int));
    assert(array_1!=NULL);

    int *array_2 = (int*) malloc(ARRAY_SIZE * sizeof(int));
    assert(array_2!=NULL);

    int *array_3 = (int*) malloc(ARRAY_SIZE * sizeof(int));
    assert(array_3!=NULL);

    int *array_4 = (int*) malloc(ARRAY_SIZE * sizeof(int));
    assert(array_4!=NULL);

    int *array_5 = (int*) malloc(ARRAY_SIZE * sizeof(int));
    assert(array_5!=NULL);

    int *array_6 = (int*) malloc(ARRAY_SIZE * sizeof(int));
    assert(array_6!=NULL);

    for(int i=0; i<ARRAY_SIZE; i++)
    {
        array_1[i] = i%3;
        array_2[i] = i%3;
        array_3[i] = i%3;
        array_4[i] = i%3;
        array_5[i] = i%3;
        array_6[i] = i%3;
    }

    arg_1.array = array_1;
    arg_2.array = array_2;
    arg_3.array = array_3;
    arg_4.array = array_4;
    arg_5.array = array_5;
    arg_6.array = array_6;

    /*
     * Thread Creation is done using the qurt_thread_create(tid, *attr, func, *arg) function
     * Upon successful creation, it returns QURT_EOK
     * qurt_thread_join(tid, *status) waits for a specific thread to finish based on the thread ID given
     * The caller thread is suspended until the callee thread exits
     */

    /*
     * Timing Measurements on target can be done using the HAP_perf_get_time_us() API which returns the time elapsed in microseconds
     * Please refer to $HEXAGON_SDK_ROOT/incs/HAP/HAP_perf.h for further information and usage of Timer APIs
     */

    /*
     * Single Thread without L2 Prefetch
     */

    FARF(ALWAYS, "Creating thread 1\n");

    uint64 start_time = HAP_perf_get_time_us();

    retcode = qurt_thread_create(&tid1, &attr1, square_sum, (void *) &arg_1);
    assert(retcode==QURT_EOK);

    status = qurt_thread_join(tid1, &thread_exit_status);
    assert((status==QURT_EOK) || (status==QURT_ENOTHREAD));

    time_no_l2prefetch = HAP_perf_get_time_us()-start_time;

    FARF(ALWAYS, "Thread 1 returned with status 0x%x\n", thread_exit_status);

    /*
     * Two Threads created without L2 Prefetch
     * There is a small overhead of an additional thread creation but two threads can be run concurrently, increasing the throughput
     */

    FARF(ALWAYS, "Creating thread 2\n");

    FARF(ALWAYS, "Creating thread 3\n");

    start_time = HAP_perf_get_time_us();

    retcode = qurt_thread_create(&tid2, &attr2, square_sum, (void *) &arg_2);
    assert(retcode==QURT_EOK);

    retcode = qurt_thread_create(&tid3, &attr3, square_sum, (void *) &arg_3);
    assert(retcode==QURT_EOK);

    status = qurt_thread_join(tid2, &thread_exit_status_1);
    assert((status==QURT_EOK) || (status==QURT_ENOTHREAD));

    status = qurt_thread_join(tid3, &thread_exit_status_2);
    assert((status==QURT_EOK) || (status==QURT_ENOTHREAD));

    time_multithreaded = HAP_perf_get_time_us()-start_time;

    FARF(ALWAYS, "Thread 2 returned with status 0x%x\n", thread_exit_status_1);

    FARF(ALWAYS, "Thread 3 returned with status 0x%x\n", thread_exit_status_2);

    /*
     * Single Thread with L2 Prefetching enabled
     */

    FARF(ALWAYS, "Creating thread 4\n");

    start_time = HAP_perf_get_time_us();

    retcode = qurt_thread_create(&tid4, &attr4, square_sum_l2prefetch, (void *) &arg_4);
    assert(retcode==QURT_EOK);

    status = qurt_thread_join(tid4, &thread_exit_status);
    assert((status==QURT_EOK) || (status==QURT_ENOTHREAD));

    time_l2prefetch = HAP_perf_get_time_us()-start_time;

    FARF(ALWAYS, "Thread 4 returned with status 0x%x\n", thread_exit_status);

    /*
     * Two Threads with L2 Prefetching enabled in both
     * Concurrently execute two threads with a small additional overhead of thread creation
     */

    FARF(ALWAYS, "Creating thread 5\n");

    FARF(ALWAYS, "Creating thread 6\n");

    start_time = HAP_perf_get_time_us();

    retcode = qurt_thread_create(&tid5, &attr5, square_sum_l2prefetch, (void *) &arg_5);
    assert(retcode==QURT_EOK);

    retcode = qurt_thread_create(&tid6, &attr6, square_sum_l2prefetch, (void *) &arg_6);
    assert(retcode==QURT_EOK);

    status = qurt_thread_join(tid5, &thread_exit_status_1);
    assert((status==QURT_EOK) || (status==QURT_ENOTHREAD));

    status = qurt_thread_join(tid6, &thread_exit_status_2);
    assert((status==QURT_EOK) || (status==QURT_ENOTHREAD));

    time_multithreaded_l2prefetch = HAP_perf_get_time_us()-start_time;

    FARF(ALWAYS, "Thread 5 returned with status 0x%x\n", thread_exit_status_1);

    FARF(ALWAYS, "Thread 6 returned with status 0x%x\n", thread_exit_status_2);




    FARF(ALWAYS, "Without L2 prefetch - Single Thread - sum_1: %d    \t\t    execution time for 1 array : %d micro seconds\n", arg_1.result, time_no_l2prefetch);
    FARF(ALWAYS, "Without L2 prefetch - Two Threads   - sum_2: %d, sum_3: %d  execution time for 2 arrays: %d micro seconds\n", arg_2.result, arg_3.result, time_multithreaded);
    FARF(ALWAYS, "With L2 prefetch    - Single Thread - sum_4: %d    \t\t    execution time for 1 array : %d micro seconds\n", arg_3.result, time_l2prefetch);
    FARF(ALWAYS, "With L2 prefetch    - Two Threads   - sum_5: %d, sum_6: %d  execution time for 2 arrays: %d micro seconds\n", arg_5.result, arg_6.result, time_multithreaded_l2prefetch);


    free(thread_stack_addr[0]);
    free(thread_stack_addr[1]);
    free(thread_stack_addr[2]);
    free(thread_stack_addr[3]);
    free(thread_stack_addr[4]);
    free(thread_stack_addr[5]);

    free(array_1);
    free(array_2);
    free(array_3);
    free(array_4);
    free(array_5);
    free(array_6);

    return AEE_SUCCESS;
}

void square_sum (void *arg)
{
    struct square_sum_args *square_sum_args_local = (struct square_sum_args*) arg;
    int *array = square_sum_args_local->array;
    int sum = 0;

    for(int i=0; i<ARRAY_SIZE; i++)
    {
        sum +=  array[i]*array[i];
    }

    square_sum_args_local->result = sum;

    qurt_thread_exit(QURT_EOK);
}

/*
 * L2 prefetching is done in chunks of 8KB of Memory at a time using the Q6_l2fetch_AR(Address, Rt) intrinsic
 * The region of memory to prefetch is encoded within the Rt register :
 *     Rt[7:0]   - Height : Number of "Width" size blocks to fetch
 *     Rt[15:8]  - Width  : Width of the block being fetched
 *     Rt[31:16] - Stride : Offset value to increment pointer after fetching a "Width" size block
 * The L2FETCH instruction is non-blocking and the program continues on to the next instrruction while the prefetching is done in the background
 *
 * Please refer to Hexagon Programmer's Reference Manual for further information about L2 Cache Prefetching
 */

void square_sum_l2prefetch (void *arg)
{
    struct square_sum_args *square_sum_args_local = (struct square_sum_args*) arg;
    int *array = square_sum_args_local->array;
    int sum = 0;

    int stride = ARRAY_SIZE>>7;

    for(int i=0; i<128; i++)
    {
        #ifndef TOOLV81
            Q6_l2fetch_AR(array+i*stride, 0x00808040);
        #endif

        for(int j=0; j<stride; j++)
        {
            sum +=  array[i*stride+j]*array[i*stride+j];
        }
    }

    square_sum_args_local->result = sum;

    qurt_thread_exit(QURT_EOK);
}

/*
 * Barriers are used as a fence for synchronizing the threads at a given point
 * Barriers are initialized with the number of threads to synchronize on it
 * A thread waiting on a barrier is suspended until the total number of threads waiting on the barrier is less than the given number
 * Once the total number of threads is greater than the threshold, all the threads waiting on the barrier are awakened
 *
 * The example creates 16 threads with increasing priority number (decreasing priority of execution)
 * Once all the threads are created and the number of threads waiting on the barrier reaches the threshold specified (17), all the threads are awakened
 * The threads with highest execution priority are scheduled first
 *
 * The APIs used are:
 * qurt_barrier_init(*barrier, num) : To initialize the barrier
 * qurt_barrier_wait(*barrier)      : Function to synchronize threads on the barrier
 * qurt_barrier_destroy(*barrier)   : To destroy the barrier
 */

int multithreading_barriers(remote_handle64 h)
{
    qurt_thread_attr_t thread_attr;
    int i=0, status, thread_exit_status;

    qurt_thread_attr_init(&thread_attr);
    qurt_barrier_init(&barrier, THREAD_NUM_BARRIER+1);

    for(i=0; i<THREAD_NUM_BARRIER; i++)
    {
        thread_info[i].stack_addr = malloc(STACK_SIZE);
        assert(thread_info[i].stack_addr != NULL);
        snprintf(thread_info[i].name, 10, "thread%d", i);
        qurt_thread_attr_set_name(&thread_attr, thread_info[i].name);
        qurt_thread_attr_set_stack_addr(&thread_attr, thread_info[i].stack_addr);
        qurt_thread_attr_set_stack_size(&thread_attr, STACK_SIZE);
        thread_info[i].prio = 100+i;
        qurt_thread_attr_set_priority(&thread_attr, thread_info[i].prio);
        status = qurt_thread_create(&thread_info[i].id, &thread_attr, barrier_thread, &thread_info[i]);
        FARF(ALWAYS, "thread%d created id = 0x%x\n", i, thread_info[i].id);
        assert(status==QURT_EOK);
    }

    qurt_barrier_wait(&barrier);

    for(i=0; i<THREAD_NUM_BARRIER; i++)
    {
        status = qurt_thread_join(thread_info[i].id, &thread_exit_status);
        assert((status==QURT_EOK) || (status==QURT_ENOTHREAD));
        FARF(ALWAYS, "thread%d return status 0x%x\n", i, thread_exit_status);
        if(status==QURT_EOK)
            assert(thread_exit_status==thread_info[i].id);
        free(thread_info[i].stack_addr);
    }

    qurt_barrier_destroy(&barrier);

    return AEE_SUCCESS;
}

void barrier_thread(void *arg)
{
    int status, id, prio;
    char thread_name[10];

    struct thread_info_t *ptr_thread_info = (struct thread_info_t*) arg;

    /*
     * A thread is suspended on this barrier until the total number of threads waiting on it has reached the threshold value of 17
     */
    qurt_barrier_wait(&barrier);
    qurt_thread_get_name(thread_name, 10);

    status = strcmp(ptr_thread_info->name, thread_name);
    assert(status==0);

    FARF(ALWAYS, "%s starts\n", thread_name);

    id = qurt_thread_get_id();
    assert(id==ptr_thread_info->id);

    prio = qurt_thread_get_priority(id);
    assert(prio==ptr_thread_info->prio);

    FARF(ALWAYS, "%s exiting\n", thread_name);

    qurt_thread_exit(id);
}


/*
 * Mutex Objects are used for mutually exclusive access to a Critical Code Section or Shared Resources
 *
 * Whenever a thread locks and acquires a mutex object, it enters the critical section and is protected by the mutex
 * Any other thread that tries to lock the mutex is suspended on it and added to a mutex wait queue
 * The thread with the highest execution priority is awakened whenever the mutex is unlocked
 *
 * The example creates 8 threads with decreasing priority number (increasing priority of execution)
 * Thread with the highest priority acquires the mutex lock, enters the critical section and unlocks the mutex before exiting
 * An unlocked mutex is always locked by the thread with the next highest priority
 *
 * The APIs used are:
 * qurt_mutex_init(*mutex)    : Initializes the mutex object
 * qurt_mutex_lock(*mutex)    : Thread locks the mutex object and enters a critical section
 * qurt_mutex_unlock(*mutex)  : Unlocks the mutex object, awakens the highest priority thread that is suspended on the mutex
 * qurt_mutex_destroy(*mutex) : Destroys the mutex object
 */

int multithreading_mutexes(remote_handle64 h)
{
    qurt_thread_attr_t thread_attr;
    char *thread_stack[THREAD_NUM_MUTEX];
    qurt_thread_t thread_id[THREAD_NUM_MUTEX];

    int i=0, status=0;
    char thread_name[10];

    qurt_thread_attr_init(&thread_attr);
    qurt_mutex_init(&testmutex);
    qurt_anysignal_init(&testsignal);
    qurt_anysignal_init(&testsignal2);

    thread_stack[0] = malloc(STACK_SIZE);
    assert(thread_stack[0]!=NULL);
    snprintf(thread_name, 10, "thread0");

    qurt_thread_attr_set_name(&thread_attr, thread_name);
    qurt_thread_attr_set_stack_addr(&thread_attr, thread_stack[0]);
    qurt_thread_attr_set_stack_size(&thread_attr, STACK_SIZE);
    qurt_thread_attr_set_priority(&thread_attr, 100);

    status = qurt_thread_create(&thread_id[0], &thread_attr, mutex_thread, NULL);
    assert(status==QURT_EOK);

    /*
     * Waits for a trigger from thread 0 to start the creation of thread1, thread2, ..., thread7
     */

    qurt_anysignal_wait(&testsignal, 0x1);

    for(i=1; i<THREAD_NUM_MUTEX; i++)
    {
        thread_stack[i] = malloc(STACK_SIZE);
        assert(thread_stack[i]!=NULL);
        snprintf(thread_name, 10, "thread%d", i);

        qurt_thread_attr_set_name(&thread_attr, thread_name);
        qurt_thread_attr_set_stack_addr(&thread_attr, thread_stack[i]);
        qurt_thread_attr_set_stack_size(&thread_attr, STACK_SIZE);
        qurt_thread_attr_set_priority(&thread_attr, 100-i);

        status = qurt_thread_create(&thread_id[i], &thread_attr, mutex_thread, NULL);
        assert(status==QURT_EOK);
    }

    qurt_anysignal_set(&testsignal2, 0x1);

    for(i=0; i<THREAD_NUM_MUTEX; i++)
    {
        qurt_thread_join(thread_id[i], &status);
        free(thread_stack[i]);
    }

    qurt_mutex_destroy(&testmutex);
    qurt_anysignal_destroy(&testsignal);
    qurt_anysignal_destroy(&testsignal2);

    return AEE_SUCCESS;
}

/*
 * Thread with the highest priority locks the mutex and enters the Critical Section
 * After exiting the Critical Section, thread unlocks the mutex and the thread waiting on the mutex with the next highest priority is awakened
 */

void mutex_thread(void *arg)
{
    int i, j, thread_index;
    int counter1, counter2;
    char thread_name[10];

    static unsigned int access_order;

    qurt_thread_get_name(thread_name, 10);
    thread_index = atoi(&thread_name[6]);
    FARF(ALWAYS, "\nthread%d starts\n", thread_index);

    qurt_mutex_lock(&testmutex);
    /*
     * Begin Critical Section
     */
    FARF(ALWAYS, "\nMutex Lock acquired by %s, Access Order = %d\n", thread_name, access_order);

    if(thread_index==0)
    {
        qurt_anysignal_set(&testsignal, 0x1);
        assert(access_order == 0);
        qurt_anysignal_wait(&testsignal2, 0x1);
    }
    else
    {
        assert(access_order == (THREAD_NUM_MUTEX-thread_index));
    }

    access_order++;

    for(i=0; i<10000; i++)
    {
        counter1 = -1;
        counter2 = -1;

        for(j=0; j<1000; j++)
        {
            counter1++;
            counter2++;

            assert(counter1==j&&counter2==j);
        }
    }

    /*
     * End Critical Section
     */

    qurt_mutex_unlock( &testmutex );

    FARF(ALWAYS, "\nMutex Released by %s\n", thread_name);

    qurt_thread_exit(QURT_EOK);
}
