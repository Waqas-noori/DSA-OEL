#pragma once
#include "Product.cpp"
#include "Transaction.cpp"

struct Inventory {
	ProductList products;     // linked list
	TransactionQueue txns;    // queue
	UndoStack undo;			  // stack (session only)
};