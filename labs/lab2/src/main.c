//-------------------------------------------------------
//    Выполнил: Матвеев Никита группа 8362
//    Задание: 2 "Вызов лифта"
//    Дата выполнения: 16.02.2023
//    Версия: 0.1
//-------------------------------------------------------
//    Последовательность команд для запуска программы
//        gcc T2_Matveev_8362.c -o T2_Matveev_8362 -pthread
//        chmod +x T2_Matveev_8362
//        ./T2_Matveev_8362
//-------------------------------------------------------
//    Программа запускает дочерний поток, в котором выполняется
//      обработка ввода пользователя и вызов соответствующего
//      вводу сигнала (Up или Down) или завершение программы
//      обработки. Обработчик данных сигналов запускает эмуляцию
//      работы лифта в соответствии с нажимаемыми кнопками
//      вызова лифта
//-------------------------------------------------------

#include <stdio.h>      //!< Стандартная библиотека ввода\вывода
#include <stdlib.h>     //!< Стандартная библиотека функций
#include <signal.h>     //!< Библиотека для работы с сигналами
#include <pthread.h>    //!< Библиотека для работы с потоками
#include <unistd.h>     //!< Библиотека API POSIX

//! \brief Определение типа булевых переменных
typedef enum
{
    false = 0,
    true  = 1
} bool;

#define DEBUG false

//! \brief Перечисление описывающее возможные состояния лифта
typedef enum
{
    Top         = 2,    //!< Лифт выше нашего этажа
    ThisFloor   = 1,    //!< Лифт на нашем этаже
    Bottom      = 0,    //!< Лифт ниже нашего этажа
} LiftState;
static LiftState liftState = ThisFloor; //!< Переменная, содержащее текущее положение лифта

static const char bottomString[]    = "Bottom floor";
static const char currentString[]   = "This floor";
static const char topString[]       = "Top floor";
//! Текстовое представление элементов LiftState
static const char* LiftStateStrings[] = {bottomString, currentString, topString};

#define duration 3 //!< Константа указывающая сколько циклов необходимо для смены состояния (этажа)

#define sigUp   SIGRTMIN
#define sigDown SIGRTMIN + 1

//! \brief Функция для обработки ввода пользователя и вызова соответствующих методов
void* pushButton (void* args);

//! \brief Функция обработчик сигналов
//! \param signo - Номер сигнала
void liftControl(int signo, siginfo_t* info, void* nk);


//! \brief Функция реализующая смену состояния
//!     (По сути движение самого лифта)
//! \param targetState - Состояние к которому мы хотим перейти
void changeState (LiftState targetState);

int main (int argc, char *argv[])
{
    pthread_t t;
    printf ("Start\n");

    sigset_t set;
    sigemptyset (&set);
    sigaddset (&set, sigUp);
    sigaddset (&set, sigDown);

    printf ("Block Signals\n");
    pthread_sigmask (SIG_BLOCK, &set, NULL);

    pthread_create (&t, NULL, &pushButton, &set);

    pthread_join (t, NULL);
    printf ("Finish\n");
    return EXIT_SUCCESS;
}


void* pushButton (void* args)
{
    sigset_t *set = (sigset_t *) args;

    struct sigaction act;
    act.sa_sigaction = &liftControl;
    act.sa_flags = SA_SIGINFO;
    act.sa_mask = *set;
    sigaction(sigUp, &act, NULL);
    sigaction(sigDown, &act, NULL);

    if (DEBUG) printf ("Unblock Signals\n");
    pthread_sigmask(SIG_UNBLOCK, set, NULL);

    while (true)
    {
        char ch = getchar ();
        switch (ch)
        {
        case 'u':
            raise (sigUp);
            break;
        case 'd':
            raise (sigDown);
            break;
        case 'q':
            pthread_exit(EXIT_SUCCESS);
        default:
            break;
        };
    }
}

void liftControl (int sigNumber, siginfo_t* info, void* nk)
{

    if (DEBUG) printf ("[liftControl] receive signal\n");
    //-- Сначала в любом случае лифт должен приехать на наш этаж
    changeState (ThisFloor);

    if (sigNumber == sigUp)
    {
        if (DEBUG) printf ("[liftControl] receive signal \"sigUp\"\n");
        changeState (Top);
    }
    else if (sigNumber == sigDown)
    {
        if (DEBUG) printf ("[liftControl] receive signal \"sigDown\"\n");
        changeState (Bottom);
    }
    else
    {
        if (DEBUG) printf ("[liftControl] undefined signal number\n\tSignalNumber: %d\n", sigNumber);
    }
    return;
}

void changeState (LiftState targetState)
{
    //-- Проверка на корректность переданного аргумента
    if (targetState < 0 || targetState > 2)
    {
        printf ("[changeState] undefined targetState\n\ttargetState: %d\n", targetState);
    }

    //-- Проверяем что нам действительно необходимо двигаться
    //--    т.е. что мы еще не находимся на целевом этаже
    while (liftState != targetState)
    {
        //-- Если лифт находится ниже целевого этажа, едем вверх
        if (liftState < targetState)
        {
            for(int i = 0; i < duration; i++)
            {
                printf("*** Go UP\n");
                usleep(500000);
            }
            liftState++;
        }
        //-- Если лифт находится выше целевого этажа, едем вниз
        else
        {
            for(int i = 0; i < duration; i++)
            {
                printf("*** Go DOWN\n");
                usleep(500000);
            }
            liftState--;
        }
        printf("**** Current floor: %s\n", liftState[LiftStateStrings]);
    }

    //-- Имитируем открытие дверей, если лифт приехал на наш этаж
    if (targetState == ThisFloor)
    {
        printf ("This Floor, doors is opened\n");
        sleep (1);
        printf ("Doors closed\n");
    }

    return;
}

//LOG:
//    Start
//    Block Signals
//    u
//    This Floor, doors is opened
//    Doors closed
//    *** Go UP
//    *** Go UP
//    *** Go UP
//    **** Current floor: Top floor
//    d
//    *** Go DOWN
//    *** Go DOWN
//    *** Go DOWN
//    **** Current floor: This floor
//    This Floor, doors is opened
//    Doors closed
//    *** Go DOWN
//    *** Go DOWN
//    *** Go DOWN
//    **** Current floor: Bottom floor
//    d
//    *** Go UP
//    *** Go UP
//    *** Go UP
//    **** Current floor: This floor
//    This Floor, doors is opened
//    Doors closed
//    *** Go DOWN
//    *** Go DOWN
//    *** Go DOWN
//    **** Current floor: Bottom floor
//    q
//    Finish
