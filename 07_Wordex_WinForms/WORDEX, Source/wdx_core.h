//Copyright (c) 2023 Alex Lykov. All rights reserved.
// This file contains the entire game core. All game logic is implemented within the GameCoreClass functions.
// The GameCoreClass also includes the Profile struct, which serves as a container for saving and loading data at runtime.

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <fstream>

#pragma once
#ifndef _WDX_GAMECORECLASS_
#define _WDX_GAMECORECLASS_

namespace WDX {
	enum class AccessPoint {
		kFromMainMenu,
		kGameStart,
		kFromGame
	};

	class GameCoreClass final {
		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////

		struct WordPair final {
			std::string WordOne;
			std::string WordTwo;

		private:
			WordPair() = delete;

			inline explicit WordPair(const std::string& first, const std::string& second)
				: WordOne(first), WordTwo(second) {}

			friend class GameCoreClass;
		};

		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		struct Profile final {
		private:
			std::unordered_map<std::string, std::vector<WordPair>> AllLoadedDecks;
			std::string ProfileName;
			int ProfileScore;
		public:
			bool LettersOnly;

		private:
			inline Profile() : ProfileName("%%%"), ProfileScore(0), LettersOnly(false) {}
			inline Profile(std::string NewName, int NewScore, bool NewLettersOnly = false) {
				ProfileName = NewName;
				ProfileScore = NewScore;
				LettersOnly = NewLettersOnly;
			}

			friend class GameCoreClass;
		};

		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
	public:
		Profile CurrentProfile_;
	private:
		std::unordered_set<std::string> DeckNamesAvailable_;
		std::unordered_set<std::string> DeckNamesLoaded_;
		const std::string kDirectoryDecks_{ "WRX_DECKS/" };
		const std::string kDirectoryProfiles_{ "WDX_PROFILES/" };
	public:
		const std::string kDeckFormat_{ ".txt" };
		const std::string kProfileFormat_{ ".bin" };
	private:
		std::string CurrentWordOne_;
		std::string CurrentWordTwo_;
		std::string CurrentDeckName_;
		AccessPoint AccessWindow_;
		std::vector<WordPair>* CurrentDeck_;
		int CurrentDeckIndex_;
		char ProfileDelimiter_;

		//////////////////////////////////////////////////////////////
	private:
		inline GameCoreClass() : ProfileDelimiter_('\t') {}

		GameCoreClass(const GameCoreClass&) = delete;
		GameCoreClass& operator=(const GameCoreClass&) = delete;
		GameCoreClass(GameCoreClass&&) = delete;
		GameCoreClass& operator=(GameCoreClass&&) = delete;
		//////////////////////////////////////////////////////////////
		////////////////////////ACCESSORS///////////////////////////
		//////////////////////////////////////////////////////////////
	public:
		inline static GameCoreClass& GetCore() {
			static GameCoreClass GameCore;
			return GameCore;
		}

		inline void SetAccessPoint(AccessPoint Point) {
			AccessWindow_ = Point;
		}

		inline void SetDeckCurrent() {
			CurrentDeck_ = &CurrentProfile_.AllLoadedDecks.begin()->second;
			CurrentDeckName_ = CurrentProfile_.AllLoadedDecks.begin()->first;
		}

		inline void SetDelimiter(char w = '\t') {
			if (w == '\0') {
				ProfileDelimiter_ = '\t';
			}
			ProfileDelimiter_ = w;
		}

		//////////////////////////////////////////////////////////////
	public:
		inline bool GetDirProfiles(std::filesystem::path& DirProfiles) const {
			std::filesystem::path DirPath(kDirectoryProfiles_);
			DirProfiles = DirPath;
			if (std::filesystem::exists(DirPath)
				&& std::filesystem::is_directory(DirPath)) {
				return true;
			}
			return false;
		}

		inline bool GetDirDecks(std::filesystem::path& DirDecks) const {
			std::filesystem::path DirPath(kDirectoryDecks_);
			DirDecks = DirPath;
			if (std::filesystem::exists(DirPath)
				&& std::filesystem::is_directory(DirPath)) {
				return true;
			}
			return false;
		}

		//////////////////////////////////////////////////////////////
	public:
		inline AccessPoint GetAccessPoint() const {
			return AccessWindow_;
		}
		inline const std::string& GetWordFirst() const {
			return CurrentWordOne_;
		}
		inline const std::string& GetWordSecond() const {
			return CurrentWordTwo_;
		}
		inline const std::string& GetProfileName()const {
			return CurrentProfile_.ProfileName;
		}
		inline const size_t GetCurrentDeckSize() const {
			return CurrentDeck_->size();
		}
		inline const std::string GetCurrentDeckName() const {
			return CurrentDeckName_;
		}
		inline const std::vector<WordPair>* GetCurrentDeck() const {
			return CurrentDeck_;
		}
		inline const int& GetScore() const {
			return CurrentProfile_.ProfileScore;
		}
		inline size_t GetLoadedDecksNum() const {
			return CurrentProfile_.AllLoadedDecks.size();
		}

		//////////////////////////////////////////////////////////////
		///////////////////CORE, SCORE AND PROFILE////////////////////
		//////////////////////////////////////////////////////////////
	public:
		inline void AddScorePoints(int ScoreChange) {
			CurrentProfile_.ProfileScore += ScoreChange;
		}

		void ResetGameCore();
		bool MakeNewProfile(const std::string& NewName);
		bool FormProlileList(std::vector<std::string>& ProfileNames);

		//////////////////////////////////////////////////////////////
		////////////////////////SAVE & LOAD///////////////////////////
		//////////////////////////////////////////////////////////////
	public:
		bool SaveProfile();
		bool LoadProfile(const std::string& ProfileName);

		//////////////////////////////////////////////////////////////
		////////////////////////WORD FUNCTIONS////////////////////////
		//////////////////////////////////////////////////////////////
	public:
		std::string GetWordHinted(uint8_t HintNum) const;

		inline void EraseWordPair() {
			static_cast<void>(
				CurrentDeck_->erase(CurrentDeck_->begin() + CurrentDeckIndex_)
				);
		}

		inline void UpdateWordPair() {
			if (CurrentDeck_->size() == 0) return;
			srand(static_cast<unsigned int>(time(0)));
			CurrentDeckIndex_ = rand() % CurrentDeck_->size();

			CurrentWordOne_ = CurrentDeck_->at(CurrentDeckIndex_).WordOne;
			CurrentWordTwo_ = CurrentDeck_->at(CurrentDeckIndex_).WordTwo;
		}

		//////////////////////////////////////////////////////////////
		////////////////////////DECK FUNCTIONS////////////////////////
		//////////////////////////////////////////////////////////////
	private:
		std::vector<WordPair> ReadADeck(const std::string& FileName);

	public:
		std::vector<std::string> CollectSelectedDecks(std::unordered_set<std::string>& SelectedDeckNames);

		void UpdateDeckLists(
			std::unordered_set<std::string>& DeckNamesAvailable,
			std::unordered_set<std::string>& DeckNamesLoaded,
			bool AvailableOnly = false);

		inline void SelectDeckCurrent(const std::string& DeckName) {
			CurrentDeck_ = &CurrentProfile_.AllLoadedDecks.at(DeckName);
			CurrentDeckName_ = DeckName;
			UpdateWordPair();
		}

		//////////////////////////////////////////////////////////////////////////
	};
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////

void WDX::GameCoreClass::ResetGameCore() {
	DeckNamesAvailable_.clear();
	DeckNamesLoaded_.clear();
	CurrentWordOne_.clear();
	CurrentWordTwo_.clear();
	CurrentDeckName_.clear();
	AccessWindow_ = AccessPoint::kFromMainMenu;
	CurrentDeck_ = nullptr;
	CurrentDeckIndex_ = 0;
	ProfileDelimiter_ = '\t';
	CurrentProfile_.AllLoadedDecks.clear();
	CurrentProfile_.ProfileName.clear();
	CurrentProfile_.ProfileScore = 0;
	CurrentProfile_.LettersOnly = false;
}

bool WDX::GameCoreClass::MakeNewProfile(const std::string& NewName) {
	CurrentProfile_.ProfileName = NewName;
	if (std::filesystem::exists(kDirectoryProfiles_) == false) {
		std::filesystem::create_directory(kDirectoryProfiles_);
	}

	std::string FileName = kDirectoryProfiles_ + NewName + kProfileFormat_;
	std::ofstream FileForProfile(FileName, std::ios::binary | std::ios::trunc);

	if (!FileForProfile.is_open()) {
		return false;
	}
	else {
		FileForProfile.close();
		return true;
	}
}

bool WDX::GameCoreClass::FormProlileList(std::vector<std::string>& ProfileNames) {
	// Get the path to the profiles directory

	std::filesystem::path DirPath;

	if (GetDirProfiles(DirPath) == false) {
		return false;
	}

	// Iterate over the directory and add the names of all valid .bin files to the vector. All empty bin files will be deleted!
	for (const auto& Entry : std::filesystem::directory_iterator(DirPath)) {
		if (Entry.is_regular_file() && Entry.path().extension() == kProfileFormat_) {
			if (std::filesystem::file_size(Entry) > 0) {
				ProfileNames.push_back(Entry.path().filename().string());
			}
			else {
				std::filesystem::remove(Entry);
			}
		}
	}

	std::sort(begin(ProfileNames), end(ProfileNames));

	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////

void WDX::GameCoreClass::UpdateDeckLists(
	std::unordered_set<std::string>& DeckNamesAvailable,
	std::unordered_set<std::string>& DeckNamesLoaded,
	bool AvailableOnly)
{
	//If directory is empty or non-existent, stop here
	if (!std::filesystem::exists(kDirectoryDecks_)) {
		std::filesystem::create_directory(kDirectoryDecks_);
		return;
	}
	if (std::filesystem::is_empty(kDirectoryDecks_)) {
		return;
	}

	/// If the game has not started, gets names of available decks only
	if (AvailableOnly) {
		for (const auto& entry : std::filesystem::directory_iterator(kDirectoryDecks_)) {
			if (entry.path().extension() != kDeckFormat_) continue;
			std::string DeckName = entry.path().filename().stem().string();
			DeckNamesAvailable.emplace(DeckName);
		}
		return;
	}
	/// If the game has started, and CurrentProfile_ contains some decks
	else {
		//Form the list of Loaded Decks
		for (const auto& pair : CurrentProfile_.AllLoadedDecks) {
			DeckNamesLoaded.emplace(pair.first);
		}
		//Form the list of Available Decks
		for (const auto& entry : std::filesystem::directory_iterator(kDirectoryDecks_)) {
			if (entry.path().extension() != kDeckFormat_) continue;
			std::string DeckName = entry.path().filename().stem().string();
			if (DeckNamesLoaded.find(DeckName) != end(DeckNamesLoaded)) continue;
			DeckNamesAvailable.emplace(DeckName);
		}
		return;
	}
}

//////////////////////////////////////////////////////////////

std::vector<WDX::GameCoreClass::WordPair> WDX::GameCoreClass::ReadADeck(
	const std::string& FileName)
{
	std::vector<WordPair> TempPairs;
	std::ifstream file(FileName);
	std::string line;

	while (std::getline(file, line)) {
		std::string FirstW, SecondW;
		bool IsFirst{ true };
		if (line.size() < 3) continue;
		for (const auto& ch : line) {
			if (IsFirst == true && (ch != ProfileDelimiter_)) {
				FirstW.push_back(ch);
			}
			else if (IsFirst == false && (ch != ProfileDelimiter_)) {
				SecondW.push_back(ch);
			}
			else IsFirst = false;
		}
		if (SecondW.size() <= 0)  continue;
		TempPairs.emplace_back(WordPair(FirstW, SecondW));
	}
	return TempPairs;
}

//////////////////////////////////////////////////////////////

std::vector<std::string> WDX::GameCoreClass::CollectSelectedDecks(std::unordered_set<std::string>& SelectedDeckNames) {
	//Vector for empty decks or decks with wrong delimiter
	std::vector<std::string> FailedDecks;

	for (const auto& name : SelectedDeckNames) {
		//if (DeckNamesAvailable_.find(name) == end(DeckNamesAvailable_)) continue;
		std::vector<WordPair> TempPairs = ReadADeck(kDirectoryDecks_ + name + kDeckFormat_);
		//If the processed deck is empty, remember it
		if (TempPairs.size() > 0) {
			CurrentProfile_.AllLoadedDecks[name] = TempPairs;
		}
		else {
			FailedDecks.push_back(name);
		}
		TempPairs.clear();
	}

	if (CurrentProfile_.AllLoadedDecks.size() != 0) {
		//Assign the first loaded deck to play at game start
		CurrentDeck_ = &CurrentProfile_.AllLoadedDecks.begin()->second;
	}

	return FailedDecks;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////

std::string WDX::GameCoreClass::GetWordHinted(uint8_t HintNum) const {
	std::string temp;
	for (size_t i = 0; i <= CurrentWordTwo_.length() - 1; ++i) {
		if (i <= HintNum) {
			temp.push_back(CurrentWordTwo_[i]);
			continue;
		}
		else {
			temp.push_back('*');
			continue;
		}
	}
	return temp;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool WDX::GameCoreClass::LoadProfile(const std::string& ProfileName) {
	if (std::filesystem::exists(kDirectoryProfiles_) == false) {
		std::filesystem::create_directory(kDirectoryProfiles_);
	}

	std::ifstream file(kDirectoryProfiles_
		+ ProfileName
		+ kProfileFormat_,
		std::ios::in | std::ios::binary);
	if (!file.is_open()) {
		return false;
	}

	// Empty variables to collect file data
	int NewScore{ 0 };
	bool NewLettersOnly{ false };
	std::unordered_map<std::string, std::vector<WordPair>> ProfileDecks;

	// Read the number of loaded decks
	size_t NumDecks;
	file.read(reinterpret_cast<char*>(&NumDecks), sizeof(size_t));

	// Read each loaded deck
	for (size_t i = 0; i < NumDecks; ++i) {
		// Read the deck name size and deck name
		size_t DeckNameSize;
		file.read(reinterpret_cast<char*>(&DeckNameSize), sizeof(size_t));
		std::string NewDeckName(DeckNameSize, '\0');
		file.read(&NewDeckName[0], DeckNameSize);

		// Read the number of word pairs in the deck
		size_t NumWordPairs;
		file.read(reinterpret_cast<char*>(&NumWordPairs), sizeof(size_t));

		// Read each word pair
		std::vector<WordPair> NewWordPair;
		for (size_t j = 0; j < NumWordPairs; ++j) {
			// Read the first word size and the first word
			size_t WordOneSize;
			file.read(reinterpret_cast<char*>(&WordOneSize), sizeof(size_t));
			std::string WordFirst(WordOneSize, '\0');
			file.read(&WordFirst[0], WordOneSize);

			// Read the second word size and the second word
			size_t WordTwoSize;
			file.read(reinterpret_cast<char*>(&WordTwoSize), sizeof(size_t));
			std::string WordSecond(WordTwoSize, '\0');
			file.read(&WordSecond[0], WordTwoSize);

			NewWordPair.emplace_back(WordPair(WordFirst, WordSecond));
		}

		ProfileDecks[NewDeckName] = NewWordPair;
	}

	// Read ProfileName
	size_t ProfileNameSize;
	file.read(reinterpret_cast<char*>(&ProfileNameSize), sizeof(size_t));
	std::string NewProfileName(ProfileNameSize, '\0');
	file.read(&NewProfileName[0], ProfileNameSize);

	// Read ProfileScore
	file.read(reinterpret_cast<char*>(&NewScore), sizeof(int));

	// Read LettersOnly
	file.read(reinterpret_cast<char*>(&NewLettersOnly), sizeof(bool));

	// Create a new instance of Profile
	Profile LoadedProfile(NewProfileName, NewScore, NewLettersOnly);

	// Set the loaded decks
	LoadedProfile.AllLoadedDecks = ProfileDecks;

	// Assign the new profile to CurrentProfile_
	CurrentProfile_ = LoadedProfile;

	//When the profile is loaded, set its first deck as current
	SetDeckCurrent();

	return true;
}

bool WDX::GameCoreClass::SaveProfile() {
	if (std::filesystem::exists(kDirectoryProfiles_) == false) {
		std::filesystem::create_directory(kDirectoryProfiles_);
	}

	std::ofstream SavedFile(
		kDirectoryProfiles_
		+ CurrentProfile_.ProfileName
		+ kProfileFormat_,
		std::ios::out | std::ios::binary | std::ios::trunc);

	if (!SavedFile.is_open()) {
		return false;
	}

	// Write the number of loaded decks
	size_t NumDecks = CurrentProfile_.AllLoadedDecks.size();
	SavedFile.write(reinterpret_cast<const char*>(&NumDecks), sizeof(size_t));

	// Write each loaded deck
	for (const auto& LoadedDeck : CurrentProfile_.AllLoadedDecks) {
		// Write the deck name size and deck name
		size_t DeckNameSize = LoadedDeck.first.size();
		SavedFile.write(reinterpret_cast<const char*>(&DeckNameSize), sizeof(size_t));
		SavedFile.write(LoadedDeck.first.data(), DeckNameSize);

		// Write the number of word pairs in the deck
		size_t NumWordPairs = LoadedDeck.second.size();
		SavedFile.write(reinterpret_cast<const char*>(&NumWordPairs), sizeof(size_t));

		// Write each word pair
		for (const auto& OldWordPair : LoadedDeck.second) {
			// Write the first word size and the first word
			size_t WordOneSize = OldWordPair.WordOne.size();
			SavedFile.write(reinterpret_cast<const char*>(&WordOneSize), sizeof(size_t));
			SavedFile.write(OldWordPair.WordOne.data(), WordOneSize);

			// Write the second word size and the second word
			size_t WordTwoSize = OldWordPair.WordTwo.size();
			SavedFile.write(reinterpret_cast<const char*>(&WordTwoSize), sizeof(size_t));
			SavedFile.write(OldWordPair.WordTwo.data(), WordTwoSize);
		}
	}

	// Write ProfileName
	size_t ProfileNameSize = CurrentProfile_.ProfileName.size();
	SavedFile.write(reinterpret_cast<const char*>(&ProfileNameSize), sizeof(size_t));
	SavedFile.write(CurrentProfile_.ProfileName.data(), ProfileNameSize);

	// Write ProfileScore
	SavedFile.write(reinterpret_cast<const char*>(&CurrentProfile_.ProfileScore), sizeof(int));

	// Write LettersOnly
	SavedFile.write(reinterpret_cast<const char*>(&CurrentProfile_.LettersOnly), sizeof(bool));

	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#endif
