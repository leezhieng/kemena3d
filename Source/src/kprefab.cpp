#include "kprefab.h"

#include <fstream>
#include <iostream>

namespace kemena
{
    kPrefab::kPrefab()
    {
        uuid = generateUuid();
    }

    kPrefab::~kPrefab() = default;

    bool kPrefab::loadFromFile(const kString &path)
    {
        std::ifstream f(path);
        if (!f.is_open())
        {
            std::cerr << "kPrefab::loadFromFile: cannot open " << path << "\n";
            return false;
        }

        try
        {
            json data = json::parse(f);
            uuid     = data.value("uuid", generateUuid());
            name     = data.value("name", kString(""));
            rootJson = data.contains("root") ? data["root"] : json::object();
            return true;
        }
        catch (const std::exception &e)
        {
            std::cerr << "kPrefab::loadFromFile: " << e.what() << "\n";
            return false;
        }
    }

    bool kPrefab::saveToFile(const kString &path)
    {
        json data = {
            {"type", "prefab"},
            {"uuid", uuid},
            {"name", name},
            {"root", rootJson},
        };

        std::ofstream f(path);
        if (!f.is_open())
        {
            std::cerr << "kPrefab::saveToFile: cannot open " << path << "\n";
            return false;
        }
        f << data.dump(4);
        return true;
    }

    static void renameUuidsRecursive(json &node,
                                     std::unordered_map<kString, kString> *map)
    {
        kString templateUuid = node.value("uuid", kString(""));
        kString freshUuid    = generateUuid();
        node["uuid"]          = freshUuid;
        node["template_uuid"] = templateUuid;
        if (map && !templateUuid.empty())
            (*map)[templateUuid] = freshUuid;

        if (node.contains("children") && node["children"].is_array())
        {
            for (auto &child : node["children"])
                renameUuidsRecursive(child, map);
        }
    }

    json kPrefab::instantiateJson(const json &source,
                                  std::unordered_map<kString, kString> *templateMap)
    {
        json copy = source;
        renameUuidsRecursive(copy, templateMap);
        return copy;
    }
}
