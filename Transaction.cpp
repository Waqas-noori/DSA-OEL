#pragma once
#include <string>
#include <ctime>
using namespace std;

// ---- Queue (linked, front = oldest, rear = newest) ----
struct Transaction {
	int txnId;
	string type;       // IN, OUT, UNDO-IN, UNDO-OUT
	int prodId;
	int qty;
	time_t when;
	Transaction* next = nullptr;
};

class TransactionQueue {
	Transaction* frontP = nullptr;
	Transaction* rearP = nullptr;
	int count = 0;     // also the last transaction ID
public:
	~TransactionQueue() {
		while (frontP) { Transaction* n = frontP->next; delete frontP; frontP = n; }
	}

	// Appends a transaction at the rear. Also used when loading from file.
	void add(int id, const string& type, int prodId, int qty, time_t when) {
		Transaction* n = new Transaction{ id, type, prodId, qty, when };
		(rearP ? rearP->next : frontP) = n;
		rearP = n;
		count = id;
	}

	void log(const string& type, int prodId, int qty) {
		add(count + 1, type, prodId, qty, time(0));
	}

	Transaction* front() const { return frontP; }
};

// ---- Stack (linked, LIFO) of reversible stock operations ----
struct UndoEntry {
	string type;       // IN or OUT
	int prodId;
	int qty;
	UndoEntry* next;
};

class UndoStack {
	UndoEntry* top = nullptr;
public:
	~UndoStack() { while (top) pop(); }

	void push(const string& type, int prodId, int qty) {
		top = new UndoEntry{ type, prodId, qty, top };
	}

	UndoEntry* peek() const { return top; }

	void pop() {
		UndoEntry* t = top;
		top = top->next;
		delete t;
	}
};
