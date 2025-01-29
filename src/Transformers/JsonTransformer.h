#ifndef JSON_TRANSFORMER_H
#define JSON_TRANSFORMER_H

#include <string>
#include <vector>
#include <Models/Category.h>
#include <Models/Entry.h>


class JsonTransformer {
public:
    std::string emptyJsonStructure();

    // Convertit un std::vector<Entry> en chaîne JSON
    std::string toJson(const std::vector<Entry>& vault);

    // Parse une chaîne JSON pour obtenir un std::vector<Entry>
    std::vector<Entry> fromJsonToEntries(const std::string& jsonContent);

    // Convertit un vecteur de catégories en chaîne JSON
    std::string toJson(const std::vector<Category>& categories);

    // Parse une chaîne JSON pour obtenir un vecteur de catégories
    std::vector<Category> fromJsonToCategories(const std::string& jsonContent);

    // Fusionne un vecteur d'entries et un vecteur de catégories en un seul JSON valide
    std::string mergeEntriesAndCategoriesToJson(const std::vector<Entry>& entries, const std::vector<Category>& categories);
};

#endif // JSON_TRANSFORMER_H
