
#include <Network/EntitySnapshot.h>
#include <IO/MemoryWriter.h>
#include <IO/MemoryReader.h>
#include <Core/DataRecord.h>
#include <Network/NetworkSystem.h>
#include <Core/Meta.h>
#include <IO/Serializer.h>

EntitySnapshot::EntitySnapshot() {
    m_buffer = 0;
    m_bufferSize = 0;
}

EntitySnapshot::~EntitySnapshot() {
    auto ei = entityRecords.begin();
    for(; ei != entityRecords.end(); ei++) {
        delete(ei->second);
    }
    entityRecords.clear();
    
    auto ci = componentRecords.begin();
    for(; ci != componentRecords.end(); ci++) {
        delete (*ci)->record;
        delete *ci;
    }
    componentRecords.clear();
    
    if(m_buffer) {
        free(m_buffer);
        m_bufferSize = 0;
    }
}

void EntitySnapshot::Serialize() {
    if(m_buffer) {
        free(m_buffer);
        m_bufferSize = 0;
    }
    
    m_buffer = (unsigned char*)malloc(NETWORK_MAX_PACKET_SIZE);
    m_bufferSize = NETWORK_MAX_PACKET_SIZE;
    memset(m_buffer, 0, NETWORK_MAX_PACKET_SIZE);
    
    // Keep track of our current buffersize in a memory writer
    MemoryWriter* writer = new MemoryWriter();
    writer->Initialize(m_buffer, m_bufferSize);
    writer->Resizable(true, NETWORK_MAX_PACKET_SIZE);
    
    // Write type
    uint32_t t = type;
    writer->Write(&t, sizeof(uint32_t));
    
    // Write number of entities
    uint32_t count = entityRecords.size();
    writer->Write(&count, sizeof(uint32_t));
    
    // Iterate over entities first
    auto ei = entityRecords.begin();
    for(; ei != entityRecords.end(); ei++) {
        // Write entity
        uint32_t size = 0;
        unsigned char* data = Serializer::Serialize(ei->second, size);
        
        writer->Write((void*)&ei->first, sizeof(uint32_t));
        
        writer->Write(&size, sizeof(uint32_t));
        writer->Write(data, size);
        delete[] data;
    }
    
    // Number of components
    count = componentRecords.size();
    writer->Write(&count, sizeof(uint32_t));
    
    // Then components
    auto ci = componentRecords.begin();
    for(; ci != componentRecords.end(); ci++) {
        uint32_t size = 0;
        unsigned char* data = Serializer::Serialize((*ci)->record, size);
        
        uint32_t entityID = (*ci)->entityID;
        writer->Write(&entityID, sizeof(uint32_t));
        
        writer->Write(&size, sizeof(uint32_t));
        writer->Write(data, size);
        delete[] data;
    }
    
    m_bufferSize = writer->GetPosition();
    delete writer;
}

void EntitySnapshot::Deserialize() {
    // Deserialize buffer, start with reading data out
    MemoryReader* reader = new MemoryReader();
    reader->Initialize(m_buffer, m_bufferSize);
    
    // Read type
    uint32_t t = type;
    reader->Read(&t, sizeof(uint32_t));
    type = t;
    
    // Read number of entities
    uint32_t count = 0;
    reader->Read(&count, sizeof(uint32_t));
    
    // Read entities
    for(uint32_t i = 0; i < count; i++) {
        // Read binary size
        uint32_t size = 0;
        reader->Read(&size, sizeof(uint32_t));
        
        // Read entity ID
        uint32_t entityID = 0;
        reader->Read(&entityID, sizeof(uint32_t));
        
        // Read entity
        DataRecord* record = new DataRecord();
        Serializer::Deserialize(record, reader->GetCurrent(), size);
        entityRecords[entityID] = record;
        
        reader->SetPosition(reader->GetPosition() + size);
    }
    
    count = 0;
    reader->Read(&count, sizeof(uint32_t));
    
    // Read components
    for(uint32_t i = 0; i < count; i++) {
        // Read binary size
        uint32_t size = 0;
        reader->Read(&size, sizeof(uint32_t));
        
        // Read entity ID
        uint32_t entityID = 0;
        reader->Read(&entityID, sizeof(uint32_t));
        
        // Read entity
        DataRecord* record = new DataRecord();
        Serializer::Deserialize(record, reader->GetCurrent(), size);
        
        ComponentRecord* cr = new ComponentRecord();
        cr->entityID = entityID;
        cr->record = record;
        componentRecords.push_back(cr);
        
        reader->SetPosition(reader->GetPosition() + size);
    }
    
    delete reader;
}

unsigned char* EntitySnapshot::GetData(int& size) {
    size = m_bufferSize;
    return(m_buffer);
}

void EntitySnapshot::AddEntity(Entity* entity) {
    DataRecord* record = 0;
    auto ei = entityRecords.find(entity->entityID);
    if(ei != entityRecords.end()) {
        record = ei->second;
    }
    else {
        record = new DataRecord();
    }
    
    entity->SerializeObject(record);
    entityRecords[entity->entityID] = record;
}

void EntitySnapshot::AddComponent(Component* component) {
    DataRecord* record = new DataRecord();
    component->SerializeObject(record);
    
    ComponentRecord* cr = new ComponentRecord();
    cr->entityID = component->GetParent() == 0 ? 0 : component->GetParent()->entityID;
    cr->record = record;
    
    componentRecords.push_back(cr);
}

void EntitySnapshot::SetData(unsigned char* buffer, int size) {
    m_buffer = (unsigned char*)malloc(size);
    memcpy(m_buffer, buffer, size);
    m_bufferSize = size;
}
