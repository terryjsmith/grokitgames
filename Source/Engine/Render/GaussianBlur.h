
#ifndef gaussianblur_h
#define gaussianblur_h

#include <Render/Filter.h>

class GIGA_API GaussianBlur : public Filter {
public:
    GaussianBlur();
    ~GaussianBlur();
    
    void Apply();
    
protected:
    // A temporary texture for "middle" step
    Texture2D* m_tempTexture;
};

#endif 
