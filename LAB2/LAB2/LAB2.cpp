// LAB2.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <string>
#include <cstdio>
#include <cstring>
#include <stdio.h>
#include <unordered_map>
#include <vector>
#include <set>
#include <locale.h>
#include <cstdlib>
#include <windows.h>

using namespace std;

class UserAndDepart // класс для задания связи (должности в структурном подразделении)
{
public:
	double stavka; // Ставка
	string dolg; // Должность
	string departName; // Название подразделения если это ключ 
	UserAndDepart(double st, string dol, string dep)
	{
		stavka = st;
		dolg = dol;
		departName = dep;
	}
};

class MyDB;

class MyUser // класс для сотрудников
{
public:
	static MyDB* db;
	string name; // ФИО, тоже ключ (при определенных условия)
	int year; // год рождения
	int ID; // Ключ - СНИЛС
	MyUser(string nam = "", int ye = 0, int id = -1)
	{
		name = nam;
		year = ye;
		ID = id;
	}

	~MyUser()
	{
		svyzi.clear();
	}

	void print()
	{
		cout << "ФИО: " << name + "; Год рождения: " + to_string(year) + "; СНИЛС: " + to_string(ID) << endl;
		cout << "Должности:" << endl;
		for (UserAndDepart* pos : svyzi)
		{
			cout << pos->dolg + "; " + pos->departName + "; ставка: " + to_string(pos->stavka) << endl;
		}

	}
	void ToDolg(string depart, string dolg_Name, double stav); // назначить на должность
	vector<UserAndDepart*> svyzi; // Связи пользователя с подразделениями
};

class MyDepartment // класс для структурных подразделений
{
public:
	set<string> sotr;  // Список сотрудников  
	string name; // Ключ - название
	string story; // Краткое опиание
	MyDepartment(string _name = "", string _story = "")
	{
		name = _name; story = _story;
	}
	void print()
	{
		cout << " Название: " << name << "; Описание: " + story << endl;
		cout << "Cотрудники подразделения:" << endl;
		for (string pos : sotr)
			cout << pos << endl;
	}
};

class MyDB // класс для базы данных
{
public:
	unordered_map<string, MyUser*> users_Name; // сотрудники, по ФИО
	unordered_map<int, MyUser*> users_ID; // сотрудники, по СНИЛС
	unordered_map<string, MyDepartment*> departs; // структурные подразделения
	void addUser(MyUser* user) // добавить сотрудника
	{
		if (findUserById(user->ID) || findUserByName(user->name))
		{
			cout << "Уже существует сотрудник с данным ФИО или СНИЛС." << endl;
			return;
		}
		users_Name[user->name] = user;
		users_ID[user->ID] = user;
		cout << "Сотрудник добавлен." << endl;
	}
	void addDepart(MyDepartment* depart) // добавить структурное подразделение
	{
		if (findDepartByName(depart->name))
		{
			cout << "Уже существует структурное подразделение с данным названием." << endl;
			return;
		}
		departs[depart->name] = depart;
		cout << "Структурное подразделение добавлено." << endl;
	}

	MyDB() {}

	// вывести все структурные подразделения
	void print_Depart()
	{
		// Отображаем элементы
		for (auto it = departs.begin(); it != departs.end(); ++it)
		{
			it->second->print();
		}
	}

	// вывести всех сотрудников
	void print_UserName()
	{
		// Отображаем элементы
		for (auto it = users_Name.begin(); it != users_Name.end(); ++it)
		{
			it->second->print();
		}
	}

	//- поиск полных данных о сотруднике (сотрудниках) (поиск по	ФИО);
	MyUser* findUserByName(string name)
	{
		auto it = users_Name.find(name);

		if (it == users_Name.end())
		{
			return nullptr;
		}
		else
		{
			return it->second;
		}
	}

	//- вывод полных данных о сотруднике (сотрудниках) (поиск по	ФИО);
	void printUserByName(string name)
	{
		cout << "Поиск сотрудника по ФИО: " << name << endl;
		MyUser* user = findUserByName(name);

		if (!user)
		{
			cout << "Не удалось найти сотрудника с заданным ФИО" << endl;
		}
		else
		{
			user->print();
		}
	}

	//- поиск полных данных о сотруднике(сотрудниках) (поиск по СНИЛС);
	MyUser* findUserById(int id)
	{
		auto it = users_ID.find(id);

		if (it == users_ID.end())
		{
			return nullptr;
		}
		else
		{
			return it->second;
		}
	}

	//- вывод полных данных о сотруднике(сотрудниках) (поиск по СНИЛС);
	void printUserById(int id)
	{
		cout << "Поиск сотрудника по СНИЛС: " << id << endl;
		MyUser* user = findUserById(id);

		if (!user)
		{
			cout << "Не удалось найти сотрудника с заданным СНИЛС" << endl;
		}
		else
		{
			user->print();
		}
	}

	//- поиск по СНИЛС и изменение некоторых данных о сотруднике (изменение ФИО, должности в структурном подразделении);
	void changeNameById(int id, string newName)
	{
		MyUser* user = findUserById(id);
		if (!user)
		{
			cout << "Не удалось найти сотрудника с заданным СНИЛС" << endl;
		}
		else
		{
			MyUser* user2 = findUserByName(newName);
			if (user2)
			{
				cout << "Сотрудник с заданным ФИО уже существует." << endl;
			}
			else
			{
				cout << "ФИО сотрудника изменено. Старое ФИО: " << user->name << ", новое ФИО: " << newName << endl;
				user->name = newName;
			}
		}
	}

	//- поиск по СНИЛС и изменение ставки некоторой должности в некотором структурном подразделении;
	void changeDolgById(int id, string depart, string dolg_Name, double newStav)
	{
		MyUser* user = findUserById(id);
		if (!user)
		{
			cout << "Не удалось найти сотрудника с заданным СНИЛС" << endl;
		}
		else
		{
			cout << "ФИО сотрудника: " << user->name << endl;
			UserAndDepart* svyz = nullptr;
			for (auto sv : user->svyzi)
			{
				if (sv->departName == depart && sv->dolg == dolg_Name)
				{
					svyz = sv;
					break;
				}
			}

			if (!svyz)
			{
				cout << "Не найденно соответсвующей должности" << endl;
			}
			else
			{
				cout << "Структурное подразделение: " << depart << ", должность: " << dolg_Name << endl;
				cout << "Ставка изменена. Старая ставка: " << svyz->stavka << ", новая ставка: " << newStav << endl;
				svyz->stavka = newStav;
			}
		}
	}
	//-поиск по СНИЛС и удаление сотрудника из БД;
	void deleteUserById(int id)
	{

		MyUser* user = findUserById(id);
		if (!user)
		{
			cout << "Не удалось найти сотрудника с заданным СНИЛС" << endl;
		}
		else
		{
			users_Name.erase(user->name);
			users_ID.erase(id);
			for (auto sv : user->svyzi)
			{
				departs[sv->departName]->sotr.erase(user->name);
			}
			delete user;
			cout << "Сотрудник удалён" << endl;
		}
	}

	//-поиск по названию структурного подразделения;
	MyDepartment* findDepartByName(string depart_name)
	{
		auto it = departs.find(depart_name);

		if (it == departs.end())
		{
			return nullptr;
		}
		else
		{
			return it->second;
		}
	}

	//-вывод структурного подразделения;
	void printDepartByName(string depart_name)
	{
		cout << "Поиск структурного подразделения по названию: " << depart_name << endl;
		MyDepartment* dep = findDepartByName(depart_name);

		if (!dep)
		{
			cout << "Не удалось найти структурного подразделения с заданным названием" << endl;
		}
		else
		{
			dep->print();
		}
	}

	//-поиск по названию и изменение некоторых данных о структурном	подразделении (новое название и краткое описание);
	void changeDepartByName(string depart_name, string new_name, string new_story)
	{
		MyDepartment* dep = findDepartByName(depart_name);
		if (!dep)
		{
			cout << "Не удалось найти структурного подразделения с заданным названием" << endl;
		}
		else
		{
			cout << "Структурное подразделение: " << depart_name << endl;
			cout << "Новое название: " << new_name << ", новое описание: " << new_story << endl;
			dep->name = new_name;
			dep->story = new_story;
		}
	}
};

// Сотрудника назначить на должность
void MyUser::ToDolg(string depart, string dolg_Name, double stav)
{
	if (!db->findDepartByName(depart))
	{
		cout << "Подразделения с заданным названием не существует." << endl;
	}
	svyzi.push_back(new UserAndDepart(stav, dolg_Name, depart));
	db->departs[depart]->sotr.insert(name);
}

MyDB* MyUser::db = new MyDB();

void print_help()
{
	cout << endl << "Команды:" << endl;
	cout << " 1: Добавить нового сотрудника" << endl;
	cout << " 2: Добавить новое структурное подразделение" << endl;
	cout << " 3: Назначить на должность сотрудника" << endl;
	cout << " 4: Вывести всех сотрудников" << endl;
	cout << " 5: Вывести все структурные подразделения" << endl;
	cout << " 6: Вывести сотрудника по ФИО" << endl;
	cout << " 7: Вывести сотрудника по СНИЛС" << endl;
	cout << " 8: Найти сотрудника по СНИЛС и изменить его ФИО" << endl;
	cout << " 9: Изменить ставку должности сотрудника" << endl;
	cout << " 10: Найти сотрудника по СНИЛС и удалить его из базы" << endl;
	cout << " 11: Вывести структурное подразделение по названию" << endl;
	cout << " 12: Изменить название и краткое описание структурного подразделения" << endl;
	cout << " 0: Выход" << endl;
	cout << endl;
}

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	setlocale(LC_ALL, "Russian");
	MyDB* db = MyUser::db;
	int cmd, year, id;
	double stavka;
	string name, depart_name, depart_name2, story, dolg_name;
	MyUser* newUser = nullptr;
	MyDepartment* newDepart = nullptr;

	do
	{
		print_help();
		cout << "**************************************************" << endl;
		cout << "Введите команду: ";                  // Считать команду
		cin >> cmd;
		switch (cmd)
		{
		case 0:
			break;
		case 1:
			cout << "Режим: Добавление нового сотрудника..." << endl;
			cout << "Введите ФИО сотрудника: ";
			cin.ignore();
			getline(cin, name);
			cout << "Введите год рождения сотрудника: ";
			cin >> year;
			cout << "Введите СНИЛС сотрудника: ";
			cin >> id;
			newUser = new MyUser(name, year, id);
			db->addUser(newUser);
			break;
		case 2:
			cout << "Режим: Добавление структурного подразделения..." << endl;
			cout << "Введите название структурного подразделения: ";
			cin.ignore();
			getline(cin, depart_name);
			cout << "Введите краткое описание структурного подразделения: ";
			//cin.ignore();
			getline(cin, story);
			newDepart = new MyDepartment(depart_name, story);
			db->addDepart(newDepart);
			break;
		case 3:
			cout << "Режим: Назначение на должность сотрудника..." << endl;
			cout << "Введите СНИЛС сотрудника: ";
			cin >> id;
			cout << "Введие название структурного подразделения: ";
			cin.ignore();
			getline(cin, depart_name);
			cout << "Введие название должности: ";
			//cin.ignore();
			getline(cin, dolg_name);
			cout << "Введите ставку: ";
			cin >> stavka;
			newUser = db->findUserById(id);
			if (!newUser)
			{
				cout << "Не удалось найти сотрудника с заданным СНИЛС" << endl;
			}
			else
			{
				newUser->ToDolg(depart_name, dolg_name, stavka);
			}
			break;
		case 4:
			cout << "Режим: Вывод всех сотрудников..." << endl;
			db->print_UserName();
			break;
		case 5:
			cout << "Режим: Вывод всех структурных подразделений..." << endl;
			db->print_Depart();
			break;
		case 6:
			cout << "Режим: Вывод сотрудника по ФИО..." << endl;
			cout << "Введите ФИО сотрудника: ";
			cin.ignore();
			getline(cin, name);
			db->printUserByName(name);
			break;
		case 7:
			cout << "Режим: Вывод сотрудника по СНИЛС..." << endl;
			cout << "Введите СНИЛС сотрудника: ";
			cin >> id;
			db->printUserById(id);
			break;
		case 8:
			cout << "Режим: Поиск сотрудника по СНИЛС и изменение ФИО..." << endl;
			cout << "Введите СНИЛС сотрудника: ";
			cin >> id;
			cout << "Введите новое ФИО сотрудника: ";
			cin.ignore();
			getline(cin, name);
			db->changeNameById(id, name);
			break;
		case 9:
			cout << "Режим: Изменение ставки должности сотрудника..." << endl;
			cout << "Введите СНИЛС сотрудника: ";
			cin >> id;
			cout << "Введие название структурного подразделения: ";
			cin.ignore();
			getline(cin, depart_name);
			cout << "Введие название должности: ";
			//cin.ignore();
			getline(cin, dolg_name);
			cout << "Введите новую ставку: ";
			cin >> stavka;
			db->changeDolgById(id, depart_name, dolg_name, stavka);
			break;
		case 10:
			cout << "Режим: Удаление сотрудника..." << endl;
			cout << "Введите СНИЛС сотрудника: ";
			cin >> id;
			db->deleteUserById(id);
			break;
		case 11:
			cout << "Режим: Вывод структурного подразделения..." << endl;
			cout << "Введие название структурного подразделения: ";
			cin.ignore();
			getline(cin, depart_name);
			db->printDepartByName(depart_name);
			break;
		case 12:
			cout << "Режим: Изменение структурного подразделения..." << endl;
			cout << "Введие название структурного подразделения: ";
			cin.ignore();
			getline(cin, depart_name);
			cout << "Введие новое название структурного подразделения: ";
			//cin.ignore();
			getline(cin, depart_name2);
			cout << "Введие новое краткое описание структурного подразделения: ";
			//cin.ignore();
			getline(cin, story);
			db->changeDepartByName(depart_name, depart_name2, story);
			break;
		default:                               // Неверная команда
			cout << "Неправильная команда. Попробуйте снова." << endl;
		}
		cout << "**************************************************" << endl;
	} while (cin && (cmd != 0));

	return 0;
}
