//-------------------------------------------------------
//    Выполнил: Матвеев Никита группа 8362
//    Задание: 4 "Модель циклической системы управления"
//    Дата выполнения: 01.05.2023
//    Версия: 0.1
//-------------------------------------------------------
//    Последовательность команд для запуска программы
//        gcc T4_Matveev_8362.c -o T4_Matveev_8362 -pthread -lrt
//        chmod +x T4_Matveev_8362
//        ./T4_Matveev_8362
//-------------------------------------------------------
//    Три программных таймера с интервалами 1 с, 2 с и 3 с
//    вызывают три независимых потока с управляющими функциями doControl{1,2,3}().
//    Функция doControl{1,2,3}() выводит сообщение со значение времени текущего вызова и
//    значением количества времени, прошедшего с ее последнего вызова.
//-------------------------------------------------------

#include <stdio.h>      //!< Стандартная библиотека ввода\вывода
#include <stdlib.h>     //!< Стандартная библиотека функций
#include <pthread.h>    //!< Библиотека для работы с потоками
#include <unistd.h>     //!< Библиотека API POSIX
#include <signal.h>     //!< Библотека сигналов
#include <semaphore.h>  //!< Библиотека семафоров
#include <errno.h>      //!< Библиотека для подробного вывода ошибок
#include <time.h>       //!< Библиотека функций времени

//! \brief Определение типа булевых переменных
typedef enum
{
    false = 0,
    true  = 1
} bool;

#define DEBUG true

sem_t s; //!< Идентификатор семафора


time_t lastCall1 = 0;   //!< Переменная, хранящая время последнего вызова 1 функции управления
time_t lastCall2 = 0;   //!< Переменная, хранящая время последнего вызова 2 функции управления
time_t lastCall3 = 0;   //!< Переменная, хранящая время последнего вызова 3 функции управления

void doControl1 ();  //!< Функция управления 1
void doControl2 ();  //!< Функция управления 2
void doControl3 ();  //!< Функция управления 3

void printCurrTime ();                  //!< Функция вывода текущего времени
void printTimeDiff (time_t lastCall);   //!< Функция вывода времени, прошедшего с последнего вызова

int main (int argc, char *argv[])
{
    struct sigevent event1, event2, event3;
    struct itimerspec iTime1, iTime2, iTime3;
    timer_t timerId1, timerId2, timerId3;

    event1.sigev_notify = SIGEV_THREAD;
    event2.sigev_notify = SIGEV_THREAD;
    event3.sigev_notify = SIGEV_THREAD;

    event1.sigev_notify_function = &doControl1;
    event2.sigev_notify_function = &doControl2;
    event3.sigev_notify_function = &doControl3;

    event1.sigev_notify_attributes = NULL;
    event2.sigev_notify_attributes = NULL;
    event3.sigev_notify_attributes = NULL;

    if (timer_create (CLOCK_REALTIME, &event1, &timerId1))
    {
        if (DEBUG)
        {
            perror ("#!# (1) timer_create()");
        }
        return EXIT_FAILURE;
    }
    else
    {
        if (DEBUG)
        {
            printf ("## (1) timer initialized successfully\n");
        }
    }

    if (timer_create (CLOCK_REALTIME, &event2, &timerId2))
    {
        if (DEBUG)
        {
            perror ("#!# (2) timer_create()");
        }
        return EXIT_FAILURE;
    }
    else
    {
        if (DEBUG)
        {
            printf ("## (2) timer initialized successfully\n");
        }
    }

    if (timer_create (CLOCK_REALTIME, &event3, &timerId3))
    {
        if (DEBUG)
        {
            perror ("#!# (3) timer_create()");
        }
        return EXIT_FAILURE;
    }
    else
    {
        if (DEBUG)
        {
            printf ("## (3) timer initialized successfully\n");
        }
    }

    iTime1.it_value.tv_sec = 1;
    iTime1.it_value.tv_nsec = 0;
    iTime2.it_value.tv_sec = 1;
    iTime2.it_value.tv_nsec = 0;
    iTime3.it_value.tv_sec = 1;
    iTime3.it_value.tv_nsec = 0;

    iTime1.it_interval.tv_sec = 1;
    iTime1.it_interval.tv_nsec = 0;
    iTime2.it_interval.tv_sec = 2;
    iTime2.it_interval.tv_nsec = 0;
    iTime3.it_interval.tv_sec = 3;
    iTime3.it_interval.tv_nsec = 0;

    if (sem_init (&s, 0, 1))
    {
        if (DEBUG)
        {
            perror ("#!# sem_init() ");
        }
        return EXIT_FAILURE;
    }
    else
    {
        if (DEBUG)
        {
            printf ("## Semaphore initialized successfully\n");
        }
    }

    if (timer_settime(timerId1, 0, &iTime1, NULL))
    {
        if (DEBUG)
        {
            perror ("#!# (1) timer_settime() ");
        }
        return EXIT_FAILURE;
    }
    else
    {
        if (DEBUG)
        {
            printf ("## (1) timer start successfully\n");
        }
    }
    if (timer_settime(timerId2, 0, &iTime2, NULL))
    {
        if (DEBUG)
        {
            perror ("#!# (2) timer_settime() ");
        }
        return EXIT_FAILURE;
    }
    else
    {
        if (DEBUG)
        {
            printf ("## (2) timer start successfully\n");
        }
    }
    if (timer_settime(timerId3, 0, &iTime3, NULL))
    {
        if (DEBUG)
        {
            perror ("#!# (3) timer_settime() ");
        }
        return EXIT_FAILURE;
    }
    else
    {
        if (DEBUG)
        {
            printf ("## (3) timer start successfully\n");
        }
    }

    getchar ();

    if (sem_destroy (&s))
    {
        if (DEBUG)
        {
            perror("#!# sem_destroy() ");
        }
        return EXIT_FAILURE;
    }
    else
    {
        if (DEBUG)
        {
            printf ("## Semaphore destroyed successfully\n");
        }
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
void doControl1 ()
{
    sem_wait(&s);
    printf("\n## Start doControl1\n");

    printCurrTime();
    //-- Первый запуск
    if (lastCall1 == 0)
    {
        printf("### First call: 0 sec\n");
    }
    else
    {
        printTimeDiff (lastCall1);
    }
    lastCall1 = time (NULL);
    printf("## End doControl1\n");
    sem_post(&s);
}

void doControl2 ()
{
    sem_wait(&s);
    printf("\n## Start doControl2\n");

    printCurrTime();
    //-- Первый запуск
    if (lastCall2 == 0)
    {
        printf("### First call: 0 sec\n");
    }
    else
    {
        printTimeDiff (lastCall2);
    }
    lastCall2 = time (NULL);
    printf("## End doControl2\n");
    sem_post(&s);
}

void doControl3 ()
{
    sem_wait(&s);
    printf("\n## Start doControl3\n");

    printCurrTime();
    //-- Первый запуск
    if (lastCall3 == 0)
    {
        printf("### First call: 0 sec\n");
    }
    else
    {
        printTimeDiff (lastCall3);
    }
    lastCall3 = time (NULL);
    printf("## End doControl3\n");
    sem_post(&s);
}

void printCurrTime ()
{
    time_t result = time (NULL);
    printf ("### Current Time: %s", asctime (gmtime (&result)));
}

void printTimeDiff (time_t lastCall)
{
    time_t now = time (NULL);
    printf ("### Time from last call: %ld sec\n", now - lastCall);
    return;
}

//LOG:
//    ## (1) timer initialized successfully
//    ## (2) timer initialized successfully
//    ## (3) timer initialized successfully
//    ## Semaphore initialized successfully
//    ## (1) timer start successfully
//    ## (2) timer start successfully
//    ## (3) timer start successfully
//
//    ## Start doControl1
//    ### Current Time: Mon May  1 10:09:20 2023
//    ### First call: 0 sec
//    ## End doControl1
//
//    ## Start doControl2
//    ### Current Time: Mon May  1 10:09:20 2023
//    ### First call: 0 sec
//    ## End doControl2
//
//    ## Start doControl3
//    ### Current Time: Mon May  1 10:09:20 2023
//    ### First call: 0 sec
//    ## End doControl3
//
//    ## Start doControl1
//    ### Current Time: Mon May  1 10:09:21 2023
//    ### Time from last call: 1 sec
//    ## End doControl1
//
//    ## Start doControl2
//    ### Current Time: Mon May  1 10:09:22 2023
//    ### Time from last call: 2 sec
//    ## End doControl2
//
//    ## Start doControl1
//    ### Current Time: Mon May  1 10:09:22 2023
//    ### Time from last call: 1 sec
//    ## End doControl1
//
//    ## Start doControl3
//    ### Current Time: Mon May  1 10:09:23 2023
//    ### Time from last call: 3 sec
//    ## End doControl3
//
//    ## Start doControl1
//    ### Current Time: Mon May  1 10:09:23 2023
//    ### Time from last call: 1 sec
//    ## End doControl1
//
//    ## Start doControl2
//    ### Current Time: Mon May  1 10:09:24 2023
//    ### Time from last call: 2 sec
//    ## End doControl2
//
//    ## Start doControl1
//    ### Current Time: Mon May  1 10:09:24 2023
//    ### Time from last call: 1 sec
//    ## End doControl1
//
//    ## Start doControl1
//    ### Current Time: Mon May  1 10:09:25 2023
//    ### Time from last call: 1 sec
//    ## End doControl1
//
//    ## Start doControl1
//    ### Current Time: Mon May  1 10:09:26 2023
//    ### Time from last call: 1 sec
//    ## End doControl1
//
//    ## Start doControl2
//    ### Current Time: Mon May  1 10:09:26 2023
//    ### Time from last call: 2 sec
//    ## End doControl2
//
//    ## Start doControl3
//    ### Current Time: Mon May  1 10:09:26 2023
//    ### Time from last call: 3 sec
//    ## End doControl3
//
//    ## Semaphore destroyed successfully

