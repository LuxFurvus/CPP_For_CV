#pragma once
#ifndef _BS_BANK_
#define _BS_BANK_




class BANK {

public:
    BANK();

private:
    std::map<int, account> AccountMap;
    const char* AccountList = "AccountList";
    bool IsWorking = 1;
    void mainmenu();
    void LoadAccounts();
    void SaveAccounts();
    void GenerateAccounts(unsigned int);

    void AccountOpen();
    void MoneyAddTake(bool&&);
    void showAccount_ALL();
    void showAccount_ONE();
    void AccountClose();
};


BANK::BANK() {
    LoadAccounts();
    while (IsWorking) mainmenu();
}


void BANK::mainmenu() {
    std::cout << "\n============================\n"
        << "1. Open account\n"
        << "2. Deposit\n"
        << "3. Withdraw\n"
        << "4. Show account\n"
        << "5. Show all accounts (" << AccountMap.size() << ")" << '\n'
        << "6. Close account\n"
        << "7. Quit\n============================\n"
        << "\nSelect any option: \n";
    std::string InputCheck;
    std::cin >> InputCheck;
    int choice = SafeIntInput(InputCheck);
    choice = (choice > 7 || choice < 1) ? 999 : choice;

    switch (choice)
    {
    case 1:
        AccountOpen();
        break;
    case 2:
        MoneyAddTake(1);
        break;
    case 3:
        MoneyAddTake(0);
        break;
    case 4:
        showAccount_ONE();
        break;
    case 5:
        showAccount_ALL();
        break;
    case 6:
        AccountClose();
        break;
    case 7:
        IsWorking = 0;
        std::cout << "\n+++   GOODBYE...   +++\n";
        break;

    default:
        std::cout << "\nSelect available options only!\nTry again...\n\n";
        break;
    }
}

void BANK::LoadAccounts() {
    std::ifstream indoc;
    indoc.open(AccountList);
    if (!indoc || (LinesInFile(AccountList) < 4)) {
        GenerateAccounts(7);
    }
    else {
        unsigned int Lines = LinesInFile(AccountList);
        int AccountNum = (Lines - 1) / 4;
        for (int i = 0; i < AccountNum; i++) {
            account temp;
            indoc >> temp;
            AccountMap.emplace(temp.GetNumber(), temp);
        }
        auto iter = end(AccountMap);
        account::account_counter = iter->first + 1;
        std::cout << "\nAll accounts uploaded!\n";
    }
    SaveAccounts();
}


void BANK::SaveAccounts() {
    std::map<int, account>::iterator it;
    for (it = AccountMap.begin(); it != AccountMap.end(); it++) {
        if (it == AccountMap.begin()) {
            std::ofstream outf(AccountList, std::ios::trunc);
            outf << it->second;
        }
        else {
            std::ofstream outf(AccountList, std::ios::app);
            outf << it->second;
        }
    }
}


void BANK::GenerateAccounts(unsigned int NumOfAccounts) {
    for (int i = 0; i < NumOfAccounts; i++) {
        account NewAcc(1);
        AccountMap.emplace(NewAcc.GetNumber(), NewAcc);
        std::ofstream OutF;
        OutF.open(AccountList, std::ios::trunc);
        OutF << NewAcc;
    }
}


#endif