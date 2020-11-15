//mutex (пропускает 1 по очереди)
//barrier (ждет k потоков (счетчик))

#include <iostream>
#include <string>
#include <thread>
#include <vector>


const double STEP = 0.01;//Шаг
double a;//Нижняя граница
double b;//Верхняя граница

///Функция для интегрирования
double f(double x) {
    return sin(x);
}

///Метод для считывания кол-ва потоков для использования [int]
int getIntValue(const std::string &mes, int max) {
    int k;//Кол-во потоков
    std::string str;//Вводимая строка
    bool flag = true;
    while (flag) // цикл продолжается до тех пор, пока пользователь не введет корректное значение
    {
        try {
            std::cout << mes;//Сообщение для пользователя
            std::cin >> str;
            k = std::stoi(str);//Перевод string в int
            flag = false;//Если исключение не выброшено

        } catch (std::exception e) {
            std::cout << e.what();
        }
    }
    //Проверка на корректность диапозона [1,max]
    if (k <= 0 || k > max) {
        std::cout << "Incorrect number!";
        return getIntValue(mes, max);
    }

    return k;
}

///Считывание одной границы интегрирования [double]
double getDoubleValue(const std::string &mes) {
    double k;//Граница
    std::string str;//Вводимая строка
    while (true) // цикл продолжается до тех пор, пока пользователь не введет корректное значение
    {
        try {
            std::cout << mes;//Сообщение для пользователя
            std::cin >> str;
            k = std::stod(str);//ПЕревод string -> double (либо эксепшн)

            return k;
        } catch (std::exception e) {
            std::cout << e.what();
        }
    }
}

///Считывание пределов интегрирования a b
void ReadAB() {

    a = getDoubleValue("Input double a:\n");
    b = getDoubleValue("Input double b:\n");

    //В случае некорректного порялка меняем местами границы
    if (a > b) {
        std::swap(a, b);
    }
}

//стартовая функция для дочерних потоков
//номер потока, кол-во потоков, сумма для одного потока
void integral(int iTread, int iTN, double &sum) {
    for (double i = a + iTread * STEP; i < b; i += iTN * STEP) {//i+кол-во_потоков* шаг [начинаем №потока*STEP]
        sum += STEP * (f(i) + f(i + STEP)) / 2;//шаг*h//Принцип Дихотомии
    }
}

//Итеративный параллелизм
int main() {

    //ВВод границ
    ReadAB();
    //Определение количество потоков
    int max = static_cast<int>((a + b) / STEP ) + 1;
    std::string mes = {"Enter the desired number of threads to use in the range [1,"};
    mes += std::to_string(max) + "]..";
    int threadNumber = getIntValue(mes, max);
    std::vector<std::thread> thr;//Вектор потоков
    thr.reserve(threadNumber);//Кол-во потоков
    //std::thread *thr=new std::thread [threadNumber];
    auto *sum = new double[threadNumber];

    std::cout << "Upper bound: " << b << "\nLow bound: " << a << std::endl;
    std::cout << "Upper bound: " << b << "\nLow bound: " << a << std::endl;
    std::cout << "Step: " << STEP << std::endl;
    std::cout << "Number of threads: " << threadNumber << std::endl;


    auto begin = std::chrono::steady_clock::now();
    //Создание потоков
    for (int i = 0; i < threadNumber; i++) {
        sum[i] = 0;
        thr.emplace_back(std::thread{integral, i, threadNumber, std::ref(sum[i])});//Инициализируем поток[i]
    }

    double rez = 0; //для результата
    // Завершение потоков
    for (int i = 0; i < threadNumber; i++) {
        thr[i].join();
        rez += sum[i];//Суммируем суммы посчитанные одним потоком
    }

    auto end = std::chrono::steady_clock::now();
    std::cout << std::endl;
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    std::cout << "milliseconds: " << elapsed_ms.count() << std::endl;
    printf("Result is: %.3f", rez);

    return 0;

}

