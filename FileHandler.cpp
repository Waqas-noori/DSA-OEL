#pragma once
#include <fstream>
#include <iostream>
#include <iomanip>
#include "Product.cpp"
#include "Transaction.cpp"
#include "Inventory.cpp"
using namespace std;

const char* const PRODUCT_FILE = "products.txt";          // id qty price limit in out name
const char* const TRANSACTION_FILE = "transactions.txt";  // txnId type prodId qty time

void saveAll(const Inventory& inv) {
	ofstream pf(PRODUCT_FILE);
	pf << fixed << setprecision(2);
	for (Product* p = inv.products.first(); p; p = p->next)    // name is last, so it may contain spaces
		pf << p->id << " " << p->qty << " " << p->price << " " << p->th << " "
		<< p->in << " " << p->out << " " << p->name << "\n";

	ofstream tf(TRANSACTION_FILE);
	for (Transaction* t = inv.txns.front(); t; t = t->next)
		tf << t->txnId << " " << t->type << " " << t->prodId << " " << t->qty << " " << (long long)t->when << "\n";

	if (!pf || !tf) cout << "Warning: could not write data files.\n";
}

void loadAll(Inventory& inv) {
	int bad = 0;

	ifstream pf(PRODUCT_FILE);
	if (!pf) cout << "No saved data found. Starting with an empty inventory.\n";
	int id, q, th;
	double pr;
	long long inQty, outQty;
	string name;
	while (pf >> id >> q >> pr >> th >> inQty >> outQty) {
		getline(pf >> ws, name);                 // rest of the line is the name
		if (id <= 0 || name.empty() || q < 0 || q > MAX_QTY || pr < 0 || pr > MAX_PRICE
			|| th < 0 || th > MAX_QTY || inv.products.findById(id)) {
			bad++;
			continue;
		}
		Product* p = inv.products.add(id, name, q, pr, th);
		p->in = inQty;
		p->out = outQty;
	}

	ifstream tf(TRANSACTION_FILE);
	int tid, pid, tq;
	long long when;
	string type;
	while (tf >> tid >> type >> pid >> tq >> when) inv.txns.add(tid, type, pid, tq, (time_t)when);

	if (bad > 0) cout << "Warning: " << bad << " invalid product line(s) in the data file were skipped.\n";
}