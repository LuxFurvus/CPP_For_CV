#pragma once
#ifndef _BS_ACCOUNT_
#define _BS_ACCOUNT_



class account {
private:
	std::string Notification, surname, name;
	long number;
	long balance;
public:
	static long account_counter;
	account(bool);
	int GetBalance();
	void MoneyAdd(int);
	void MoneyTake(int);
	int GetNumber();
	std::string GetName();
	std::string GetSurname();
	std::string desc();
	friend std::ofstream& operator<<(std::ofstream&, account&);
	friend std::ifstream& operator>>(std::ifstream&, account&);
};
long account::account_counter = 1;

account::account(bool FormType = 2) {
	number = account_counter;
	account_counter++;
	if (FormType == 0) {// we create account manually
		std::cout << "***NEW ACCOUNT***\nEnter account holder first name: \n...or print \"=\" to return to the Main Menu...\n";
		std::cin >> name;
		if (name == "=") {
			return;
		}
		std::cout << "Enter account holder second name: \n...or print \"=\" to return to the Main Menu...\n";
		std::cin >> surname;
		if (surname == "=") {
			return;
		}
		std::cout << "Enter account initial balance: \n...or print \"=\" to return to the Main Menu...\n";
		std::string init;
		std::cin >> init;
		if (init == "=") {
			balance = -999'999'999;
			return;
		}
		balance = SafeIntInput(init);
	}
	else if (FormType == 1) {// programm generates account randomly
		name = NameGen::RandName(2, 4);
		surname = NameGen::RandName(5, 6);
		balance = NameGen::RandNum(5000, 9000);
	}
	else;// programm recreates account from the file
}
std::ofstream& operator<<(std::ofstream& ss, account& aa) {
	ss << aa.GetNumber() << "\n";
	ss << aa.GetSurname() << "\n";
	ss << aa.GetName() << "\n";
	ss << aa.GetBalance() << "\n";
	return ss;
};
std::ifstream& operator>>(std::ifstream& ss, account& aa) {
	ss >> aa.number;
	ss >> aa.surname;
	ss >> aa.name;
	ss >> aa.balance;
	return ss;
};
int account::GetBalance() {
	return balance;
};
std::string account::GetName() {
	return name;
};
std::string account::GetSurname() {
	return surname;
};
int account::GetNumber() {
	return number;
}
std::string account::desc() {
	return "Account #" + std::to_string(number) + " for " + name + " " + surname;
}
void account::MoneyAdd(int sum) {
	this->balance += sum;
}
void account::MoneyTake(int sum) {
	this->balance -= sum;
}




#endif