
BEGIN;

CREATE TABLE Contacts (
    Contact_ID SERIAL PRIMARY KEY,
    Version VARCHAR(16)
);

CREATE TABLE Names (
    Name_ID SERIAL PRIMARY KEY,
    Contact_ID INT NOT NULL,
    Family VARCHAR(255),
    Personal VARCHAR(255),
    Father VARCHAR(255),
    Address_Form VARCHAR(255),
    Suffix VARCHAR(255),
    FOREIGN KEY (Contact_ID) REFERENCES Contacts(Contact_ID)
);

CREATE TABLE Phonetics (
    Phonetic_ID SERIAL PRIMARY KEY,
    Contact_ID INT NOT NULL,
    First_Name VARCHAR(255),
    Middle_Name VARCHAR(255),
    Last_Name VARCHAR(255),
    FOREIGN KEY (Contact_ID) REFERENCES Contacts(Contact_ID)
);

CREATE TABLE Nicknames (
    NickName_ID SERIAL PRIMARY KEY,
    Contact_ID INT NOT NULL,
    Nick VARCHAR(255),
    FOREIGN KEY (Contact_ID) REFERENCES Contacts(Contact_ID)
);

CREATE TABLE Telephones (
    Telephone_ID SERIAL PRIMARY KEY,
    Contact_ID INT NOT NULL,
    tel_type VARCHAR(255),
    tel_number VARCHAR(255),
    FOREIGN KEY (Contact_ID) REFERENCES Contacts(Contact_ID)
);

CREATE TABLE Emails (
    Email_ID SERIAL PRIMARY KEY,
    Contact_ID INT NOT NULL,
    Email_Type VARCHAR(255),
    Address VARCHAR(255),
    FOREIGN KEY (Contact_ID) REFERENCES Contacts(Contact_ID)
);

CREATE TABLE Addresses (
    Address_ID SERIAL PRIMARY KEY,
    Contact_ID INT NOT NULL,
    Address_Type VARCHAR(255),
    Street VARCHAR(255),
    City VARCHAR(255),
    Region VARCHAR(255),
    Mail_Index VARCHAR(255),
    Country VARCHAR(255),
    FOREIGN KEY (Contact_ID) REFERENCES Contacts(Contact_ID)
);

CREATE TABLE WorkInfos (
    WorkInfo_ID SERIAL PRIMARY KEY,
    Contact_ID INT NOT NULL,
    Company VARCHAR(255),
    Department VARCHAR(255),
    Title VARCHAR(255),
    FOREIGN KEY (Contact_ID) REFERENCES Contacts(Contact_ID)
);

CREATE TABLE Urls (
    Url_ID SERIAL PRIMARY KEY,
    Contact_ID INT NOT NULL,
    Url_Address VARCHAR(255),
    FOREIGN KEY (Contact_ID) REFERENCES Contacts(Contact_ID)
);

CREATE TABLE Notes (
    Note_ID SERIAL PRIMARY KEY,
    Contact_ID INT NOT NULL,
    Note_Text TEXT,
    FOREIGN KEY (Contact_ID) REFERENCES Contacts(Contact_ID)
);

CREATE TABLE Events (
    Event_ID SERIAL PRIMARY KEY,
    Contact_ID INT NOT NULL,
    Event_Name VARCHAR(255),
    Event_Day VARCHAR(16),
    Event_Month VARCHAR(16),
    Event_Year VARCHAR(16),
    Event_Type INT,
    FOREIGN KEY (Contact_ID) REFERENCES Contacts(Contact_ID)
);

CREATE TABLE SocialNets (
    SNS_ID SERIAL PRIMARY KEY,
    Contact_ID INT NOT NULL,
    SNS_Name VARCHAR(255),
    SNS_Contact VARCHAR(255),
    FOREIGN KEY (Contact_ID) REFERENCES Contacts(Contact_ID)
);

CREATE TABLE Relations (
    Relation_ID SERIAL PRIMARY KEY,
    Contact_ID INT NOT NULL,
    Rel_Name VARCHAR(255),
    Rel_TypeName VARCHAR(255),
    Rel_TypeNum INT,
    FOREIGN KEY (Contact_ID) REFERENCES Contacts(Contact_ID)
);

COMMIT;
