#pragma once

#include <iosfwd>
#include <sstream>

enum class EUserStates : uint8_t
{
    DeleteGame,
    AddGame,
    AdminAdd,
    AdminRemove,
    None
};

enum class EUserRole : uint8_t {
    Guest,
    User,
    Admin,
    None
};

namespace GlobalSettings
{
    constexpr const char* BotToken = "7587798634:AAFqGAZ_KrGnBQu21xoe2mBSMjRrvp72Mes";
    constexpr const char* ManagedGroup ("@GGcafeNhaTrang");
} // namespace GlobalSettings

namespace TgBotUtilities
{
    inline std::string GetUserLink(const int64_t UserId)
    {
        std::ostringstream Oss;
        Oss << "<a href=\"tg://user?id=" << UserId << "\">" << UserId << "</a>";
        return Oss.str();
    }

    inline std::string ToString(const auto Num)
    {
        std::ostringstream Stream;
        Stream << Num;
        return Stream.str();
    }

    inline int64_t StringToInt64(const std::string& MsgText, std::string& ErrMsg)
    {
        ErrMsg.clear();

        int64_t NewAdminId = 0;

        try { NewAdminId = std::stoll(MsgText); }
        catch (const std::exception& Exn)
        {
            ErrMsg = ErrMsg.append(Exn.what()).append("\n");
        }

        return NewAdminId;
    }

}

