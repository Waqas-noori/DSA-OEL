#pragma once
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include "Product.cpp"
using namespace std;

enum SortKey { BY_ID = 1, BY_NAME = 2, BY_QTY = 3, BY_PRICE = 4 };

string lower(string s) {
	for (size_t i = 0; i < s.size(); i++) if (s[i] >= 'A' && s[i] <= 'Z') s[i] += 32;
	return s;
}

// true if a and b are in the wrong order (a should come after b) for ascending order
bool outOfOrder(const Product* a, const Product* b, int key) {
	switch (key) {
	case BY_ID:    return a->id > b->id;
	case BY_NAME:  return lower(a->name) > lower(b->name);
	case BY_QTY:   return a->qty > b->qty;
	default:       return a->price > b->price;
	}
}

// Selection sort: put the next smallest element in location a[i].
void selectionSort(Product** a, int n, int key) {
	for (int i = 0; i < n; i++) {
		int smallest = i;
		for (int j = i + 1; j < n; j++)
			if (outOfOrder(a[smallest], a[j], key)) smallest = j;
		Product* t = a[i]; a[i] = a[smallest]; a[smallest] = t;
	}
}

// Insertion sort: take one value at a time and insert it into the sorted part (like sorting cards).
void insertionSort(Product** a, int n, int key) {
	for (int i = 1; i < n; i++) {
		Product* value = a[i];
		int j = i - 1;
		bool done = false;
		do {
			if (outOfOrder(a[j], value, key)) {
				a[j + 1] = a[j];
				j--;
				if (j < 0) done = true;
			}
			else done = true;
		} while (!done);
		a[j + 1] = value;
	}
}

void mergeRec(Product** a, Product** tmp, int lo, int hi, int key) {
	if (lo >= hi) return;
	int mid = (lo + hi) / 2;
	mergeRec(a, tmp, lo, mid, key);
	mergeRec(a, tmp, mid + 1, hi, key);
	int i = lo, j = mid + 1, k = lo;
	while (i <= mid && j <= hi)
		tmp[k++] = outOfOrder(a[i], a[j], key) ? a[j++] : a[i++];   // take left on ties: stable
	while (i <= mid) tmp[k++] = a[i++];
	while (j <= hi)  tmp[k++] = a[j++];
	for (k = lo; k <= hi; k++) a[k] = tmp[k];
}

void mergeSort(Product** a, int n, int key) {
	Product** tmp = new Product*[n];
	mergeRec(a, tmp, 0, n - 1, key);
	delete[] tmp;
}

int binarySearchId(Product* arr[], int lo, int hi, int id) {
	if (lo > hi) return -1;
	int mid = lo + (hi - lo) / 2;
	if (arr[mid]->id == id) return mid;
	if (arr[mid]->id < id) return binarySearchId(arr, mid + 1, hi, id);
	return binarySearchId(arr, lo, mid - 1, id);
}

bool containsIgnoreCase(const string& text, const string& part) {
	return lower(text).find(lower(part)) != string::npos;
}

double readNum(const string& prompt) {
	double v;
	cout << prompt; cin >> v;
	return v;
}

// Keep asking until the number is between lo and hi (inclusive).
double readRange(const string& prompt, double lo, double hi) {
	double v = readNum(prompt);
	while (v < lo || v > hi) {
		cout << "Value out of range. Try again.\n";
		v = readNum(prompt);
	}
	return v;
}

string readLine(const string& prompt) {
	string s;
	cout << prompt; getline(cin, s);
	return s;
}

bool confirm(const string& prompt) {
	char c;
	cout << prompt << " (y/n): "; cin >> c;
	return c == 'y' || c == 'Y';
}

string formatTime(time_t t) {
	char buf[32];
	strftime(buf, sizeof buf, "%Y-%m-%d %H:%M:%S", localtime(&t));
	return buf;
}