//-------------------------------------------------------
//    Выполнил: Матвеев Никита группа 8362
//    Задание: 1  "Обеспечение тайм-аута"
//    Дата выполнения: 04.02.2023
//    Версия: 0.1
//-------------------------------------------------------
//    Последовательность команд для запуска программы
//        gcc T1_Matveev_8362.c -o T1_Matveev_8362 -pthread
//        chmod +x T1_Matveev_8362
//        ./T1_Matveev_8362
//-------------------------------------------------------
//    Программа запускает дочерний поток, в котором выполняется
//        "полезная нагрузка". Во время выполнения нагрузки с
//        шансом STUCK_CHANCE может произойти зависание.
//        В случае если выполнение не укладывается по времени в
//        заданный интервал генерируется сообщение об этом.
//        В случае если происходит зависание, генерируется
//        сообщение об этом и испускается сигнал SIGUSR1,
//        завершающий работу программы
//-------------------------------------------------------

#include <stdio.h>      //!< Стандартная библиотека ввода\вывода
#include <stdlib.h>     //!< Стандартная библиотека функций
#include <signal.h>     //!< Библиотека для работы с сигналами
#include <pthread.h>    //!< Библиотека для работы с потоками
#include <unistd.h>     //!< Библиотека API POSIX
#include <sys/time.h>   //!< Библиотека для работы с временем

//! \brief Определение типа булевых переменных
typedef enum
{
    false = 0,
    true  = 1
} bool;

#define STUCK_CHANCE 10                 //!< Шанс зависания функции в процентах (1-100)
#if STUCK_CHANCE < 0
    #error STUCK_CHANCE must be more or equal 0
#elif STUCK_CHANCE > 100
    #error STUCK_CHANCE must be less or equal 100
#endif

#define DEATHLINE 500000L               //!< Рассчетное время работы функции (мксек)
#define SOFT_DEATHLINE DEATHLINE*1.04   //!< "Мягкий" дедлайн работы функции (мксек)
#define MKSEC_PER_SEC 1000000L

//! \brief doControl
void doControl ();

//! \brief Рабочий поток
void *workingThread (void* arg);

//! \brief Обработчик превышения дедлайна
void deadlineHandler ();

//! \brief Обработчик сигнала об ошибке
void alarmHandler ();

int main (int argc, char *argv[])
{
    // Сбрасываем seed для rand ()
    srand (time (NULL));

    // Устанавливаем обрабочик для сигнала alarm ()
    signal (SIGALRM, alarmHandler);


    pthread_t threadId; //!< ID потока

    // Запускаем новый поток
    int ret = pthread_create (&threadId, NULL, workingThread, NULL);
    if (ret)
    {
        printf("Error creating working thread\n\tERROR NUMBER: %d\n", ret);

    }
    printf("Main thread waiting for finish working thread\n");

    pthread_join(threadId, NULL);
    return EXIT_SUCCESS;
}

void doControl ()
{
    long time,  //!< Переменная, содержащая общее время выполнения
         dt;    //!< Переменная, содержащая увеличение времени выполнения функции
    int x;      //!< Переменная, обеспечивающая шанс зависания процесса

    // Инициализация переменной зависания с помощью rand ()
    //      x принимает случайное значение от 1 до 100
    x = (rand () % 100) + 1;
    // Обеспечиваем зависание
    if (x <= STUCK_CHANCE)
    {
        while (true);
    }

    // Обеспечиваем опоздание в пределах мягкого дедлайна примерно в половине случаев
    double tmp = (double)(rand () % (long) ( (double)DEATHLINE * 0.08)) - (double)DEATHLINE * 0.04;
    dt = tmp <= 0 ? 0 : (long)tmp;

    // Высчитываем общее время работы функции для данной итерации и выводим его в консоль
    time = (long)DEATHLINE + dt;
    printf ("########## %ld \n", time);

    // Имитируем время работы doControl ()
    usleep (time);
}

void *workingThread (void* arg)
{
    while (true)
    {
        struct timeval start, end; //!< Структуры хранения меток времени

        // Пришлось установить alarm () на 1 секунду потому что
        //  он не принимает в качестве аргумента милисекунды
        alarm (1);

        // Получаем метку времени перед выполнением операции
        gettimeofday(&start, NULL);

        // Собственно сама функция
        doControl ();

        // сбрасываем alarm ()
        alarm(0);

        // Получаем метку времени после выполнения операции
        gettimeofday(&end, NULL);

        // Вычисляем время выполнения в микросекундах и сравниваем с "жестким" дедлайном
        //  если время просрочено, то вызываем метод оповещения об этом
        long dTime = (end.tv_sec-start.tv_sec) * MKSEC_PER_SEC + end.tv_usec-start.tv_usec;
        if (dTime > DEATHLINE * 1.01)   // Тут пришлось сделать модификатор из-за задержки самого измерения времени
                                        //  иначе даже при идеальном выполнении измеренное время получалось больше дедлайна
            deadlineHandler ();

        // Пауза между итерациями
        sleep (1);

    }
}

void deadlineHandler ()
{
    printf ("####   Deadline exceeded   ####\n");
}

void alarmHandler ()
{
    printf ("#####-- Restart requered! --#####\n");
    raise (SIGUSR1);
}


//LOG:
//    Main thread waiting for finish working thread
//    ########## 510488
//    ####   Deadline exceeded   ####
//    ########## 519850
//    ####   Deadline exceeded   ####
//    ########## 500000
//    ########## 500000
//    ########## 500000
//    ########## 500000
//    ########## 502443
//    #####-- Restart requered! --#####
//    User defined signal 1
