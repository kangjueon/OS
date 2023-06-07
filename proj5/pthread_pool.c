/*
 * 2017013663 강주언
 */
#include "pthread_pool.h"
#include <stdlib.h>

// 원형 버퍼 q가 꽉 차있으면 1
static bool is_full(pthread_pool_t *pool) {
    return pool->q_len == pool->q_size;
}

// 원형 버퍼 q가 비어 있으면 1
static bool is_empty(pthread_pool_t *pool) {
    return pool->q_len == 0;
}


static void enqueue(pthread_pool_t *pool, task_t task) {
    int index = (pool->q_front + pool->q_len) % pool->q_size;  //원형 buffer q_size
    pool->q[index] = task;  
    pool->q_len++;
}

static task_t dequeue(pthread_pool_t *pool) {
    task_t task = pool->q[pool->q_front]; // q_front에 있는 것을 작업한다.
    pool->q_front = (pool->q_front + 1) % pool->q_size;
    pool->q_len--;
    return task;
}

static void execute_task(task_t task) {
    task.function(task.param);
}


/*
 * 풀에 있는 일꾼(일벌) 스레드가 수행할 함수이다.
 * FIFO 대기열에서 기다리고 있는 작업을 하나씩 꺼내서 실행한다.
 * 대기열에 작업이 없으면 새 작업이 들어올 때까지 기다린다.
 * 이 과정을 스레드풀이 종료될 때까지 반복한다.
 */
static void *worker(void *param)
{
    pthread_pool_t *pool = (pthread_pool_t *)param;

    while (1) {
        pthread_mutex_lock(&pool->mutex);

        while (is_empty(pool) && pool->running) {
            pthread_cond_wait(&pool->empty, &pool->mutex);
        }

        if (!pool->running) {
            pthread_mutex_unlock(&pool->mutex);
            pthread_exit(NULL);
        }

        task_t task = dequeue(pool);
        pthread_cond_signal(&pool->full);
        
        pthread_mutex_unlock(&pool->mutex);

        execute_task(task);
    }
    
}

/*
 * 스레드풀을 생성한다. bee_size는 일꾼(일벌) 스레드의 개수이고, queue_size는 대기열의 용량이다.
 * bee_size는 POOL_MAXBSIZE를, queue_size는 POOL_MAXQSIZE를 넘을 수 없다.
 * 일꾼 스레드와 대기열에 필요한 공간을 할당하고 변수를 초기화한다.
 * 일꾼 스레드의 동기화를 위해 사용할 상호배타 락과 조건변수도 초기화한다.
 * 마지막 단계에서는 일꾼 스레드를 생성하여 각 스레드가 worker() 함수를 실행하게 한다.
 * 대기열로 사용할 원형 버퍼의 용량이 일꾼 스레드의 수보다 작으면 효율을 극대화할 수 없다.
 * 이런 경우 사용자가 요청한 queue_size를 bee_size로 상향 조정한다.
 * 성공하면 POOL_SUCCESS를, 실패하면 POOL_FAIL을 리턴한다.
 */
int pthread_pool_init(pthread_pool_t *pool, size_t bee_size, size_t queue_size)
{
     if (bee_size <= 0 || bee_size > POOL_MAXBSIZE || queue_size <= 0 || queue_size > POOL_MAXQSIZE) {
        return POOL_FAIL;
    }

    pool->running = true;
    pool->q_size = queue_size;
    pool->q_front = 0;
    pool->q_len = 0;
    pool->bee_size = bee_size;

    //상향 조정
    if (queue_size < bee_size)
        pool->q_size = bee_size;

    pool->q = (task_t *)malloc(sizeof(task_t) * pool->q_size);
    if (pool->q == NULL) {
        free(pool->q);
        return POOL_FAIL;
    }

    pool->bee = (pthread_t *)malloc(sizeof(pthread_t) * pool->bee_size);
    if (pool->bee == NULL) {
        free(pool->q);
        return POOL_FAIL;
    }

    if (pthread_mutex_init(&pool->mutex, NULL) != 0) {
        free(pool->q);
        free(pool->bee);
        pthread_mutex_destroy(&pool->mutex);
        return POOL_FAIL;
    }

    if (pthread_cond_init(&pool->full, NULL) != 0) {
        free(pool->q);
        free(pool->bee);
        pthread_cond_destroy(&pool->full);
        return POOL_FAIL;
    }

    if (pthread_cond_init(&pool->empty, NULL) != 0) {
        free(pool->q);
        free(pool->bee);
        pthread_cond_destroy(&pool->empty);
        return POOL_FAIL;
    }

    //bee_size만큼의 일꾼 스레드 생성
    for (size_t i = 0; i < bee_size; i++) {
        if (pthread_create(&pool->bee[i], NULL, worker, (void *)pool) != 0) {
            free(pool->q);
            free(pool->bee);
            pthread_mutex_destroy(&pool->mutex);
            pthread_cond_destroy(&pool->full);
            pthread_cond_destroy(&pool->empty);
            return POOL_FAIL;
        }
    }

    return POOL_SUCCESS;
}

/*
 * 스레드풀에서 실행시킬 함수와 인자의 주소를 넘겨주며 작업을 요청한다.
 * 스레드풀의 대기열이 꽉 찬 상황에서 flag이 POOL_NOWAIT이면 즉시 POOL_FULL을 리턴한다.
 * POOL_WAIT이면 대기열에 빈 자리가 나올 때까지 기다렸다가 넣고 나온다.
 * 작업 요청이 성공하면 POOL_SUCCESS를 리턴한다.
 */
int pthread_pool_submit(pthread_pool_t *pool, void (*f)(void *p), void *p, int flag)
{
    if (pool == NULL || f == NULL) {
        return POOL_FAIL;
    }

    pthread_mutex_lock(&pool->mutex);

   if (is_full(pool)) {
        if (flag == POOL_NOWAIT) {
            pthread_mutex_unlock(&pool->mutex);
            return POOL_FULL;
        } else if (flag == POOL_WAIT) {
             if (!is_empty(pool)) {
                task_t task = dequeue(pool);
                pthread_mutex_unlock(&pool->mutex);
                execute_task(task);
                return POOL_SUCCESS;
            }

            while (!is_empty(pool)) {
                pthread_cond_wait(&pool->full, &pool->mutex);
            }
        }
    }

    enqueue(pool, (task_t){.function = f, .param = p});
    pthread_cond_broadcast(&pool->empty); //작업이 채워졌다
    pthread_mutex_unlock(&pool->mutex);
    

    return POOL_SUCCESS;

}    

/*
 * 스레드풀을 종료한다. 일꾼 스레드가 현재 작업 중이면 그 작업을 마치게 한다.
 * how의 값이 POOL_COMPLETE이면 대기열에 남아 있는 모든 작업을 마치고 종료한다.
 * POOL_DISCARD이면 대기열에 새 작업이 남아 있어도 더 이상 수행하지 않고 종료한다.
 * 부모 스레드는 종료된 일꾼 스레드와 조인한 후에 스레드풀에 할당된 자원을 반납한다.
 * 스레드를 종료시키기 위해 철회를 생각할 수 있으나 바람직하지 않다.
 * 락을 소유한 스레드를 중간에 철회하면 교착상태가 발생하기 쉽기 때문이다.
 * 종료가 완료되면 POOL_SUCCESS를 리턴한다.
 */
int pthread_pool_shutdown(pthread_pool_t *pool, int how)
{
    if (pool == NULL) {
        return POOL_FAIL;
    }

    pthread_mutex_lock(&pool->mutex);
    pool->running = false;
    if (how == POOL_COMPLETE) {
       
        // 대기열에 남아 있는 모든 작업을 마치도록 기다림
        while (pool->q_len > 0) {
            pthread_cond_wait(&pool->empty, &pool->mutex);
        }

    } else if (how == POOL_DISCARD) {
        // 대기열에 남아 있는 작업을 무시하고 종료
        pool->q_len = 0;
        pool->q_front = 0;
    }

   
    pthread_mutex_unlock(&pool->mutex);

    // 대기열에 대기 중인 작업을 마치도록 신호 전달
    pthread_cond_broadcast(&pool->empty);

    for (size_t i = 0; i < pool->bee_size; i++) {
        pthread_join(pool->bee[i], NULL);
    }

    pthread_cond_destroy(&pool->full);
    pthread_cond_destroy(&pool->empty);
    pthread_mutex_destroy(&pool->mutex);
    free(pool->q);
    free(pool->bee);

    return POOL_SUCCESS;
}
