
#ifdef VCFE_QT_JSON_PARSER_HPP
#define VCFE_QT_JSON_PARSER_HPP

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QTextStream>
#include <iostream>

class QtJsonSerializer {
private:
	QJsonObject card_tree;
	bool was_new_file_handled{ false };

private:
	void serializeNames(const NameRecord& names, QJsonObject& obj);
	void serializePhoneticName(const PhoneticRecord& phoneticNames, QJsonObject& obj);
	void serializeNickName(const NickNameRecord& nickname, QJsonObject& obj);
	void serializeTelephone(const TelephoneRecord& telephones, QJsonObject& obj);
	void serializeEmails(const EmailRecord& emails, QJsonObject& obj);
	void serializeAddress(const AddressRecord& addresses, QJsonObject& obj);
	void serializeEvent(const EventRecord& event, QJsonObject& obj);
	void serializeWorkInfo(const WorkInfoRecord& workInfo, QJsonObject& obj);
	void serializeUrl(const UrlRecord& url, QJsonObject& obj);
	void serializeNote(const NoteRecord& note, QJsonObject& obj);
	void serializeSocialNet(const SocialNetRecord& socialNet, QJsonObject& obj);
	void serializeRelation(const RelationRecord& relation, QJsonObject& obj);

	void deserializeNames(const QJsonObject& obj, NameRecord& names);
	void deserializePhonetic(const QJsonObject& obj, PhoneticRecord& phoneticNames);
	void deserializeNickName(const QJsonObject& obj, NickNameRecord& nickname);
	void deserializeWorkInfo(const QJsonObject& obj, WorkInfoRecord& workInfo);
	void deserializeNote(const QJsonObject& obj, NoteRecord& note);
	void deserializeTelephone(const QJsonObject& obj, TelephoneRecord& telephone);
	void deserializeEmail(const QJsonObject& obj, EmailRecord& email);
	void deserializeAddress(const QJsonObject& obj, AddressRecord& address);
	void deserializeUrl(const QJsonObject& obj, UrlRecord& url);
	void deserializeEvent(const QJsonObject& obj, EventRecord& event);
	void deserializeSocialNet(const QJsonObject& obj, SocialNetRecord& socialNet);
	void deserializeRelation(const QJsonObject& obj, RelationRecord& relation);

public:
	virtual ~QtJsonSerializer() = default;
	void serializeVcards(const std::vector<ContactData>& cards);
	void deserializeVcards(const QString& filename, std::vector<ContactData>& cards);

public:
	inline void unsetNewFileHandled() noexcept {
		was_new_file_handled = false;
	}

	inline void printJsonToFile(const QString& filename) const {
		QFile file(filename + ".json");
		if (!file.open(QIODevice::WriteOnly)) {
			return;
		}
		QJsonDocument doc(card_tree);
		QTextStream out(&file);
		out << doc.toJson(QJsonDocument::Indented);
		file.close();
	}

	inline void printJsonToConsole() const {
		QJsonDocument doc(card_tree);
		std::cout << QString(doc.toJson(QJsonDocument::Indented)).toStdString();
	}
};

#endif // VCFE_QT_JSON_PARSER_HPP
