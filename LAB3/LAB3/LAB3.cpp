// LAB3.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
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
#include <utility>

using namespace std;

const string AllowedChars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz#@$^*()"; // символы для генерации солей
const int saltLength = 8; //длина строки для соли

class MD5 // класс для хэширования паролей 
{
public:
	typedef unsigned int size_type; // must be 32bit

	MD5();
	MD5(const std::string& text);
	void update(const unsigned char* buf, size_type length);
	void update(const char* buf, size_type length);
	MD5& finalize();
	std::string hexdigest() const;
	friend std::ostream& operator<<(std::ostream&, MD5 md5);

private:
	void init();
	typedef unsigned char uint1; //  8bit
	typedef unsigned int uint4;  // 32bit
	enum { blocksize = 64 }; // VC6 won't eat a const static int here

	void transform(const uint1 block[blocksize]);
	static void decode(uint4 output[], const uint1 input[], size_type len);
	static void encode(uint1 output[], const uint4 input[], size_type len);

	bool finalized;
	uint1 buffer[blocksize]; // bytes that didn't fit in last 64 byte chunk
	uint4 count[2];   // 64bit counter for number of bits (lo, hi)
	uint4 state[4];   // digest so far
	uint1 digest[16]; // the result

	// low level logic operations
	static inline uint4 F(uint4 x, uint4 y, uint4 z);
	static inline uint4 G(uint4 x, uint4 y, uint4 z);
	static inline uint4 H(uint4 x, uint4 y, uint4 z);
	static inline uint4 I(uint4 x, uint4 y, uint4 z);
	static inline uint4 rotate_left(uint4 x, int n);
	static inline void FF(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
	static inline void GG(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
	static inline void HH(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
	static inline void II(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
};

std::string md5(const std::string str);

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

///////////////////////////////////////////////

// F, G, H and I are basic MD5 functions.
inline MD5::uint4 MD5::F(uint4 x, uint4 y, uint4 z) {
	return x & y | ~x & z;
}

inline MD5::uint4 MD5::G(uint4 x, uint4 y, uint4 z) {
	return x & z | y & ~z;
}

inline MD5::uint4 MD5::H(uint4 x, uint4 y, uint4 z) {
	return x ^ y ^ z;
}

inline MD5::uint4 MD5::I(uint4 x, uint4 y, uint4 z) {
	return y ^ (x | ~z);
}

// rotate_left rotates x left n bits.
inline MD5::uint4 MD5::rotate_left(uint4 x, int n) {
	return (x << n) | (x >> (32 - n));
}

// FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
// Rotation is separate from addition to prevent recomputation.
inline void MD5::FF(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac) {
	a = rotate_left(a + F(b, c, d) + x + ac, s) + b;
}

inline void MD5::GG(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac) {
	a = rotate_left(a + G(b, c, d) + x + ac, s) + b;
}

inline void MD5::HH(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac) {
	a = rotate_left(a + H(b, c, d) + x + ac, s) + b;
}

inline void MD5::II(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac) {
	a = rotate_left(a + I(b, c, d) + x + ac, s) + b;
}

//////////////////////////////////////////////

// default ctor, just initailize
MD5::MD5()
{
	init();
}

//////////////////////////////////////////////

// nifty shortcut ctor, compute MD5 for string and finalize it right away
MD5::MD5(const std::string& text)
{
	init();
	update(text.c_str(), text.length());
	finalize();
}

//////////////////////////////

void MD5::init()
{
	finalized = false;

	count[0] = 0;
	count[1] = 0;

	// load magic initialization constants.
	state[0] = 0x67452301;
	state[1] = 0xefcdab89;
	state[2] = 0x98badcfe;
	state[3] = 0x10325476;
}

//////////////////////////////

// decodes input (unsigned char) into output (uint4). Assumes len is a multiple of 4.
void MD5::decode(uint4 output[], const uint1 input[], size_type len)
{
	for (unsigned int i = 0, j = 0; j < len; i++, j += 4)
		output[i] = ((uint4)input[j]) | (((uint4)input[j + 1]) << 8) |
		(((uint4)input[j + 2]) << 16) | (((uint4)input[j + 3]) << 24);
}

//////////////////////////////

// encodes input (uint4) into output (unsigned char). Assumes len is
// a multiple of 4.
void MD5::encode(uint1 output[], const uint4 input[], size_type len)
{
	for (size_type i = 0, j = 0; j < len; i++, j += 4) {
		output[j] = input[i] & 0xff;
		output[j + 1] = (input[i] >> 8) & 0xff;
		output[j + 2] = (input[i] >> 16) & 0xff;
		output[j + 3] = (input[i] >> 24) & 0xff;
	}
}

//////////////////////////////

// apply MD5 algo on a block
void MD5::transform(const uint1 block[blocksize])
{
	uint4 a = state[0], b = state[1], c = state[2], d = state[3], x[16];
	decode(x, block, blocksize);

	/* Round 1 */
	FF(a, b, c, d, x[0], S11, 0xd76aa478); /* 1 */
	FF(d, a, b, c, x[1], S12, 0xe8c7b756); /* 2 */
	FF(c, d, a, b, x[2], S13, 0x242070db); /* 3 */
	FF(b, c, d, a, x[3], S14, 0xc1bdceee); /* 4 */
	FF(a, b, c, d, x[4], S11, 0xf57c0faf); /* 5 */
	FF(d, a, b, c, x[5], S12, 0x4787c62a); /* 6 */
	FF(c, d, a, b, x[6], S13, 0xa8304613); /* 7 */
	FF(b, c, d, a, x[7], S14, 0xfd469501); /* 8 */
	FF(a, b, c, d, x[8], S11, 0x698098d8); /* 9 */
	FF(d, a, b, c, x[9], S12, 0x8b44f7af); /* 10 */
	FF(c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
	FF(b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
	FF(a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
	FF(d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
	FF(c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
	FF(b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

	/* Round 2 */
	GG(a, b, c, d, x[1], S21, 0xf61e2562); /* 17 */
	GG(d, a, b, c, x[6], S22, 0xc040b340); /* 18 */
	GG(c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
	GG(b, c, d, a, x[0], S24, 0xe9b6c7aa); /* 20 */
	GG(a, b, c, d, x[5], S21, 0xd62f105d); /* 21 */
	GG(d, a, b, c, x[10], S22, 0x2441453); /* 22 */
	GG(c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
	GG(b, c, d, a, x[4], S24, 0xe7d3fbc8); /* 24 */
	GG(a, b, c, d, x[9], S21, 0x21e1cde6); /* 25 */
	GG(d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
	GG(c, d, a, b, x[3], S23, 0xf4d50d87); /* 27 */
	GG(b, c, d, a, x[8], S24, 0x455a14ed); /* 28 */
	GG(a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
	GG(d, a, b, c, x[2], S22, 0xfcefa3f8); /* 30 */
	GG(c, d, a, b, x[7], S23, 0x676f02d9); /* 31 */
	GG(b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

	/* Round 3 */
	HH(a, b, c, d, x[5], S31, 0xfffa3942); /* 33 */
	HH(d, a, b, c, x[8], S32, 0x8771f681); /* 34 */
	HH(c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
	HH(b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
	HH(a, b, c, d, x[1], S31, 0xa4beea44); /* 37 */
	HH(d, a, b, c, x[4], S32, 0x4bdecfa9); /* 38 */
	HH(c, d, a, b, x[7], S33, 0xf6bb4b60); /* 39 */
	HH(b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
	HH(a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
	HH(d, a, b, c, x[0], S32, 0xeaa127fa); /* 42 */
	HH(c, d, a, b, x[3], S33, 0xd4ef3085); /* 43 */
	HH(b, c, d, a, x[6], S34, 0x4881d05); /* 44 */
	HH(a, b, c, d, x[9], S31, 0xd9d4d039); /* 45 */
	HH(d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
	HH(c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
	HH(b, c, d, a, x[2], S34, 0xc4ac5665); /* 48 */

	/* Round 4 */
	II(a, b, c, d, x[0], S41, 0xf4292244); /* 49 */
	II(d, a, b, c, x[7], S42, 0x432aff97); /* 50 */
	II(c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
	II(b, c, d, a, x[5], S44, 0xfc93a039); /* 52 */
	II(a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
	II(d, a, b, c, x[3], S42, 0x8f0ccc92); /* 54 */
	II(c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
	II(b, c, d, a, x[1], S44, 0x85845dd1); /* 56 */
	II(a, b, c, d, x[8], S41, 0x6fa87e4f); /* 57 */
	II(d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
	II(c, d, a, b, x[6], S43, 0xa3014314); /* 59 */
	II(b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
	II(a, b, c, d, x[4], S41, 0xf7537e82); /* 61 */
	II(d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
	II(c, d, a, b, x[2], S43, 0x2ad7d2bb); /* 63 */
	II(b, c, d, a, x[9], S44, 0xeb86d391); /* 64 */

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;

	// Zeroize sensitive information.
	memset(x, 0, sizeof x);
}

// MD5 block update operation. Continues an MD5 message-digest
// operation, processing another message block
void MD5::update(const unsigned char input[], size_type length)
{
	// compute number of bytes mod 64
	size_type index = count[0] / 8 % blocksize;

	// Update number of bits
	if ((count[0] += (length << 3)) < (length << 3))
		count[1]++;
	count[1] += (length >> 29);

	// number of bytes we need to fill in buffer
	size_type firstpart = 64 - index;

	size_type i;

	// transform as many times as possible.
	if (length >= firstpart)
	{
		// fill buffer first, transform
		memcpy(&buffer[index], input, firstpart);
		transform(buffer);

		// transform chunks of blocksize (64 bytes)
		for (i = firstpart; i + blocksize <= length; i += blocksize)
			transform(&input[i]);

		index = 0;
	}
	else
		i = 0;

	// buffer remaining input
	memcpy(&buffer[index], &input[i], length - i);
}

//////////////////////////////

// for convenience provide a verson with signed char
void MD5::update(const char input[], size_type length)
{
	update((const unsigned char*)input, length);
}

//////////////////////////////

// MD5 finalization. Ends an MD5 message-digest operation, writing the
// the message digest and zeroizing the context.
MD5& MD5::finalize()
{
	static unsigned char padding[64] = {
	  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

	if (!finalized) {
		// Save number of bits
		unsigned char bits[8];
		encode(bits, count, 8);

		// pad out to 56 mod 64.
		size_type index = count[0] / 8 % 64;
		size_type padLen = (index < 56) ? (56 - index) : (120 - index);
		update(padding, padLen);

		// Append length (before padding)
		update(bits, 8);

		// Store state in digest
		encode(digest, state, 16);

		// Zeroize sensitive information.
		memset(buffer, 0, sizeof buffer);
		memset(count, 0, sizeof count);

		finalized = true;
	}

	return *this;
}

//////////////////////////////

// return hex representation of digest as string
std::string MD5::hexdigest() const
{
	if (!finalized)
		return "";

	char buf[33];
	for (int i = 0; i < 16; i++)
		sprintf_s(buf + i * 2, 33 - i * 2, "%02x", digest[i]);
	buf[32] = 0;

	return std::string(buf);
}

//////////////////////////////

std::ostream& operator<<(std::ostream& out, MD5 md5)
{
	return out << md5.hexdigest();
}

//////////////////////////////

std::string md5(const std::string str)
{
	MD5 md5 = MD5(str);

	return md5.hexdigest();
}

class Order // Класс для заказов
{
public:
	Order()
	{
		isPaid = false;
		total_Price = 0;
	}

	~Order()
	{
		clear();
	}

	void clear()
	{
		products.clear();
		isPaid = false;
	}
	unordered_map<string, int> products;  // Список товаров и их количеств
	bool isPaid;  // оплачен ли заказ
	double total_Price; // текущая общая стоимость заказа
};

class User // Класс для пользователей
{
public:
	User(string _login, string _password) //констркутор класса по логину и паролю
	{
		login = _login;
		salt = "";
		for (int i = 0; i < saltLength; ++i)
		{
			salt += AllowedChars[rand() % AllowedChars.length()];
		}
		password = md5(_password + salt); // соединяем пароль с солью и хэшируем с помощью md5
		current_order = new Order();
	}

	~User()
	{
		delete current_order;
	}

	string getKey() const //возвращаем в качестве ключа - логин пользователя
	{
		return login;
	}

	void setPassword(string oldPassword, string newPassword) // если мы введем старый пароль, то меняет пароль на новый
	{
		if (tryPassword(oldPassword)) // проверяем введенный пароль
		{
			password = md5(newPassword + salt); // проверка пройдена и можно менять пароль на новый
		}
	}

	bool tryPassword(string pass) const // возвращает true, если введен правильный пароль
	{
		return password == md5(pass + salt); // если хэшированный истинный пароль с солью равен хешированному введеному паролю с солью
	}

	Order* current_order;  // текущий заказ пользователя

private:
	string login; // Храним логин
	string password; // Храним пароль в private в хэшированном виде с солью
	string salt; // Храним случайную соль для каждого пользователя
};

class Product  // Класс для товаров
{
public:
	Product(string _name = "", double _price = 0, int _count = 0, string _description = "")
	{
		name = _name;
		price = _price;
		count = _count;
		description = _description;
	}

	void print_short()
	{
		cout << " Название товара: " << name << "; Стоимость: " << price << "; Кол-во: " << count << endl;
	}

	void print_full()
	{
		cout << " Название товара: " << name << "; Стоимость: " << price << "; Кол-во: " << count << "; Краткое описание: " << description << endl;
	}

	string name; // Название товара
	double price; // Цена
	int count;  // Количество на складе
	string description; // Описание
};

class MyDB // класс для базы данных
{
public:
	unordered_map<string, User*> users_login; // пользователи по логину
	unordered_map<string, Product*> products_name; // товары по названию
	User* logged_user;    // текущий пользователь, который залогинился
	MyDB()
	{
		logged_user = nullptr;
	}

	void addUser(User* user) // добавить пользователя
	{
		if (findUserByLogin(user->getKey()))
		{
			cout << "Уже существует пользователь с данным логином." << endl;
			return;
		}
		users_login[user->getKey()] = user;
		cout << "Пользователь добавлен." << endl;
	}

	User* findUserByLogin(string name) // найти пользователя по логину
	{
		auto it = users_login.find(name);

		if (it == users_login.end())
		{
			return nullptr;
		}
		else
		{
			return it->second;
		}
	}

	void addProduct(Product* product) // добавить товар
	{
		if (findProductByName(product->name))
		{
			cout << "Уже существует товар с данным названием." << endl;
			return;
		}
		products_name[product->name] = product;
		cout << "Товар добавлен." << endl;
	}

	void updateProduct(string name, double new_price, int new_count, string new_description) // обновить информацию о товаре
	{
		if (!findProductByName(name))
		{
			cout << "Не существует товара с данным названием." << endl;
			return;
		}
		Product* product = products_name[name];
		product->price = new_price;
		product->count = new_count;
		product->description = new_description;
		cout << "Товар обновлен." << endl;
	}

	Product* findProductByName(string name) // найти товар по названию
	{
		auto it = products_name.find(name);

		if (it == products_name.end())
		{
			return nullptr;
		}
		else
		{
			return it->second;
		}
	}

	void tryToLogin(string login, string pass)  // попытка входа пользователя
	{
		User* user = findUserByLogin(login);
		if (!user)
		{
			cout << "Не существует пользователя с данным логином." << endl;
			return;
		}
		if (user->tryPassword(pass))
		{
			logged_user = user;
			cout << "Выполнен вход пользователя " << login << "." << endl;
		}
		else
		{
			cout << "Неверный пароль." << endl;
		}
	}

	void print_Products() // просмотр всех товаров(краткие данные);
	{
		// Отображаем элементы
		for (auto it = products_name.begin(); it != products_name.end(); ++it)
		{
			it->second->print_short();
		}
	}

	void print_Product_info(string name) // просмотр подробной информации о товаре;
	{
		if (!findProductByName(name))
		{
			cout << "Не существует товара с данным названием." << endl;
			return;
		}
		products_name[name]->print_full();
	}

	void create_New_Order() //создание заказа
	{
		if (!logged_user)
		{
			cout << "Сначала зайдите в профиль пользователя." << endl;
			return;
		}
		logged_user->current_order->clear(); // очищяем актуальный заказ пользователя
		logged_user->current_order->total_Price = 0;
		cout << "Начат новый заказ пользователя." << endl;
	}

	void add_Product_In_Order(string prodcut_name, int count) //добавление товара в заказ
	{
		if (!logged_user)
		{
			cout << "Сначала зайдите в профиль пользователя." << endl;
			return;
		}

		if (logged_user->current_order->isPaid)
		{
			cout << "Заказ уже оплачен. Сначала начните новый заказ." << endl;
			return;
		}

		Product* product = findProductByName(prodcut_name);
		if (!product)
		{
			cout << "Не существует товара с данным названием." << endl;
			return;
		}
		if (product->count < count)
		{
			cout << "Недостаточно товара на складе. Всего данного товара осталось: " << product->count << "." << endl;
			return;
		}
		auto it = logged_user->current_order->products.find(prodcut_name);
		if (it == logged_user->current_order->products.end())
		{
			logged_user->current_order->products[prodcut_name] = count;
		}
		else
		{
			logged_user->current_order->products[prodcut_name] += count;
		}
		logged_user->current_order->total_Price += product->price * count;

		cout << "Товар добавлен в заказ." << endl;
	}

	void print_Order() // просмотр заказа
	{
		if (!logged_user)
		{
			cout << "Сначала зайдите в профиль пользователя." << endl;
			return;
		}
		cout << "Заказ:" << endl;
		for (auto it = logged_user->current_order->products.begin(); it != logged_user->current_order->products.end(); ++it)
		{
			cout << "Товар: " << it->first << "; Количество: " << it->second << endl;
		}
		cout << "Общая стоимость: " << logged_user->current_order->total_Price << endl;
		cout << "Статус: ";
		if (logged_user->current_order->isPaid)
		{
			cout << "оплачен." << endl;
		}
		else
		{
			cout << "не оплачен." << endl;
		}
	}

	void pay_For_Order(double money_paid) //оплата заказа
	{
		if (!logged_user)
		{
			cout << "Сначала зайдите в профиль пользователя." << endl;
			return;
		}

		if (logged_user->current_order->isPaid)
		{
			cout << "Заказ уже оплачен." << endl;
			return;
		}

		if (money_paid < logged_user->current_order->total_Price)
		{
			cout << "Передана недостаточная сумма." << endl;
			return;
		}
		logged_user->current_order->isPaid = true;
		cout << "Заказ успешно оплачен. Сдача: " << money_paid - logged_user->current_order->total_Price << endl;
	}

};


void print_help()
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

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	setlocale(LC_ALL, "Russian");
	MyDB* db = new MyDB();
	int cmd, count;
	double price;
	string name, product_name, descr, password;
	User* newUser = nullptr;
	Product* newProduct = nullptr;

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
			cout << "Режим: Регистрация нового пользователя..." << endl;
			cout << "Введите логин пользователя: ";
			cin.ignore();
			getline(cin, name);
			cout << "Введите пароль: ";
			getline(cin, password);
			newUser = new User(name, password);
			db->addUser(newUser);
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
			newProduct = new Product(product_name, price, count, descr);
			db->addProduct(newProduct);
			break;
		case 3:
			cout << "Режим: Обновление информации о товаре..." << endl;
			cout << "Введите название товара: ";
			cin.ignore();
			getline(cin, product_name);
			newProduct = db->findProductByName(product_name);
			if (!newProduct)
			{
				cout << "Не удалось найти товар с заданным названием." << endl;
				break;
			}
			cout << "Введите новую стоимость товара: ";
			cin >> price;
			cout << "Введите новое количество товара: ";
			cin >> count;
			cout << "Введите новое краткое описание товара: ";
			cin.ignore();
			getline(cin, descr);
			db->updateProduct(product_name, price, count, descr);
			break;
		case 4:
			cout << "Режим: Вход пользователя..." << endl;
			cout << "Введите Ваш логин: ";
			cin.ignore();
			getline(cin, name);
			cout << "Введите пароль: ";
			getline(cin, password);
			db->tryToLogin(name, password);
			break;
		case 5:
			cout << "Режим: Просмотр всех товаров (краткие данные)..." << endl;
			db->print_Products();
			break;
		case 6:
			cout << "Режим: Просмотр подробной информации о товаре..." << endl;
			cout << "Введите название товара: ";
			cin.ignore();
			getline(cin, product_name);
			db->print_Product_info(product_name);
			break;
		case 7:
			cout << "Режим: Создание заказа..." << endl;
			db->create_New_Order();
			break;
		case 8:
			cout << "Режим: Добавление товара в заказ..." << endl;
			cout << "Введите название товара: ";
			cin.ignore();
			getline(cin, product_name);
			cout << "Введите количество: ";
			cin >> count;
			db->add_Product_In_Order(product_name, count);
			break;
		case 9:
			cout << "Режим: Просмотр заказа..." << endl;
			db->print_Order();
			break;
		case 10:
			cout << "Режим: Оплата заказа..." << endl;
			cout << "Введите предлагаемое количество денег: ";
			cin >> price;
			db->pay_For_Order(price);
			break;
		default:                               // Неверная команда
			cout << "Неправильная команда. Попробуйте снова." << endl;
		}
		cout << "**************************************************" << endl;
	} while (cin && (cmd != 0));

	return 0;
}
