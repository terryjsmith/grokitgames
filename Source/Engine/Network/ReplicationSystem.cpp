
#include <Network/ReplicationSystem.h>
#include <Core/MessageSystem.h>
#include <Core/Application.h>
#include <Core/TimeSystem.h>
#include <Network/NetworkSystem.h>
#include <Network/Messages/EntitySnapshotMessage.h>
#include <Network/Messages/CommandMessage.h>
#include <Core/DataRecord.h>
#include <Core/World.h>
#include <Network/Messages/MessageTypes.h>

ReplicationSystem::ReplicationSystem() {
    m_lastTick = 0;
    m_type = 0;
    m_initialized = false;
    m_replay = false;
    m_commandTick = 0;
    m_clientAuthoritative = false;
    m_clientShouldBeAuthoritative = false;
    m_checkPredictionErrors = false;
    m_activeCommands = 0;
    m_listening = false;
}

ReplicationSystem::~ReplicationSystem() {
    std::vector<EntitySnapshot*>::iterator i = m_snapshots.begin();
    for(; i != m_snapshots.end(); i++) {
        delete (*i);
    }
}

void ReplicationSystem::Initialize() {
    NetworkSystem* networkSystem = GetSystem<NetworkSystem>();
    networkSystem->RegisterMessageType<EntitySnapshotMessage>(Message_EntitySnapshotMessage);
    networkSystem->RegisterMessageType<CommandMessage>(Message_CommandMessage);
}

void ReplicationSystem::Update(float delta) {
    // Make sure the type is set
    if (m_type == 0) {
        return;
    }
    
    if(m_listening == false) {
        MessageSystem* messageSystem = GetSystem<MessageSystem>();
        messageSystem->RegisterCallback(this, "Command", &ReplicationSystem::CommandHandler);
        
        m_listening = true;
    }
    
    // Get the current tick
    NetworkSystem* networkSystem = GetSystem<NetworkSystem>();
    TimeSystem* timeSystem = GetSystem<TimeSystem>();
    int tick = timeSystem->GetCurrentTick();
    
    // If this is the server, prepare a sync to be sent to clients
    if (m_type == REPLICATION_SERVER) {
        // Get all entities
        World* world = World::GetInstance();
        Array<Entity*> entities = world->GetEntities();
        
        // Create an entity snapshot
        EntitySnapshot* snapshot = new EntitySnapshot();
        snapshot->tick = tick;
        snapshot->type = EntitySnapshot::SNAPSHOT_DELTA;
        
        EntitySnapshot* fullSnapshot = new EntitySnapshot();
        fullSnapshot->tick = tick;
        fullSnapshot->type = EntitySnapshot::SNAPSHOT_FULL;
        
        // Iterate over to find entities with updates
        auto i = entities.begin();
        for (; i != entities.end(); i++) {
            // First, take full entity snapshot
            DataRecord* dr = 0;
            auto it = m_records.find(*i);
            if(it != m_records.end()) {
                dr = it->second;
            }
            else {
                dr = new DataRecord();
            }
            
            fullSnapshot->AddEntity(*i);
            
            // Then do delta snapshot
            if (true) {
                bool entityAdded = false;
                Array<Component*> components = (*i)->GetComponents();
                for (size_t j = 0; j < components.size(); j++) {
                    DataRecord* record = 0;
                    auto it = m_records.find(*i);
                    if(it != m_records.end()) {
                        record = it->second;
                    }
                    else {
                        record = new DataRecord();
                    }
                    
                    fullSnapshot->AddComponent(components[j]);
                    
                    if (record->Updated()) {
                        if(entityAdded == false) {
                            snapshot->AddEntity(*i);
                            entityAdded = true;
                        }
                        
                        snapshot->AddComponent(components[j]);
                    }
                }
            }
        }
        
        snapshot->Serialize();
        fullSnapshot->Serialize();
        
        // Save
        AddSnapshot(tick, snapshot);
        AddFullSnapshot(tick, fullSnapshot);
        
        if (m_replay == false) {
            // Serialize
            int actualSize = 0;
            unsigned char* buffer = snapshot->GetData(actualSize);
            int offset = 0;
            
            EntitySnapshotMessage* msg = new EntitySnapshotMessage();
            msg->SetEntityPayload(buffer + offset, actualSize);
                
            networkSystem->Send(msg);
            delete msg;
            
            // Send our full snapshot to any sessions that need it
            if (m_sessionIDs.size()) {
                EntitySnapshotMessage* msg = new EntitySnapshotMessage();

                int size = 0;
                unsigned char* packet = fullSnapshot->GetData(size);
                msg->SetEntityPayload(packet, size);
                
                std::vector<int>::iterator s = m_sessionIDs.begin();
                for (; s != m_sessionIDs.end(); s++) {
                    networkSystem->Send(*s, msg);
                }
                
                delete msg;
                m_sessionIDs.clear();
            }
            
            // Do we have commands that need to be replayed?
            if (m_commandTick > 0) {
                // Delete all entities
                printf("Command tick %d, deleting all entities.\n", m_commandTick);
                
                // Reset back to previous tick
                world->Clear();
                
                // Re-populate the entity system with the state as it existed in the full snapshot from that tick
                EntitySnapshot* snapshot = GetFullEntitySnapshot(m_commandTick);
                snapshot->Deserialize();
                
                // Parse records
                MetaSystem* metaSystem = GetSystem<MetaSystem>();
                auto ei = snapshot->entityRecords.begin();
                for(; ei != snapshot->entityRecords.end(); ei++) {
                    GigaObject* obj = metaSystem->CreateClass(ei->second->objectType);
                    obj->DeserializeObject(ei->second);
                    
                    world->AddEntity((Entity*)obj);
                }
                
                // Loop over all ticks since then, replaying events
                MessageSystem* messageSystem = GetSystem<MessageSystem>();
                m_replay = true;
                std::vector<NetworkCommand*>::iterator ct = m_commandHistory.begin();
                for (int t = m_commandTick; t < tick; t++) {
                    while(true) {
                        if(ct == m_commandHistory.end() || (*ct)->tick > t)
                            break;
                        
                        // Get our entity
                        Entity* entity = world->FindEntity((*ct)->command->GetEntityID());
                        GIGA_ASSERT(entity != 0, "Entity not found.");
                        
                        if((*ct)->command->GetState() == Command::COMMAND_START) {
                            messageSystem->Broadcast(new Message("COMMAND_START", (*ct)->command));
                        }
                        else {
                            messageSystem->Broadcast(new Message("COMMAND_END", (*ct)->command));
                        }
                        
                        // Update last command ID processed
                        if ((*ct)->command->GetState() == Command::COMMAND_END) {
                            NetworkSession* session = networkSystem->FindSession((*ct)->sessionID);
                            session->lastCommandMessage = std::max(session->lastCommandMessage, (int)(*ct)->command->GetCommandID());
                        }

                    }
                    
                    timeSystem->SetTick(t);
                    float newDelta = (1.0f / GIGA_TICKS_PER_SECOND);
                    
                    printf("Current tick: %d, replaying tick %d with delta %f.\n", tick, t, newDelta);
                    Application::GetInstance()->Update(newDelta);
                    
                    timeSystem->SetTick(0);
                }
                
                m_replay = false;
                m_commandTick = 0;
                timeSystem->SetTick(0);
                printf("Replay complete.\n");
            }
            
            // Clean up old command history
            std::vector<NetworkCommand*>::iterator ct = m_commandHistory.begin();
            for (; ct != m_commandHistory.end(); ct++) {
                if ((*ct)->tick > (tick - NETWORK_SNAPSHOT_HISTORY)) {
                    break;
                }
                
                delete (*ct);
            }
            
            m_commandHistory.erase(m_commandHistory.begin(), ct);
        }
    }
    else {
        // If this is a client, process any updates that need to happen
        
        // Adjust our tick by a set amount of "render lag" so that we can interpolate
        int renderTick = tick - NETWORK_SNAPSHOT_RENDER_LAG;
        if (m_clientAuthoritative && m_clientShouldBeAuthoritative == false) {
            m_clientAuthoritative = false;
            m_checkPredictionErrors = true;
        }
        
        // If we are not yet initialized, check for a full snapshot first
        if(m_initialized == false && m_snapshots.size() > 0) {
            std::vector<EntitySnapshot*>::iterator i = m_snapshots.begin();
            for(; i != m_snapshots.end(); i++) {
                if((*i)->type == EntitySnapshot::SNAPSHOT_FULL) {
                    ApplySnapshot(*i, 0, 0);
                    m_initialized = true;
                }
                
                if (m_initialized == true) {
                    // Apply next snapshots up to expected render tick
                    if ((*i)->tick < renderTick) {
                        ApplySnapshot(*i, 0, 0);
                    }
                }
            }

            return;
        }
        
        if (m_initialized == false) {
            return;
        }
        
        // Check which snapshots we have to interpolate between
        int startTick = 0;
        int endTick = renderTick;
        std::vector<EntitySnapshot*>::iterator i = m_snapshots.end();
        std::vector<EntitySnapshot*>::iterator i2 = m_snapshots.end();
        
        if (m_snapshots.size() > 2) {
            i--;
            for (; i != m_snapshots.begin(); i--) {
                if ((*i)->tick <= renderTick) {
                    startTick = (*i)->tick;
                    break;
                }
                endTick = (*i)->tick;
                i2 = i;
            }
        }
        
        if(startTick > 0) {
            // Get the current offset from start tick
            float startTime = (float)startTick / GIGA_TICKS_PER_SECOND;
            float endTime = (float)endTick / GIGA_TICKS_PER_SECOND;
            float currentPos = timeSystem->GetCurrentTickTime() - (((float)tick - renderTick) / GIGA_TICKS_PER_SECOND) - startTime;
            float interpolate = currentPos / (endTime - startTime);
            if (interpolate < 0) {
                interpolate = 0;
            }
            if (interpolate > 1) {
                interpolate = 1;
            }
            
            if (i2 != m_snapshots.end()) {
                ApplySnapshot(*i, *i2, interpolate);
            }
            else {
                ApplySnapshot(*i, 0, 0);
            }
        }
    }
    
    if (m_replay == false) {
        // Finally, delete any "old" snapshots that are no longer needed
        if (m_snapshots.size()) {
            std::vector<EntitySnapshot*>::iterator i = m_snapshots.begin();
            if ((*i)->tick < (tick - NETWORK_SNAPSHOT_HISTORY)) {
                for (; i != m_snapshots.end(); i++) {
                    if ((*i)->tick > (tick - NETWORK_SNAPSHOT_HISTORY))
                        break;
                    else
                        delete (*i);
                }
                
                m_snapshots.erase(m_snapshots.begin(), i);
            }
        }
        
        // Same thing with full snapshots, delete any "old" snapshots that are no longer needed
        if (m_fullSnapshots.size()) {
            std::vector<EntitySnapshot*>::iterator i = m_fullSnapshots.begin();
            if ((*i)->tick < (tick - NETWORK_SNAPSHOT_HISTORY)) {
                for (; i != m_fullSnapshots.end(); i++) {
                    if ((*i)->tick > (tick - NETWORK_SNAPSHOT_HISTORY))
                        break;
                    else
                        delete (*i);
                }
                
                m_fullSnapshots.erase(m_fullSnapshots.begin(), i);
            }
        }
    }
    
    // Save current tick as most recent processed
    m_lastTick = tick;
}

DataRecord* ReplicationSystem::Interpolate(DataRecord* first, DataRecord* second, float interpolate) {
    // New data record
    DataRecord* newRecord = new DataRecord();
    newRecord->objectType = first->objectType;
    
    Array<std::string> fields = first->GetKeys();
    auto fi = fields.begin();
    for(; fi != fields.end(); fi++) {
        int fieldType = first->Get(*fi)->GetType();
        if(fieldType == Variant::VAR_INT32 || fieldType == Variant::VAR_INT64 || fieldType == Variant::VAR_UINT32 ||
           fieldType == Variant::VAR_UINT64) {
            int i1 = first->Get(*fi)->AsInt();
            int i2 = second->Get(*fi)->AsInt();
            
            newRecord->Set(*fi, new Variant(i1 + ((i2  - i1) * interpolate)));
            continue;
        }
        
        if(fieldType == Variant::VAR_FLOAT) {
            float i1 = first->Get(*fi)->AsFloat();
            float i2 = second->Get(*fi)->AsFloat();
            
            newRecord->Set(*fi, new Variant(i1 + ((i2 - i1) * interpolate)));
            continue;
        }
        
        if(fieldType == Variant::VAR_VECTOR2) {
            vector2 v1 = first->Get(*fi)->AsVector2();
            vector2 v2 = second->Get(*fi)->AsVector2();
            
            newRecord->Set(*fi, new Variant(v1 + ((v2 - v1)  * interpolate)));
            continue;
        }
        
        if(fieldType == Variant::VAR_VECTOR3) {
            vector3 v1 = first->Get(*fi)->AsVector3();
            vector3 v2 = second->Get(*fi)->AsVector3();
            
            newRecord->Set(*fi, new Variant(v1 + ((v2 - v1)  * interpolate)));
            continue;
        }
        
        if(fieldType == Variant::VAR_VECTOR4) {
            vector4 v1 = first->Get(*fi)->AsVector4();
            vector4 v2 = second->Get(*fi)->AsVector4();
            
            newRecord->Set(*fi, new Variant(v1 + ((v2 - v1)  * interpolate)));
            continue;
        }
        
        if(fieldType == Variant::VAR_QUATERNION) {
            quaternion q1 = first->Get(*fi)->AsQuaternion();
            quaternion q2 = second->Get(*fi)->AsQuaternion();
            
            quaternion result = glm::lerp(q1, q2, interpolate);
            newRecord->Set(*fi, new Variant(result));
            continue;
        }
        
        // For strings, bool, etc. just set to first value
        newRecord->Set(*fi, first->Get(*fi));
    }
    
    return(newRecord);
}

void ReplicationSystem::ApplySnapshot(EntitySnapshot* current, EntitySnapshot* next, float interpolate) {
    // Get link to entity system
    Application* application = Application::GetInstance();
    World* world = World::GetInstance();
    
    // Get our player entity ID
    NetworkSystem* networkSystem = GetSystem<NetworkSystem>();
    NetworkSession* session = networkSystem->FindSession(0);
    int playerID = session->playerID;
    
    world->Clear();
    
    // Update entities
    MetaSystem* metaSystem = GetSystem<MetaSystem>();
    uint32_t entityType = metaSystem->FindClass("Entity")->typeID;
    
    auto ei = current->entityRecords.begin();
    for(; ei != current->entityRecords.end(); ei++) {
        GigaObject* obj = metaSystem->CreateClass(ei->second->objectType);
        Entity* ent = (Entity*)obj;
        obj->DeserializeObject(ei->second);
        world->AddEntity(ent);
        
        // If no next snapshot, just apply current components
        if(next == 0) {
            // Apply components
            auto ci = current->componentRecords.begin();
            for(; ci != current->componentRecords.end(); ci++) {
                obj = metaSystem->CreateClass((*ci)->record->objectType);
                obj->DeserializeObject((*ci)->record);
            }
            
            continue;
        }
        
        /*
         TODO: need to put this back in, not update if entity is client authoritative
        if (ei->first == playerID) {
            if (m_clientAuthoritative) {
                continue;
            }
        }*/
        
        // If we have a next snapshot, need to find the same component in the next snapshot to see if we can interpolate
        bool interpolated = false;
        if(next) {
            auto ri = next->entityRecords.find(ent->entityID);
            if(ri == next->entityRecords.end()) continue;
            
            std::map<uint32_t, DataRecord*> nextComponents;
            auto cii = next->componentRecords.begin();
            for(; cii != next->componentRecords.end(); cii++) {
                if((*cii)->entityID == ent->entityID) {
                    nextComponents[(*cii)->record->objectType] = (*cii)->record;
                }
            }
            
            auto ci = current->componentRecords.begin();
            for(; ci != current->componentRecords.end(); ci++) {
                if((*ci)->entityID == ent->entityID) {
                    
                    
                    GigaObject* cobj = metaSystem->CreateClass((*ci)->record->objectType);
                    
                    auto ci2 = nextComponents.find((*ci)->record->objectType);
                    if(ci2 != nextComponents.end()) {
                        // Interpolate
                        DataRecord* newrec = Interpolate((*ci)->record, ci2->second, interpolate);
                        cobj->DeserializeObject(newrec);
                    }
                    else {
                        // Apply, no match in next
                        cobj->DeserializeObject((*ci)->record);
                    }
                }
            }
        }
    }
}

void ReplicationSystem::AddSnapshot(int tick, EntitySnapshot* snapshot) {
    // Check for empty list
    if (m_snapshots.size() == 0) {
        m_snapshots.push_back(snapshot);
        return;
    }
    
    // Check if this snapshot just goes at the end
    std::vector<EntitySnapshot*>::iterator i = m_snapshots.end();
    
    if(i != m_snapshots.begin()) {
        i--;
        // Otherwise, go through the list and find out where to insert
        for (; i != m_snapshots.begin(); i--) {
            if(tick == (*i)->tick) {
                // If we get an exact match, replace
                delete (*i);
                auto p = m_snapshots.erase(i);
                m_snapshots.insert(p, snapshot);
                return;
            }
        
            if (tick > (*i)->tick) {
                m_snapshots.insert(i, snapshot);
                return;
            }
        }
    }
    
    // If we get here, we to put it at the front of the list
    m_snapshots.insert(m_snapshots.begin(), snapshot);
}

void ReplicationSystem::AddFullSnapshot(int tick, EntitySnapshot* snapshot) {
    // Check for empty list
    if (m_fullSnapshots.size() == 0) {
        m_fullSnapshots.push_back(snapshot);
        return;
    }
    
    // Check if this snapshot just goes at the end
    std::vector<EntitySnapshot*>::iterator i = m_fullSnapshots.end();
    i--;
    
    // Otherwise, go through the list and find out where to insert
    for (; i != m_fullSnapshots.begin(); i--) {
        if (tick == (*i)->tick) {
            // If we get an exact match, replace
            delete (*i);
            auto p = m_fullSnapshots.erase(i);
            m_fullSnapshots.insert(p, snapshot);
            return;
        }
        
        if (tick >(*i)->tick) {
            m_fullSnapshots.insert(i, snapshot);
            return;
        }
    }
    
    // If we get here, we to put it at the front of the list
    m_fullSnapshots.insert(m_fullSnapshots.begin(), snapshot);
}

EntitySnapshot* ReplicationSystem::GetEntitySnapshot(int tick) {
    std::vector<EntitySnapshot*>::iterator i = m_snapshots.begin();
    EntitySnapshot* lastSnapshot = 0;
    
    // Try to find an exact match, barring that, take the earlier snapshot
    for (; i != m_snapshots.end(); i++) {
        if ((*i)->tick == tick) {
            return(*i);
        }
        
        // Gone too far, return last snapshot
        if((*i)->tick > tick) {
            return(lastSnapshot);
        }
        
        lastSnapshot = (*i);
    }
    
    return(0);
}

EntitySnapshot* ReplicationSystem::GetFullEntitySnapshot(int tick) {
    std::vector<EntitySnapshot*>::iterator i = m_fullSnapshots.begin();
    EntitySnapshot* lastSnapshot = 0;
    
    // Try to find an exact match, barring that, take the earlier snapshot
    for (; i != m_fullSnapshots.end(); i++) {
        if ((*i)->tick == tick) {
            return(*i);
        }
        
        // Gone too far, return last snapshot
        if((*i)->tick > tick) {
            return(lastSnapshot);
        }
        
        lastSnapshot = (*i);
    }
    
    return(0);
}

void ReplicationSystem::SendFullSnapshot(int sessionID) {
    m_sessionIDs.push_back(sessionID);
}

void ReplicationSystem::AddCommand(uint32_t tick, uint32_t sessionID, Command* command) {
    if (m_commandTick == 0) {
        m_commandTick = tick;
    }
    else {
        m_commandTick = std::min(m_commandTick, tick);
    }
    
    // Insert into command history for replay
    std::vector<NetworkCommand*>::iterator it = m_commandHistory.begin();
    for(; it != m_commandHistory.end(); it++) {
        if((*it)->tick > tick)
            break;
    }
    
    NetworkCommand* networkCommand = new NetworkCommand();
    networkCommand->tick = tick;
    networkCommand->sessionID = sessionID;
    networkCommand->command = command;
    
    m_commandHistory.insert(it, networkCommand);
}

void ReplicationSystem::SetClientAuthoritative(bool authoritative) {
    if (m_activeCommands != 0) {
        return;
    }
    
    m_clientShouldBeAuthoritative = authoritative;
}

Command* ReplicationSystem::GetCommand(uint32_t commandID) {
    std::vector<NetworkCommand*>::iterator it = m_commandHistory.begin();
    for(; it != m_commandHistory.end(); it++) {
        if((*it)->command->GetCommandID() == commandID) {
            return((*it)->command);
        }
    }
    
    return(0);
}

void ReplicationSystem::CommandHandler(GigaObject* object, Message* message) {
    ReplicationSystem* replicationSystem = GetSystem<ReplicationSystem>();
    
    // If this is on the server, never mind
    if (replicationSystem->m_type == REPLICATION_SERVER) {
        return;
    }
    
    NetworkSystem* networkSystem = GetSystem<NetworkSystem>();
    Command* command = (Command*)message;
    
    // If this is the client, send a command message to the server
    CommandMessage* cmdMessage = new CommandMessage();
    cmdMessage->SetCommand(command);
    
    networkSystem->Send(cmdMessage);
    
    if(command->GetState() == Command::COMMAND_START) {
        replicationSystem->m_activeCommands++;
        
        // New command started on client, don't process more snapshots until confirmed
        replicationSystem->m_clientAuthoritative = true;
        replicationSystem->m_clientShouldBeAuthoritative = true;
            
        return;
    }
    else {
        replicationSystem->m_activeCommands--;
    }
}
