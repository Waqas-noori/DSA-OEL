#pragma once
#include <iostream>
#include <iomanip>
#include "Product.cpp"
#include "Transaction.cpp"
#include "Helpers.cpp"
#include "Inventory.cpp"

// Asks for a product ID and returns the product, or nullptr (after printing an error).
Product* askProduct(Inventory& inv) {
	Product* p = inv.products.findById(readNum("Enter Product ID: "));
	if (!p) cout << "Product not found.\n";
	return p;
}

// Changes a product's stock, updates its movement totals and logs the transaction.
// IN and UNDO-OUT add stock; OUT and UNDO-IN remove it.
void applyMove(Inventory& inv, Product* p, const string& type, int q) {
	if (type == "IN" || type == "UNDO-OUT") { p->qty += q; p->in += q; }
	else { p->qty -= q; p->out += q; }
	inv.txns.log(type, p->id, q);
}

void stockIn(Inventory& inv) {
	Product* p = askProduct(inv);
	if (!p) return;

	int q = readNum("Enter quantity to add: ");
	if (q <= 0) { cout << "Invalid quantity\n"; return; }
	if ((long long)p->qty + q > MAX_QTY) {
		cout << "Error: total stock cannot exceed " << MAX_QTY << ". Current: " << p->qty << "\n";
		return;
	}

	applyMove(inv, p, "IN", q);
	inv.undo.push("IN", p->id, q);
	cout << "Stock added. New quantity: " << p->qty << "\n";
}

void stockOut(Inventory& inv) {
	Product* p = askProduct(inv);
	if (!p) return;
	if (p->qty == 0) { cout << "Error: product has zero stock.\n"; return; }

	int q = readNum("Enter quantity to remove: ");
	if (q <= 0) { cout << "Invalid quantity\n"; return; }
	if (q > p->qty) { cout << "Insufficient stock. Available: " << p->qty << "\n"; return; }

	applyMove(inv, p, "OUT", q);
	inv.undo.push("OUT", p->id, q);
	cout << "Stock removed. New quantity: " << p->qty << "\n";
	if (p->qty <= p->th) cout << "Warning: this product is now low on stock.\n";
}

void undoLastTransaction(Inventory& inv) {
	UndoEntry* e = inv.undo.peek();
	if (!e) { cout << "Nothing to undo.\n"; return; }
	cout << "Last operation: " << e->type << " " << e->qty << " unit(s) of product " << e->prodId << "\n";

	Product* p = inv.products.findById(e->prodId);
	if (!p) {
		inv.undo.pop();
		cout << "Product no longer exists. Undo entry discarded.\n";
		return;
	}
	if (e->type == "IN" && p->qty < e->qty) { cout << "Cannot undo: that stock has already been used.\n"; return; }
	if (e->type == "OUT" && (long long)p->qty + e->qty > MAX_QTY) { cout << "Cannot undo: stock would exceed the maximum.\n"; return; }

	applyMove(inv, p, "UNDO-" + e->type, e->qty);
	inv.undo.pop();
	cout << "Undone. New quantity: " << p->qty << "\n";
}

void showTransactions(Inventory& inv) {
	if (!inv.txns.front()) { cout << "No transactions recorded.\n"; return; }
	cout << "1. All transactions\n2. Transactions of one product\n";
	int choice = readNum("Enter choice: ");
	int filterId = 0;
	if (choice == 2) filterId = readNum("Enter Product ID: ");
	else if (choice != 1) { cout << "Invalid choice\n"; return; }

	int shown = 0;
	for (Transaction* t = inv.txns.front(); t; t = t->next) {      // front (oldest) to rear (newest)
		if (choice == 2 && t->prodId != filterId) continue;
		if (shown++ == 0)
			cout << left << setw(8) << "TxnID" << setw(10) << "Type" << setw(8) << "ProdID"
			<< setw(8) << "Qty" << "Time\n";
		cout << left << setw(8) << t->txnId << setw(10) << t->type << setw(8) << t->prodId
			<< setw(8) << t->qty << formatTime(t->when) << "\n";
	}
	if (shown == 0) cout << "No transactions found for that product.\n";
}

void inventoryReport(Inventory& inv) {
	if (!inv.products.first()) { cout << "Inventory is empty.\n"; return; }
	double totalValue = 0;
	long long totalUnits = 0;
	printHeader();
	for (Product* p = inv.products.first(); p; p = p->next) {
		printProduct(p);
		totalValue += p->price * p->qty;
		totalUnits += p->qty;
	}
	cout << "Total products: " << inv.products.size() << "\n";
	cout << "Total units in stock: " << totalUnits << "\n";
	cout << "Total inventory value: " << fixed << setprecision(2) << totalValue << "\n";
}

void lowStockReport(Inventory& inv) {
	int found = 0;
	for (Product* p = inv.products.first(); p; p = p->next)
		if (p->qty <= p->th) {
			if (found++ == 0) printHeader();
			printProduct(p);
		}
	if (found == 0) cout << "No low-stock products.\n";
	else cout << found << " product(s) need attention.\n";
}

// Suggests restocking each low/out-of-stock product up to twice its limit (at least 1 unit).
void reorderReport(Inventory& inv) {
	int found = 0;
	double totalCost = 0;
	for (Product* p = inv.products.first(); p; p = p->next) {
		if (p->qty > p->th) continue;
		int order = 2 * p->th - p->qty;
		if (order < 1) order = 1;
		if (found++ == 0)
			cout << left << setw(8) << "ID" << setw(20) << "Name" << setw(10) << "Qty"
			<< setw(10) << "Limit" << setw(12) << "Order Qty" << "Est. Cost\n";
		double cost = order * p->price;
		totalCost += cost;
		cout << left << setw(8) << p->id << setw(20) << p->name << setw(10) << p->qty
			<< setw(10) << p->th << setw(12) << order << fixed << setprecision(2) << cost << "\n";
	}
	if (found == 0) cout << "No products need reordering.\n";
	else cout << "Estimated total reorder cost: " << fixed << setprecision(2) << totalCost << "\n";
}

// Units moved per product, from the totals kept on each product. An undo counts as the opposite movement.
void movementSummary(Inventory& inv) {
	if (!inv.txns.front()) { cout << "No transactions recorded.\n"; return; }
	cout << left << setw(8) << "ID" << setw(20) << "Name" << setw(12) << "Units In"
		<< setw(12) << "Units Out" << "Net\n";
	for (Product* p = inv.products.first(); p; p = p->next)
		cout << left << setw(8) << p->id << setw(20) << p->name << setw(12) << p->in
		<< setw(12) << p->out << (p->in - p->out) << "\n";
}

void addProduct(Inventory& inv) {
	int id = readNum("Enter Product ID (greater than 0): ");
	if (id <= 0) { cout << "Invalid ID\n"; return; }
	if (inv.products.findById(id)) { cout << "Error: Product ID already exists.\n"; return; }

	string name = readLine("Enter Name: ");

	for (Product* p = inv.products.first(); p; p = p->next) if (p->name == name) {
		cout << "Name already exists\n";
		return;
	}

	int q = readRange("Enter Quantity (0 to 1000000): ", 0, MAX_QTY);
	double pr = readRange("Enter Price (0 to 10000000): ", 0, MAX_PRICE);
	int th = readRange("Enter Low-stock limit (0 to 1000000): ", 0, MAX_QTY);

	inv.products.add(id, name, q, pr, th);
	if (q > 0) inv.txns.log("IN", id, q);          // opening stock is recorded (not undoable)
	cout << "Product added.\n";
}

void updateProduct(Inventory& inv) {
	Product* p = askProduct(inv);
	if (!p) return;

	cout << "1. Name\n2. Price\n3. Low-stock limit\n";
	switch ((int)readNum("What do you want to update? ")) {
	case 1: p->name = readLine("Enter new name: "); break;
	case 2: p->price = readRange("Enter new price: ", 0, MAX_PRICE); break;
	case 3: p->th = readRange("Enter new limit: ", 0, MAX_QTY); break;
	default: cout << "Invalid choice\n"; return;
	}
	cout << "Product updated.\n";
}

void deleteProduct(Inventory& inv) {
	Product* p = askProduct(inv);
	if (!p) return;
	if (p->qty > 0) cout << "Note: this product still has " << p->qty << " unit(s) in stock.\n";
	if (!confirm("Delete '" + p->name + "'?")) { cout << "Delete cancelled.\n"; return; }
	inv.products.remove(p->id);
	cout << "Product deleted.\n";
}

void searchProduct(Inventory& inv) {
	cout << "Search by\n1. ID (linear search)\n2. ID (binary search)\n3. Name (partial, ignores case)\n4. Price range\n";
	int choice = readNum("Enter choice: ");

	switch (choice) {
	case 1: {
		Product* p = askProduct(inv);
		if (p) { printHeader(); printProduct(p); }
		break;
	}
	case 2: {
		int id = readNum("Enter Product ID: ");
		int n = inv.products.size();
		if (n == 0) { cout << "Inventory is empty.\n"; return; }
		Product** arr = inv.products.toArray();
		mergeSort(arr, n, BY_ID);                     // binary search needs sorted data
		int idx = binarySearchId(arr, 0, n - 1, id);
		if (idx == -1) cout << "Product not found.\n";
		else { printHeader(); printProduct(arr[idx]); }
		delete[] arr;
		break;
	}
	case 3: {
		string key = readLine("Enter name or part of the name: ");
		int found = 0;
		for (Product* p = inv.products.first(); p; p = p->next)
			if (containsIgnoreCase(p->name, key)) {
				if (found++ == 0) printHeader();
				printProduct(p);
			}
		if (found == 0) cout << "Product not found.\n";
		break;
	}
	case 4: {
		double lo = readNum("Minimum price: ");
		double hi = readNum("Maximum price: ");
		if (lo < 0 || hi < lo) { cout << "Invalid price range\n"; return; }
		int found = 0;
		for (Product* p = inv.products.first(); p; p = p->next)
			if (p->price >= lo && p->price <= hi) {
				if (found++ == 0) printHeader();
				printProduct(p);
			}
		if (found == 0) cout << "No products in that price range.\n";
		break;
	}
	default:
		cout << "Invalid choice\n";
	}
}

void displaySorted(Inventory& inv) {
	int n = inv.products.size();
	if (n == 0) { cout << "Inventory is empty.\n"; return; }

	cout << "Sort by:\n1. ID\n2. Name\n3. Quantity\n4. Price\n";
	int key = readRange("Enter choice: ", 1, 4);

	cout << "Order:\n1. Ascending\n2. Descending\n";
	int ord = readRange("Enter choice: ", 1, 2);

	cout << "Algorithm:\n1. Selection sort\n2. Insertion sort\n3. Merge sort\n";
	int alg = readRange("Enter choice: ", 1, 3);

	Product** arr = inv.products.toArray();
	if (alg == 1) selectionSort(arr, n, key);
	else if (alg == 2) insertionSort(arr, n, key);
	else mergeSort(arr, n, key);

	printHeader();
	if (ord == 1)
		for (int i = 0; i < n; i++) printProduct(arr[i]);        // ascending: read from the start
	else
		for (int i = n - 1; i >= 0; i--) printProduct(arr[i]);   // descending: read from the end
	delete[] arr;
}