#ifndef WDX_GAMECORE_H
#define WDX_GAMECORE_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <fstream>

namespace WDX {
enum class AccessPoint {
    kFromMainMenu,
    kGameStart,
    kFromGame
};

class GameCoreClass final {
private:
    GameCoreClass() noexcept;

    struct WordPair final {
        std::string WordOne;
        std::string WordTwo;

        inline WordPair(const std::string& first, const std::string& second) noexcept;
    };

    struct Profile final {
        std::unordered_map<std::string, std::vector<WordPair>> AllLoadedDecks;
        std::string ProfileName;
        int ProfileScore;
        bool LettersOnly;

        inline Profile() noexcept;
        inline Profile(std::string NewName, int NewScore, bool NewLettersOnly = false) noexcept;
    };

    Profile CurrentProfile_;
    std::unordered_set<std::string> DeckNamesAvailable_;
    std::unordered_set<std::string> DeckNamesLoaded_;
    const std::string kDirectoryDecks_{ "WRX_DECKS/" };
    const std::string kDirectoryProfiles_{ "WDX_PROFILES/" };
    const std::string kDeckFormat_{ ".txt" };

public:
    GameCoreClass(const GameCoreClass&) = delete;
    GameCoreClass& operator=(const GameCoreClass&) = delete;
    GameCoreClass(GameCoreClass&&) = delete;
    GameCoreClass& operator=(GameCoreClass&&) = delete;

    static GameCoreClass& GetCore() noexcept;

    void SetAccessPoint(AccessPoint Point) noexcept;
    void SetDeckCurrent() noexcept;
    void SetDelimiter(char w = '\t') noexcept;

    bool GetDirProfiles(std::filesystem::path& DirProfiles) const noexcept;
    bool GetDirDecks(std::filesystem::path& DirDecks) const noexcept;

    AccessPoint GetAccessPoint() const noexcept;
    const std::string& GetWordFirst() const noexcept;
    const std::string& GetWordSecond() const noexcept;
    const std::string& GetProfileName() const noexcept;
    const size_t GetCurrentDeckSize() const noexcept;
    const std::string GetCurrentDeckName() const noexcept;
    const std::vector<WordPair>* GetCurrentDeck() const noexcept;
    const int& GetScore() const noexcept;
    size_t GetLoadedDecksNum() const noexcept;

    void AddScorePoints(int ScoreChange) noexcept;
    void ResetGameCore() noexcept;
    bool MakeNewProfile(const std::string& NewName) noexcept;
    bool FormProfileList(std::vector<std::string>& ProfileNames) noexcept;

    bool SaveProfile() noexcept;
    bool LoadProfile(const std::string& ProfileName) noexcept;

    std::string GetWordHinted(uint8_t HintNum) const noexcept;

    void EraseWordPair() noexcept;
    void UpdateWordPair() noexcept;

    std::vector<std::string> CollectSelectedDecks(std::unordered_set<std::string>& SelectedDeckNames) noexcept;

    void UpdateDeckLists(
        std::unordered_set<std::string>& DeckNamesAvailable,
        std::unordered_set<std::string>& DeckNamesLoaded,
        bool AvailableOnly = false) noexcept;

    void SelectDeckCurrent(const std::string& DeckName) noexcept;

    const std::string kProfileFormat_{ ".bin64" };
    std::string CurrentWordOne_;
    std::string CurrentWordTwo_;
    std::string CurrentDeckName_;
    AccessPoint AccessWindow_;
    std::vector<WordPair>* CurrentDeck_;
    int CurrentDeckIndex_;
    char ProfileDelimiter_;

    std::vector<WordPair> ReadADeck(const std::string& FileName) noexcept;
};
}

#endif // WDX_GAMECORE_H
