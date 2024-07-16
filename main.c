#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Число итераций
int itersCnt = 0;

// Максимально число итераций до останова цикла поиска корня
#define ITERS_MAX 1000000
#define INTEGRAL_ITERS_MAX 5e7
#define MAX_INTEGRAL 1e9

// Функции описаны в funcs.asm

extern double _f1(double x);
extern double _f2(double x);
extern double _f3(double x);
double f1(double x) {return _f1(x);}
double f2(double x) {return _f2(x);}
double f3(double x) {return _f3(x);}

/*
double f1(double x)
{
    return 3 / ((x - 1) * (x - 1) + 1);
}

double f2(double x)
{
    return sqrt(x + 0.5);
}

double f3(double x)
{
    return exp(-x);
}
*/
// найти корень уравнения f(x) = g(x) на отрезке [a, b]
double root(double (*f)(double x), double (*g)(double x), double a, double b, double eps1)
{
    double fgA = (*f)(a) - (*g)(a);
    double fgB = (*f)(b) - (*g)(b);
    // printf("%.5lf %.5lf\n", fgA, fgB);
    if (isnan(fgA) || isnan(fgB))
    {
        printf("Root Exception: Границы отрезков вне области определения функции\n");
        exit(0);
    }
    if (fabs(fgA) == 0)
        return a;
    if (fabs(fgB) == 0)
        return b;
    if (fgA * fgB > 0)
    {
        printf("Root Exception: на отрезке [%.5lf, %.5lf] не существует корней.\n", a, b);
        exit(0);
    }
    int cnt = 0;

#ifdef SECANT
    // метод хорд
    // предыдущие точки
    //printf("penis\n");
    double x_k2 = b;
    double x_k1 = a;
    //костыль, нужный по причине того, что при плохо выбранном отрезке (например если значения функции стремятся к 0 на нем) и эпсилоне, программа выдаст что это корень, что будет ошибкой.    
    if (x_k1 < -0.5)
        x_k1 = -0.5;
    if (x_k2 > 2)
        x_k2 = 2;
    double fgk1 = (*f)(x_k1) - (*g)(x_k1);
    double fgk2 = (*f)(x_k2) - (*g)(x_k2);
    if (isnan(fgk1) || isnan(fgk2))
    {
        printf("Root Exception: Границы отрезков вне области определения функции\n");
        exit(0);
    }
    //printf("%.5lf %.5lf\n", a, b);
    // точка с корнем
    double x0 = x_k1 - (x_k2 - x_k1) * (fgk1) / ((fgk2) - (fgk1));
    double fgx0 = (*f)(x0) - (*g)(x0);
    // printf("%.5lf %.5lf\n", x0, fgx0);

    if (isnan(fgx0))
    {
        printf("Root Exception: Границы отрезков вне области определения функции\n");
        exit(0);
    }
    // в зависимости от выпуклости функции, переворачиваем отрезок, чтобы в любом случае двигать x_k1
    if (fgx0 * fgk1 < 0)
    {
        double tmp = x_k1;
        x_k1 = x_k2;
        x_k2 = tmp;
        tmp = fgk1;
        fgk1 = fgk2;
        fgk2 = tmp;
        x0 = x_k1 - (x_k2 - x_k1) * (fgk1) / ((fgk2) - (fgk1));
        fgx0 = (*f)(x0) - (*g)(x0);
        if (isnan(fgx0))
        {
            printf("Root Exception: Границы отрезков вне области определения функции\n");
            exit(0);
        }
    }
    // printf("%.5lf %.5lf\n-----------\n", x0, fgx0);
    // будем двигать границу x_k1
    while (fabs(x0 - x_k1) > eps1 / 2.0 && cnt < ITERS_MAX)
    {
        x_k1 = x0;
        fgk1 = (*f)(x_k1) - (*g)(x_k1);
        // printf("%.5lf %.5lf\n", x_k1, fgk1);
        if (isnan(fgk1))
        {
            printf("Root Exception: Границы отрезков вне области определения функции\n");
            exit(0);
        }
        x0 = x_k1 - (x_k2 - x_k1) * (fgk1) / ((fgk2) - (fgk1));
        fgx0 = (*f)(x0) - (*g)(x0);
        //printf("%.5lf %.5lf %d\n-------\n", x0, fgx0, cnt);
        if (isnan(fgx0))
        {
            printf("Root Exception: Границы отрезков вне области определения функции\n");
            exit(0);
        }
        cnt++;
    }

#else
    // метод отрезков
    double x0 = (a + b) / 2.0;
    double fgx0 = (*f)(x0) - (*g)(x0);
    if (isnan(fgx0))
    {
        printf("Root Exception: Границы отрезков вне области определения функции\n");
        exit(0);
    }
    while (fabs(b - a) > eps1 / 2.0 && cnt < ITERS_MAX)
    {
        if (fgx0 * fgA > 0)
        {
            a = x0;
        }
        else
        {
            b = x0;
        }
        x0 = (a + b) / 2.0;
        fgx0 = (*f)(x0) - (*g)(x0);
        if (isnan(fgx0))
        {
            printf("Root Exception: Границы отрезков вне области определения функции\n");
            exit(0);
        }
        cnt++;
        //printf("%.5lf %.5lf %.5lf %d\n", a, b, fgx0, cnt);
    }
    // return x0;
#endif
    if (cnt >= ITERS_MAX)
    {
        printf("Root Exception: на отрезке [%.5lf, %.5lf] поиск корней идет слишком долго.\n", a, b);
        exit(0);
    }
    itersCnt = cnt;
    return x0;
}


// вычисление интеграла методом трапеций
double integral(double (*f)(double x), double a, double b, double eps2)
{
    // количество отрезков
    int n = 1;
    // шаг
    double dx = b - a;
    // формула двух точек
    double answ = ((*f)(b) - (*f)(a)) * 0.5 * dx;
    if (isnan((*f)(a)))
    {
        printf("Integral Exception: Границы отрезков вне области определения функции\n");
        exit(0);
    }
    double diff = eps2 + 1;

    while (diff > eps2)
    {
        if (answ > MAX_INTEGRAL)
        {
            printf("INTEGRAL EXCEPTION: Интеграл на отрезке [%.5lf, %.5lf] слишком большой.\n", a, b);
            exit(0);
        }
        if (n > INTEGRAL_ITERS_MAX)
        {
            printf("INTEGRAL EXCEPTION: Точность интеграла на отрезке [%.5lf, %.5lf] слишком большая .\n", a, b);
            exit(0);
        }
        double prevAnsw = answ;
        // точки в серединах отрезков
        double midPoints = 0;
        for (int i = 0; i < n; i++)
            midPoints += (*f)(a + dx * (i + 0.5));
        // пересчитываем формулу трапеций
        answ += midPoints * dx;
        answ *= 0.5;
        // Увеличиваем количество отрезков
        n *= 2;
        dx *= 0.5;
        diff = fabs(answ - prevAnsw);
        // printf("%.5lf %.5lf %d\n", answ, diff, n);
    }

    return answ;
}

//Вычисление площади с возможностью показа абсцисс точек пересечения и числа операций

double area(int absMode, int iterMode, double eps)
{
    eps /= 10.0;
    double rt13 = root(f1, f3, -0.262, 0.323, eps);
    int iter13 = itersCnt;
    double rt12 = root(f1, f2, 1, 2, eps);
    int iter12 = itersCnt;
    double rt23 = root(f2, f3, 0.0, 1.0, eps);
    int iter23 = itersCnt;

    if(absMode){
        printf("Абсциссы точек пересечения: \n");
        printf("f1 и f2: %.5lf\n", rt12);
        printf("f1 и f3: %.5lf\n", rt13);
        printf("f2 и f3: %.5lf\n", rt23);
    }

    if(iterMode) {
        printf("Количество операций нахождения корня уравнения: \n");
        printf("f1 и f2: %d\n", iter12);
        printf("f1 и f3: %d\n", iter13);
        printf("f2 и f3: %d\n", iter23);
    }

    double intf1 = integral(f1, rt13, rt12, eps);
    double intf2 = integral(f2, rt23, rt12, eps);
    double intf3 = integral(f3, rt13, rt23, eps);
    double area = intf1 - (intf2 + intf3);
    return area;
}

//Функции печати текста

void PrintHelp() {
    printf("\nАргументы командной строки:\n");
    printf("\n");
    printf("1) -H или --help - вызвать описание аргументов, остальные аргументы отбрасываются\n");
    printf("\n");
    printf("2) -M или --mode - выбор режима работы программы.\n");
    printf("    Принимает целое число от 1 до 3. По умолчанию 1\n");
    printf("      1: вычисление площади фигуры, ограниченной кривыми. \n");
    printf("      2: нахождение корня уравнения f(x) = g(x) на отрезке [a, b].\n");
    printf("      3: вычисление определенного интеграла функции на отрезке [a, b].\n");
    printf("\n");
    printf("3) -F или --first - задать первую функцию для режимов 2 и 3.\n    Принимает целое значение от 1 до 3, обозначающее f1, f2, f3 соответственно.\n");
    printf("Не используется при режиме 1, значение по умолчанию не задано.\n");
    printf("\n");
    printf("3) -S или --second - задать вторую функцию для режима 2.\n    Принимает целое значение от 1 до 3, обозначающее f1, f2, f3 соответственно.\n");
    printf("Не используется при режиме 1 и 3, значение по умолчанию не задано.\n");
    printf("\n");
    printf("4) -A или --left - задать левую границу отрезка.\n    Принимает вещественное значение.\n");
    printf("Не используется при режиме 1, значение по умолчанию не задано.\n");
    printf("\n");
    printf("5) -B или --right - задать правую границу отрезка.\n    Принимает вещественное значение.\n");
    printf("Не используется при режиме 1, значение по умолчанию не задано.\n");
    printf("\n");
    printf("6) -EPS --epsilon - задать точность вычислений.\n    Принимает положительное вещественное значение.\n");
    printf("Значение по умолчанию для режима 1 0.001, иначе не задано.\n");
    printf("\n");
    printf("7) -V или --vertices - при добавлении этого аргумента будут выведены точки пересечений абсцисс.\n");
    printf("Срабатывает только при режиме 1.\n");
    printf("\n");
    printf("8) -I или --iterations - при добавлении этого аргумента будут выведены количества итераций для подсчета корней уравнений.\n");
    printf("Срабатывает только при режиме 1.\n");
    printf("\n");
    printf("\nПримеры:\n");
    printf("./main -M 3 -F 3 -A -3 -B 3 -EPS 0.00001\n");    
    printf("./main -I -V -M 1\n");
    printf("./main -V -I -M 2 --left 0 -F 2 --second 3.5 --right 1000000 --epsilon 0.00001");
    printf("\n");
}

void PrintHello()
{   
	printf("\n     ВЫЧИСЛЕНИЕ КОРНЕЙ И ОПРЕДЕЛЕННЫХ ИНТЕГРАЛОВ\n");
    printf("Сделал Минхузов Дамир, 105 группа\n");
    printf("Вариант 9 5 2\n");
    printf("\n");
    printf("Функции:\n");
    printf("f1(x) = 3 / ((x - 1) ^ 2  + 1),\n");
	printf("f2(x) = sqrt(x + 0.5),\n");
	printf("f3(x) = e^-x.\n");
    printf("Программа вычисляет корни уравнения f(x) = g(x) на отрезке с помощью методов хорд и отрезков;\n");
    printf("значение определенных интегралов на отрезке, площадь фигуры ограниченной тремя функциями.\n");
    printf("Для справки введите флаг -H или --help.\n");
    printf("\n");
    //PrintHelp();
}


int main(int argc, char **argv)
{
    if(argc == 1) {
        PrintHello();
        return 0;
    }

    // аргументы и флаги для их определения
    int modeFlag = 0;
    int mode = 1;

    int fFlag = 0;
    int fFunc;
    double (*f)(double) = NULL;
    int sFlag = 0;
    int sFunc;
    double (*g)(double) = NULL;

    int aFlag = 0;
    double aLeft;
    int bFlag = 0;
    double bRight;

    int epsFlag = 0;
    double eps = 0.001;

    int vFlag = 0;

    int iFlag = 0;

    //Парсинг аргументов

    for(int i = 1; i < argc; i++) {
        if(!strcmp(argv[i], "-H") || !strcmp(argv[i], "--help")) {
            PrintHelp();
            return 0;
        }
        if(!strcmp(argv[i], "-M") || !strcmp(argv[i], "--mode")) {
            if(modeFlag) {
                printf("Аргумент -M/--mode не может быть использован дважды.\n");
                return 0;
            }
            modeFlag = 1;
            i++;
            if(i >= argc || sscanf(argv[i], "%d", &mode) != 1) {
                printf("Аргумент -M/--mode принимает целое число от 1 до 3.\n");
                return 0;
            }
            if(mode < 1) {
                printf("Аргумент -M/--mode принимает число от 1.\n");
                return 0;
            }
            if(mode > 3) {
                printf("Аргумент -M/--mode принимает число до 3.\n");
                return 0;
            }
        } else if(!strcmp(argv[i], "-F") || !strcmp(argv[i], "--first")) {
            if(fFlag) {
                printf("Аргумент -F/--first не может быть использован дважды.\n");
                return 0;
            }
            fFlag = 1;
            i++;
            if(i >= argc || sscanf(argv[i], "%d", &fFunc) != 1) {
                printf("Аргумент -F/--first принимает целое число от 1 до 3.\n");
                return 0;
            }
            if(fFunc < 1) {
                printf("Аргумент -F/--first принимает число от 1.\n");
                return 0;
            }
            if(fFunc > 3) {
                printf("Аргумент -F/--first принимает число до 3.\n");
                return 0;
            }
            if(fFunc == 1) f = f1;
            if(fFunc == 2) f = f2;
            if(fFunc == 3) f = f3;
        } else if(!strcmp(argv[i], "-S") || !strcmp(argv[i], "--second")) {
            if(sFlag) {
                printf("Аргумент -S/--second не может быть использован дважды.\n");
                return 0;
            }
            sFlag = 1;
            i++;
            if(i >= argc || sscanf(argv[i], "%d", &sFunc) != 1) {
                printf("Аргумент -S/--second принимает целое число от 1 до 3.\n");
                return 0;
            }
            if(sFunc < 1) {
                printf("Аргумент -S/--second принимает число от 1.\n");
                return 0;
            }
            if(sFunc > 3) {
                printf("Аргумент -S/--second принимает число до 3.\n");
                return 0;
            }
            if(sFunc == 1) g = f1;
            if(sFunc == 2) g = f2;
            if(sFunc == 3) g = f3;
        }
        else if(!strcmp(argv[i], "-A") || !strcmp(argv[i], "--left")) {
            if(aFlag) {
                printf("Аргумент -A/--left не может быть использован дважды.\n");
                return 0;
            }
            aFlag = 1;
            i++;
            if(i >= argc || sscanf(argv[i], "%lf", &aLeft) != 1) {
                printf("Аргумент -A/--left принимает вещественное число.\n");
                return 0;
            }
        } else if(!strcmp(argv[i], "-B") || !strcmp(argv[i], "--right")) {
            if(bFlag) {
                printf("Аргумент -B/--right не может быть использован дважды.\n");
                return 0;
            }
            bFlag = 1;
            i++;
            if(i >= argc || sscanf(argv[i], "%lf", &bRight) != 1) {
                printf("Аргумент -B/--right принимает вещественное число.\n");
                return 0;
            }
        } else if(!strcmp(argv[i], "-EPS") || !strcmp(argv[i], "--epsilon")) {
            if(epsFlag) {
                printf("Аргумент -EPS/--epsilon не может быть использован дважды.\n");
                return 0;
            }
            epsFlag = 1;
            i++;
            if(i >= argc || sscanf(argv[i], "%lf", &eps) != 1) {
                printf("Аргумент -EPS/--epsilon принимает вещественное число.\n");
                return 0;
            }
            if(eps <= 0) {
                printf("Параметр --epsilon должен быть ПОЛОЖИТЕЛЬНЫМ.\n");
                return 0;
            }
        } else if(!strcmp(argv[i], "-V") || !strcmp(argv[i], "--verticies")) {
            if(vFlag) {
                printf("Аргумент -V/--verticies не может быть использован дважды.\n");
                return 0;
            }
            vFlag = 1;
        } else if(!strcmp(argv[i], "-I") || !strcmp(argv[i], "--iterations")) {
            if(iFlag) {
                printf("Аргумент -I/--iterations не может быть использован дважды.\n");
                return 0;
            }
            iFlag = 1;
        } else {
            printf("Ошибка в команде %s.\n", argv[i]);
            return 0;
        }
    }

    //проверка корректности введенных данных (они уже подходят по типу данных, но надо чтобы подходили под использование)
    //проверка функций
    if(mode != 1 && !fFlag) {
        printf("Введите параметр -F или --first.\n");
        return 0;
    }
    if(mode == 2 && !sFlag) {
        printf("Введите параметр -S или --second.\n");
        return 0;
    }
    if(mode != 1 && f == g) {
        printf("Параметры --first/-F и --second/-S должны различаться.\n");
        return 0;
    }
    //проверка отрезков
    if(mode != 1 && !aFlag) {
        printf("Введите параметр --left или -A.\n");
        return 0;
    }
    if(mode != 1 && !bFlag) {
        printf("Введите параметр --right или -B.\n");
        return 0;
    }
    if(mode != 1 && aLeft >= bRight) {
        printf("Правая точка должна быть больше левой.\n");
        return 0;
    }
    //првоерка точности
    if(mode != 1 && !epsFlag){
        printf("Введите параметр --epsilon или -EPS.\n");
        return 0;
    }

    //площадь

    if(mode == 1) {
        double res = area(vFlag, iFlag, eps);
        printf("Площадь фигуры ограниченной тремя кривыми равна %.5lf\n", res);
        return 0; 
    }
    if(mode == 2) {
        double res = root(f, g, aLeft, bRight, eps);
        if(isnan(res)){
            printf("Произошла ошибка при вычислении корня уравнения.\n");
            return 0;
        }
        printf("Корень уравнения f%d(x) = f%d(x) на отрезке [%.5lf, %.5lf] равен %.5lf\n", fFunc, sFunc, aLeft, bRight, res);
        return 0;
    }
    if(mode == 3) {
        double res = integral(f, aLeft, bRight, eps);
        if(isnan(res)){
            printf("Произошла ошибка при вычислении интеграла.\n");
            return 0;
        }
        printf("Значение определенного интеграла f%d(x) на отрезке [%.5lf, %.5lf] равен %.5lf\n", fFunc, aLeft, bRight, res);
        return 0;
    }
    /*some debug & test
    PrintHello();
    PrintHelp();
    printf("%.5lf\n", root(f1, f3, -2, 100, 0.001));
    printf("%.5lf\n", integral(f2, -0.3, 5, 0.0001));
    printf("%.5lf\n", area(1, 1, 0.001));*/
    return 0;
}
