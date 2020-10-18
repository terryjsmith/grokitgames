
#include <IO/Serializer.h>

unsigned char* Serializer::Serialize(DataRecord *record, uint32_t& size) {
    size = GetBinarySize(record);
    unsigned char* data = new unsigned char[size];
    MemoryWriter* writer = new MemoryWriter();
    writer->Initialize(data, size);
    
    Array<std::string> keys = record->GetKeys();
    auto it = keys.begin();
    for(; it != keys.end(); it++) {
        Variant* var = record->Get(*it);
        
        // Type
        uint32_t type = var->GetType();
        writer->Write(&type, sizeof(uint32_t));
        
        // String length
        uint32_t length = (*it).length();
        writer->Write(&length, sizeof(uint32_t));
        
        // String
        const char* str = (*it).c_str();
        writer->Write((void*)str, length);
        
        // Data size
        uint32_t size = var->Size();
        writer->Write(&size, sizeof(uint32_t));
        
        // Data
        void* v = var->GetPtr();
        writer->Write(v, var->Size());
    }
    
    return(data);
}

void Serializer::Deserialize(DataRecord* record, unsigned char* data, uint32_t& size) {
    MemoryReader* reader = new MemoryReader();
    reader->Initialize(data, size);
    
    while(reader->GetPosition() < size) {
        // Type
        uint32_t type = 0;
        reader->Read(&type, sizeof(uint32_t));
        
        // String length
        uint32_t length = 0;
        reader->Read(&length, sizeof(uint32_t));
        
        // String
        char* str = new char[length + 1];
        reader->Read((void*)&str, length);
        str[length] = '\0';
        delete[] str;
        
        // Data size
        uint32_t size = 0;
        reader->Read(&size, sizeof(uint32_t));
        
        // Data
        unsigned char* data = (unsigned char*)malloc(size);
        reader->Read(data, size);
        
        Variant* v = new Variant();
        v->FromBytes(data, type, size);
        
        record->Set(str, v);
    }
}

unsigned int Serializer::GetBinarySize(DataRecord* record) {
    unsigned int size = 0;
    
    Array<std::string> keys = record->GetKeys();
    auto it = keys.begin();
    for(; it != keys.end(); it++) {
        Variant* var = record->Get(*it);
        
        size += sizeof(uint32_t);   // Type
        size += sizeof(uint32_t);   // String length
        size += (*it).length();     // String
        size += var->Size();        // Data
    }
    
    return(size);
}

