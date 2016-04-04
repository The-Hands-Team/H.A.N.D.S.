
GHand::GHand(irr::scene::ISceneManager* smgr)
{
    node = smgr->addSphereSceneNode();
    node->setVisible(false);
    node->setPosition(irr::core::vector3df(-1,-1,-1));
}

GHand::~GHand()
{
    node->remove();
}

GHand::setXYZ()
{

}

GHand::setTexture()
{

}

GHand::copyHand()
{

}
