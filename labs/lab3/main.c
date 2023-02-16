//-------------------------------------------------------
//    Выполнил: Матвеев Никита группа 8362
//    Задание: 3 "Поставщик – Потребитель"
//    Дата выполнения:
//    Версия: 0.1
//-------------------------------------------------------
//    Последовательность команд для запуска программы
//        gcc T3_Matveev_8362.c -o T3_Matveev_8362 -pthread
//        chmod +x T3_Matveev_8362
//        ./T3_Matveev_8362
//-------------------------------------------------------
//
//-------------------------------------------------------

#include <stdio.h>      //!< Стандартная библиотека ввода\вывода
#include <stdlib.h>     //!< Стандартная библиотека функций
//#include <signal.h>     //!< Библиотека для работы с сигналами
#include <pthread.h>    //!< Библиотека для работы с потоками
#include <unistd.h>     //!< Библиотека API POSIX
#include <semaphore.h>  //!< Библиотека семафоров
#include <errno.h>

//! \brief Определение типа булевых переменных
typedef enum
{
    false = 0,
    true  = 1
} bool;

#define DEBUG true

typedef enum
{
    Semaphore = 0,  //!< Режим синхронизации с помощью семафоров
    Cond,           //!< Режим синхронизации с помощью условных переменных
} SyncMode;

static SyncMode syncMode = Semaphore;

sem_t s; //!< Идентификатор семафора

pthread_cond_t cond;    //!< Индентификатор переменной состояния
pthread_mutex_t mutex;  //!< Индентификатор мьютекса

static int buffer = 5;
static const int bufferMax = 10;
static const int bufferMin = 0;

#define TIME_CONSUMER 3 //!< Период обращения к переменной потребителя
#define TIME_SUPPLIER 1 //!< Период обращения к переменной поставщика

//! \brief Функция потока потребителя
void* Consumer (void* args);

//! \brief Функция потока поставщика
void* Supplier (void* args);



int main (int argc, char *argv[])
{
    if (syncMode == Semaphore)
    {
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
    }
    else if (syncMode == Cond)
    {
        if (pthread_cond_init(&cond, NULL))
        {
            if (DEBUG)
            {
                perror ("#!# pthread_cond_init() ");
            }
            return EXIT_FAILURE;
        }
        else
        {
            if (DEBUG)
            {
                printf ("## Pthread_cond initialized successfully\n");
            }
        }

        if (pthread_mutex_init(&mutex, NULL))
        {
            if (DEBUG)
            {
                perror ("#!# pthread_mutex_init() ");
            }
            return EXIT_FAILURE;
        }
        else
        {
            if (DEBUG)
            {
                printf ("## Mutex initialized successfully\n");
            }
        }
    }
    else
    {
        //-- TODO: сделать вывод ошибки выбора режима синхронизации
    }
    bool stopFlag = false;

    pthread_t threadCons, threadSup;
    pthread_create(&threadCons, NULL, &Consumer, &stopFlag);
    pthread_create(&threadSup, NULL, &Supplier, &stopFlag);

    getchar ();
    stopFlag = true;

    pthread_join(threadCons, NULL);
    pthread_join(threadSup, NULL);


    if (syncMode == Semaphore)
    {
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
        }
    }
    else if (syncMode == Cond)
    {
        if (pthread_cond_destroy (&cond))
        {
            if (DEBUG)
            {
                perror("#!# pthread_cond_destroy() ");
            }
            return EXIT_FAILURE;
        }
        else
        {
            if (DEBUG)
            {
                printf ("## Pthread_cond destroyed successfully\n");
            }
        }

        if (pthread_mutex_destroy (&mutex))
        {
            if (DEBUG)
            {
                perror("#!# pthread_mutex_destroy() ");
            }
            return EXIT_FAILURE;
        }
        else
        {
            if (DEBUG)
            {
                printf ("## Mutex destroyed successfully\n");
            }
        }
    }

    return EXIT_SUCCESS;
}

void* Consumer (void* args)
{
    bool *stopFlag = (bool *) args;
    if (syncMode == Semaphore)
    {
        while(!(*stopFlag))
        {
            //-- Критическая секция
            sem_wait (&s);
            if (buffer > bufferMin)
            {
                buffer--;
                printf ("*** Consumer modify buffer to: %d\n", buffer);
            }
            else
            {
                printf ("* Consumer cannot modify buffer because the value is minimum: %d\n", buffer);
            }
            sem_post (&s);
            //--

            sleep (TIME_CONSUMER);
        }

    }


    return EXIT_SUCCESS;
}

void* Supplier (void* args)
{
    bool *stopFlag = (bool *) args;
    if (syncMode == Semaphore)
    {
        while(!(*stopFlag))
        {
            //-- Критическая секция
            sem_wait (&s);
            if (buffer < bufferMax)
            {
                buffer++;
                printf ("*** Supplier modify buffer to: %d\n", buffer);
            }
            else
            {
                printf ("* Supplier cannot modify buffer because the value is maximum: %d\n", buffer);
            }
            sem_post (&s);
            //--

            sleep (TIME_SUPPLIER);
        }
    }
    return EXIT_SUCCESS;
}


//LOG:
//  Вариант 1 (Семафоры)
//
//  Вариант 2 (Условные переменные)
//
