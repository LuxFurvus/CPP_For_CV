CREATE TABLE Contacts (
    ContactID SERIAL PRIMARY KEY,
    Version VARCHAR(255)
);

CREATE TABLE Names (
    NameID SERIAL PRIMARY KEY,
    ContactID INT NOT NULL,
    Family VARCHAR(255),
    Personal VARCHAR(255),
    Father VARCHAR(255),
    AddressForm VARCHAR(255),
    Suffix VARCHAR(255),
    FOREIGN KEY (ContactID) REFERENCES Contacts(ContactID)
);

CREATE TABLE Phonetics (
    PhoneticID SERIAL PRIMARY KEY,
    ContactID INT NOT NULL,
    First VARCHAR(255),
    Middle VARCHAR(255),
    Last VARCHAR(255),
    FOREIGN KEY (ContactID) REFERENCES Contacts(ContactID)
);

CREATE TABLE Nicknames (
    NickNameID SERIAL PRIMARY KEY,
    ContactID INT NOT NULL,
    Nick VARCHAR(255),
    FOREIGN KEY (ContactID) REFERENCES Contacts(ContactID)
);

CREATE TABLE Telephones (
    TelephoneID SERIAL PRIMARY KEY,
    ContactID INT NOT NULL,
    Type VARCHAR(255),
    Number VARCHAR(255),
    FOREIGN KEY (ContactID) REFERENCES Contacts(ContactID)
);

CREATE TABLE Emails (
    EmailID SERIAL PRIMARY KEY,
    ContactID INT NOT NULL,
    Type VARCHAR(255),
    Address VARCHAR(255),
    FOREIGN KEY (ContactID) REFERENCES Contacts(ContactID)
);

CREATE TABLE Addresses (
    AddressID SERIAL PRIMARY KEY,
    ContactID INT NOT NULL,
    Type VARCHAR(255),
    Street VARCHAR(255),
    City VARCHAR(255),
    Region VARCHAR(255),
    Index VARCHAR(255),
    Country VARCHAR(255),
    FOREIGN KEY (ContactID) REFERENCES Contacts(ContactID)
);

CREATE TABLE WorkInfos (
    WorkInfoID SERIAL PRIMARY KEY,
    ContactID INT NOT NULL,
    Company VARCHAR(255),
    Department VARCHAR(255),
    Title VARCHAR(255),
    FOREIGN KEY (ContactID) REFERENCES Contacts(ContactID)
);

CREATE TABLE Urls (
    UrlID SERIAL PRIMARY KEY,
    ContactID INT NOT NULL,
    UrlAddress VARCHAR(255),
    FOREIGN KEY (ContactID) REFERENCES Contacts(ContactID)
);

CREATE TABLE Notes (
    NoteID SERIAL PRIMARY KEY,
    ContactID INT NOT NULL,
    NoteText TEXT,
    FOREIGN KEY (ContactID) REFERENCES Contacts(ContactID)
);

CREATE TABLE Events (
    EventID SERIAL PRIMARY KEY,
    ContactID INT NOT NULL,
    EventName VARCHAR(255),
    Day VARCHAR(10),
    Month VARCHAR(10),
    Year VARCHAR(10),
    EventType INT,
    FOREIGN KEY (ContactID) REFERENCES Contacts(ContactID)
);

CREATE TABLE SocialNets (
    SocialNetID SERIAL PRIMARY KEY,
    ContactID INT NOT NULL,
    Name VARCHAR(255),
    Contact VARCHAR(255),
    FOREIGN KEY (ContactID) REFERENCES Contacts(ContactID)
);

CREATE TABLE Relations (
    RelationID SERIAL PRIMARY KEY,
    ContactID INT NOT NULL,
    Name VARCHAR(255),
    TypeName VARCHAR(255),
    TypeNum INT,
    FOREIGN KEY (ContactID) REFERENCES Contacts(ContactID)
);
