#pragma once
#include "Server.h"
#include "pch.h"
class Client //Класс для клиентской части
{
public:
	Client(MyDB* _db); // конструктор, принимающий базу данных
	void startSession(); // старт ввода информации пользователем
private:
	void print_help(); // функция с командами для пользователя
	MyDB* db; // база данных, к которой будет обращаться клиент
};
