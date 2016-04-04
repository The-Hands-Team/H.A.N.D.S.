#ifndef GHAND_HPP
#define GHAND_HPP

class GHand
{
public:
    GHand(irr::scene::ISceneManager*);
    ~GHand();
    void setXYZ(float x,float y, float z);
    void copyHand(Hand h);
    void setVisible(bool vis);
    void setTexture(irr::ITexture*)
private:

    irr::ISceneNode* node;
};

#endif
