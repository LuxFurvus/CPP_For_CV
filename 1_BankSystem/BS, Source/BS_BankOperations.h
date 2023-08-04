#pragma once
#ifndef _BS_BANKOPER_
#define _BS_BANKOPER_

/*
        1. Open account
        2. Deposit
        3. Withdraw
        4. Show account
        5. Show all accounts
        6. Close account
*/



void BANK::AccountOpen() {
    account NewAcc(0);
    if (NewAcc.GetName() == "=" || NewAcc.GetSurname() == "=" || NewAcc.GetBalance() == -999'999'999)
    {
        return;
    }
    AccountMap.emplace(NewAcc.GetNumber(), NewAcc);
    std::ofstream OutF;
    OutF.open(AccountList, std::ios::app);
    OutF << NewAcc;
    // std::string Note = NewAcc.desc() + " with inicial $" + std::to_string(NewAcc.GetBalance()) + " was created!\n";
    std::cout << "\n*_*_*_*_*_*_*\n" << NewAcc.desc() + " with inicial $" + std::to_string(NewAcc.GetBalance()) + " was created!\n" << "*_*_*_*_*_*_*\n";
}



void BANK::MoneyAddTake(bool&& toAdd) {
    showAccount_ALL();
    std::string AddOrTake = (toAdd) ? "deposit" : "withdraw";
    std::string GetOrLose = (toAdd) ? "gets" : "loses";
    /////SELECT ACCOUNT//////////////////////
    std::cout << "\nSelect account to " << AddOrTake << ": \n...or print \"=\" to return to the Main Menu...\n";
    std::string InputCheck;
    std::cin >> InputCheck;
    if (InputCheck == "=") {
        return;
    }
    int aaa = SafeIntInput(InputCheck);
    if (AccountMap.find(aaa) == AccountMap.end()) {
        std::cout << "Account #" << aaa << " not found!\n";
        return;
    }
    /////SELECT MONEY SUM TO ADD OR TAKE//////////////////////
    std::cout << "\nSelect sum to " << AddOrTake << ": \n...or print \"=\" to return to the Main Menu...\n";
    std::cin >> InputCheck;
    if (InputCheck == "=") {
        return;
    }
    int moneysum = SafeIntInput(InputCheck);
    if (moneysum <= 0) {
        std::cout << "Enter only real sum!\n";
        return;
    }
    /////ADD OR TAKE MONEY TO ACCOUNT//////////////////////
    else {
        if (toAdd)	AccountMap.find(aaa)->second.MoneyAdd(moneysum);
        else AccountMap.find(aaa)->second.MoneyTake(moneysum);

    /////INFORM ABOUT SUCCESS//////////////////////		
        std::cout << "\n!!!   !!!\n"
            << AccountMap.find(aaa)->second.desc() + " " + GetOrLose + " $" + std::to_string(moneysum) + "!\n"
            << "\n"
            << AccountMap.find(aaa)->second.desc() + " now has $" + std::to_string(AccountMap.find(aaa)->second.GetBalance()) + "!\n"
            << "!!!   !!!\n\n";
    /////SAVE NEW STATE OF THE ACCOUNT//////////////////////
        SaveAccounts();
        std::cout << "\nREADY!!!\n";
    }
}



void BANK::showAccount_ONE() {
    std::cout
        << "There are " << AccountMap.size() << " accounts."
        << "\nEnter account number: \n...or print \"=\" to return to the Main Menu...\n";
    std::string InputCheck;

TryAgain:
    std::cin >> InputCheck;
    if (InputCheck == "=") {
        return;
    }
    
    int ToShow = SafeIntInput(InputCheck);
    if (ToShow > AccountMap.size() || ToShow < 0)    {
        std::cout << "\nInvalid account number! Try again...\n";
        goto TryAgain;
    }
    std::cout << "\n\n#" << AccountMap.at(ToShow).GetNumber() << " => "
        << AccountMap.at(ToShow).GetSurname() << " "
        << AccountMap.at(ToShow).GetName() << ": $"
        << AccountMap.at(ToShow).GetBalance() << "\n\n";
}



void BANK::showAccount_ALL() {
    std::cout << "\n\nALL ACCOUNTS: \n";
    for (auto iter : AccountMap) {
        std::cout << "#" << iter.first << " => ";
        std::cout << iter.second.GetSurname() << " ";
        std::cout << iter.second.GetName() << ": $";
        std::cout << iter.second.GetBalance() << "\n";
    }
    std::cout << '\n';
}



void BANK::AccountClose() {
    showAccount_ALL();
    std::cout << "Select account to close: \n...or print \"=\" to return to the Main Menu...\n";
    std::string InputCheck;

TryAgain:
    std::cin >> InputCheck;
    if (InputCheck == "=") {
        return;
    }    
    
    int ToClose = SafeIntInput(InputCheck);
    if (ToClose > AccountMap.size() || ToClose < 0)    {
        std::cout << "\nInvalid account number! Try again...\n";
        goto TryAgain;
    }
    else {
        // std::string Note = AccountMap.find(ToClose)->second.desc() + " was closed!\n";
        std::cout
            << "\n!!!   !!!\n"
            << AccountMap.find(ToClose)->second.desc() + " was closed!\n"
            << "!!!   !!!\n\n";
        AccountMap.erase(ToClose);
        SaveAccounts();
    }
}



#endif