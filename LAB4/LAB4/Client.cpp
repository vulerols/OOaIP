#include "Client.h"
#include <iostream>
#include <locale.h>
#include <windows.h>
#include "pch.h"

Client::Client(MyDB* _db)
{
	db = _db;
}

void Client::startSession()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	setlocale(LC_ALL, "Russian");
	string delimiter = "$$";
	int cmd, count;
	double price;
	string name, product_name, descr, password;
	string query;
	do
	{
		print_help();
		cout << "**************************************************" << endl;
		cout << "Введите команду: ";                  // Считать команду
		cin >> cmd;
		switch (cmd)
		{
		case 0:
			query = "";
			break;
		case 1:
			cout << "Режим: Регистрация нового пользователя..." << endl;
			cout << "Введите логин пользователя: ";
			cin.ignore();
			getline(cin, name);
			cout << "Введите пароль: ";
			getline(cin, password);
			cout << "Если пользователь будет являться администратором введите 1, иначе - 0: ";
			cin >> count;
			query = "1" + delimiter + name + delimiter + password + delimiter + to_string(count) + delimiter;
			break;
		case 2:
			cout << "Режим: Добавление нового товара..." << endl;
			cout << "Введите название товара: ";
			cin.ignore();
			getline(cin, product_name);
			cout << "Введите стоимость товара: ";
			cin >> price;
			cout << "Введите количество товара: ";
			cin >> count;
			cout << "Введите краткое описание товара: ";
			cin.ignore();
			getline(cin, descr);
			query = "2" + delimiter + product_name + delimiter + to_string(price) + delimiter + to_string(count) + delimiter + descr + delimiter;
			break;
		case 3:
			cout << "Режим: Обновление информации о товаре..." << endl;
			cout << "Введите название товара: ";
			cin.ignore();
			getline(cin, product_name);
			cout << "Введите новую стоимость товара: ";
			cin >> price;
			cout << "Введите новое количество товара: ";
			cin >> count;
			cout << "Введите новое краткое описание товара: ";
			cin.ignore();
			getline(cin, descr);
			query = "3" + delimiter + product_name + delimiter + to_string(price) + delimiter + to_string(count) + delimiter + descr + delimiter;
			break;
		case 4:
			cout << "Режим: Вход пользователя..." << endl;
			cout << "Введите Ваш логин: ";
			cin.ignore();
			getline(cin, name);
			cout << "Введите пароль: ";
			getline(cin, password);
			query = "4" + delimiter + name + delimiter + password + delimiter;
			break;
		case 5:
			cout << "Режим: Просмотр всех товаров (краткие данные)..." << endl;
			query = "5" + delimiter;
			break;
		case 6:
			cout << "Режим: Просмотр подробной информации о товаре..." << endl;
			cout << "Введите название товара: ";
			cin.ignore();
			getline(cin, product_name);
			query = "6" + delimiter + product_name + delimiter;
			break;
		case 7:
			cout << "Режим: Создание заказа..." << endl;
			query = "7" + delimiter;
			break;
		case 8:
			cout << "Режим: Добавление товара в заказ..." << endl;
			cout << "Введите название товара: ";
			cin.ignore();
			getline(cin, product_name);
			cout << "Введите количество: ";
			cin >> count;
			query = "8" + delimiter + product_name + delimiter + to_string(count) + delimiter;
			break;
		case 9:
			cout << "Режим: Просмотр заказа..." << endl;
			query = "9" + delimiter;
			break;
		case 10:
			cout << "Режим: Оплата заказа..." << endl;
			cout << "Введите предлагаемое количество денег: ";
			cin >> price;
			query = "10" + delimiter + to_string(price) + delimiter;
			break;
		default:                               // Неверная команда
			cout << "Неправильная команда. Попробуйте снова." << endl;
			query = "";
		}
		if (query != "")
		{
			cout << "***********************" << endl;
			cout << "Запрос клиента: " << endl << query << endl;
			cout << "***********************" << endl;
			cout << "Ответ сервера: " << endl << db->runQuery(query) << endl;
			cout << "***********************" << endl;
		}
		cout << "**************************************************" << endl;
	} while (cin && (cmd != 0));
}

void Client::print_help()
{
	cout << endl << "Команды:" << endl;
	cout << " 1: Регистрация нового пользователя" << endl;
	cout << " 2: Добавить новый товар" << endl;
	cout << " 3: Обновить информацию о товаре" << endl;
	cout << " 4: Вход пользователя" << endl;
	cout << " 5: Просмотр всех товаров (краткие данные)" << endl;
	cout << " 6: Просмотр подробной информации о товаре" << endl;
	cout << " 7: Создание заказа" << endl;
	cout << " 8: Добавление товара в заказ" << endl;
	cout << " 9: Просмотр заказа" << endl;
	cout << " 10: Оплата заказа" << endl;
	cout << " 0: Выход" << endl;
	cout << endl;
}
