
#include <Core/DataLoader.h>

Array<GigaObject*> DataLoader::GetObjects(std::string table) {
    std::map<std::string, std::string> search;
    return(this->GetObjects(table, search));
}

Array<DataRecord*> DataLoader::GetRecords(std::string table) {
    std::map<std::string, std::string> search;
    return(this->GetRecords(table, search));
}
