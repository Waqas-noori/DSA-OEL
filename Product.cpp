#pragma once
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

const int    MAX_QTY = 1000000;       // maximum quantity (also the maximum low-stock limit)
const double MAX_PRICE = 10000000.0;  // maximum unit price

struct Product {
	int id;
	string name;
	int qty;
	double price;
	int th;                 // low-stock threshold: qty <= th means LOW STOCK
	long long in;           // total units moved in (kept here so the movement summary needs no scan)
	long long out;          // total units moved out
	Product* next = nullptr;
};

class ProductList {
	Product* head = nullptr;
	Product* tail = nullptr;          // insertion O(1)
	int count = 0;
public:
	~ProductList() {
		while (head) { Product* n = head->next; delete head; head = n; }
	}

	Product* add(int id, const string& name, int qty, double price, int th) {
		Product* n = new Product{ id, name, qty, price, th, qty, 0 };   // opening stock counts as units in
		(tail ? tail->next : head) = n;
		tail = n;
		count++;
		return n;
	}

	Product* findById(int id) {
		for (Product* p = head; p; p = p->next) if (p->id == id) return p;
		return nullptr;
	}

	bool remove(int id) {
		Product* prev = nullptr;
		Product* p = head;
		while (p && p->id != id) { prev = p; p = p->next; }
		if (!p) return false;

		(prev ? prev->next : head) = p->next;
		if (p == tail) tail = prev;
		delete p;
		count--;
		return true;
	}

	Product** toArray() {
		Product** arr = new Product*[count];
		int i = 0;
		for (Product* p = head; p; p = p->next) arr[i++] = p;
		return arr;
	}

	int size() const { return count; }
	Product* first() const { return head; }
};

string stockStatus(const Product* p) {
	if (p->qty == 0) return "OUT OF STOCK";
	if (p->qty <= p->th) return "LOW STOCK";
	return "OK";
}

void printHeader() {
	cout << left << setw(8) << "ID" << setw(20) << "Name" << setw(10) << "Qty"
		<< setw(12) << "Price" << setw(8) << "Limit" << "Status\n";
}

void printProduct(const Product* p) {
	cout << left << setw(8) << p->id << setw(20) << p->name << setw(10) << p->qty
		<< setw(12) << fixed << setprecision(2) << p->price << setw(8) << p->th
		<< stockStatus(p) << "\n";
}
