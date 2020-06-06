
#include <IO/Profiler.h>
#include <IO/LogSystem.h>
#include <Core/Application.h>

Profiler* Profiler::m_instance = 0;

Profiler::Profiler() {
    m_currentArea = 0;
    m_rootArea = 0;
    m_timer = 0;
    m_maxAreaNameLength = 0;
    m_currentDepth = 0;
    m_frameTime = 0;
}

Profiler::~Profiler() {
    if (m_rootArea) {
        delete m_rootArea;
    }
    
    if (m_timer) {
        delete m_timer;
    }
}

Profiler::ProfileArea::ProfileArea() {
    avgFrameTime = 0;
    currentStartTime = 0;
    parent = 0;
}

Profiler::ProfileArea::~ProfileArea() {
    auto i = children.begin();
    for (; i != children.end(); i++) {
        delete(*i);
    }
}

Profiler* Profiler::GetInstance() {
    if (m_instance == 0) {
        m_instance = new Profiler();
    }
    
    return(m_instance);
}

void Profiler::Log() {
    std::string headerFn = "Function Name";
    std::string headerAvg = "Duration";
    m_maxAreaNameLength = std::max(m_maxAreaNameLength, (int)headerFn.length());
    
    char buffer[1000];
    int level = 1;
    sprintf(buffer, "% *c%- *s | %- 10s ms", level * 2, ' ', m_maxAreaNameLength - (level * 2), headerFn.c_str(), headerAvg.c_str());
    
    LogSystem* logSystem = GetSystem<LogSystem>();
    logSystem->Log(Error::MSG_INFO, buffer);
    
    RecursiveLog(level, m_rootArea);
    
    headerFn = "Avg. Frame Time";
    float avgFrame = m_frameTime;
    avgFrame *= 1000.0f;
    sprintf(buffer, "% *c%- *s | %- 10s ms", level * 2, ' ', m_maxAreaNameLength - (level * 2), headerFn.c_str(), std::to_string(avgFrame).c_str());
    
    logSystem->Log(Error::MSG_INFO, buffer);
}

void Profiler::RecursiveLog(int level, Profiler::ProfileArea* area) {
    char buffer[1000];
    sprintf(buffer, "% *c%- *s | % 10.4f ms", level * 2, ' ', m_maxAreaNameLength - (level * 2), area->areaName.c_str(), area->avgFrameTime);
    
    LogSystem* logSystem = GetSystem<LogSystem>();
    logSystem->Log(Error::MSG_INFO, buffer);
    
    auto i = area->children.begin();
    for (; i != area->children.end(); i++) {
        RecursiveLog(level + 1, *i);
    }
}

void Profiler::StartFrame() {
    // Make sure we have a root node
    if (m_rootArea == 0) {
        m_rootArea = new ProfileArea();
        m_rootArea->areaName = "root";
    }
    
    // Set it to our current node
    m_currentArea = m_rootArea;
    m_currentDepth = 0;
    
    // Get start time
    if (m_timer == 0) {
        m_timer = new Timer();
    }
    
    m_timer->Start();
}

void Profiler::EndFrame() {
    m_currentArea = 0;
    m_currentDepth = 0;
    
    if(m_frameTime) {
        m_frameTime += m_timer->Duration();
        m_frameTime /= 2.0f;
    }
    else {
        m_frameTime += m_timer->Duration();
    }
    
    m_timer->Reset();
}

void Profiler::StartArea(std::string name) {
    assert(m_currentArea != 0); // Must start a frame first
    
    m_currentDepth++;
    m_maxAreaNameLength = std::max(m_maxAreaNameLength, (int)name.length() + (2 * m_currentDepth) + 2);
    
    auto it = m_currentArea->children.begin();
    for(; it != m_currentArea->children.end(); it++) {
        if((*it)->areaName.compare(name) == 0)
            break;
    }
    
    if (it == m_currentArea->children.end()) {
        ProfileArea* area = new ProfileArea();
        area->areaName = name;
        m_currentArea->children.push_back(area);
        area->parent = m_currentArea;
        
        m_currentArea = area;
    }
    else {
        m_currentArea = (*it);
    }
    
    m_currentArea->currentStartTime = m_timer->Duration();
}

void Profiler::EndArea(std::string name) {
    if (m_currentArea->areaName != name) {
        printf("Current area name: %s\n", m_currentArea->areaName.c_str());
        assert(false); // You forgot to close a profiling section
    }
    
    float currentOffset = m_timer->Duration();
    float timeDiff = (currentOffset - m_currentArea->currentStartTime) * 1000.0f;
    
    if (m_currentArea->avgFrameTime) {
        m_currentArea->avgFrameTime = (m_currentArea->avgFrameTime + timeDiff) / 2.0f;
    }
    else {
        m_currentArea->avgFrameTime = timeDiff;
    }
    
    if (m_currentArea->parent) {
        m_currentArea = m_currentArea->parent;
    }
    else {
        m_currentArea = m_rootArea;
    }
    
    m_currentDepth--;
}
