#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "StockOps.cpp"
#include "FileHandler.cpp"
using namespace std;

static void printMenu() {
	cout << "\n <INVENTORY MANAGEMENT SYSTEM> \n"
		<< " 1. Add product\n"
		<< " 2. Update product\n"
		<< " 3. Delete product\n"
		<< " 4. Search product\n"
		<< " 5. Display sorted inventory\n"
		<< " 6. Stock-in\n"
		<< " 7. Stock-out\n"
		<< " 8. Undo last stock operation\n"
		<< " 9. Transaction history\n"
		<< "10. Inventory report\n"
		<< "11. Low-stock report\n"
		<< "12. Reorder suggestions\n"
		<< "13. Stock movement summary\n"
		<< " 0. Exit\n";
}

int main() {
	Inventory inv;
	loadAll(inv);
	int choice = -1;
	do {
		printMenu();
		choice = readNum("Enter choice: ");
		switch (choice) {
		case 1:  addProduct(inv); break;
		case 2:  updateProduct(inv); break;
		case 3:  deleteProduct(inv); break;
		case 4:  searchProduct(inv); break;
		case 5:  displaySorted(inv); break;
		case 6:  stockIn(inv); break;
		case 7:  stockOut(inv); break;
		case 8:  undoLastTransaction(inv); break;
		case 9:  showTransactions(inv); break;
		case 10: inventoryReport(inv); break;
		case 11: lowStockReport(inv); break;
		case 12: reorderReport(inv); break;
		case 13: movementSummary(inv); break;
		case 0:  break;
		default: cout << "Invalid choice.\n";
		}
		saveAll(inv);// data is saved after every operation
	} while (choice != 0);

	cout << "Goodbye.\n";
	return 0;   // destructors free all lists
}