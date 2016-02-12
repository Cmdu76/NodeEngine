#include "World.hpp"

NWorld NWorld::mInstance;

NWorld& NWorld::instance()
{
    return mInstance;
}

void NWorld::addEvent(sf::Event const& event)
{
    mInstance.mEvents.add(event);
}

/*
bool NWorld::testEvent(sf::Event const& event)
{
    for (std::size_t i = 0; i < mInstance.mEvents.size(); i++)
    {
        if (event == mInstance.mEvents[i])
        {
            return true;
        }
    }
    return false;
}*/

bool NWorld::testAction(NAction const& action)
{
    for (std::size_t i = 0; i < mInstance.mEvents.size(); i++)
    {
        if (action == mInstance.mEvents[i])
        {
            return true;
        }
    }
    return false;
}

void NWorld::tick(sf::Time dt)
{
    for (auto itr = mInstance.mTickables.begin(); itr != mInstance.mTickables.end(); itr++)
    {
        (*itr)->tick(dt);
    }
}

void NWorld::render(sf::RenderTarget& target)
{
    mInstance.mRenderables.sort([](NSceneComponent* a, NSceneComponent* b) -> bool
    {
       if (a != nullptr && b != nullptr)
       {
           if (a->getFinalZ() < b->getFinalZ())
           {
                return true;
           }
           else if (a->getFinalZ() > b->getFinalZ())
           {
               return false;
           }
           else
           {
               return (a->getFinalPosition().y < b->getFinalPosition().y);
           }
       }
       return true;
    });

    sf::View old = target.getView();
    target.setView(mInstance.mCameraManager.getActiveView());
    for (auto itr = mInstance.mRenderables.begin(); itr != mInstance.mRenderables.end(); itr++)
    {
        (*itr)->render(target);
    }
    target.setView(old);
}

void NWorld::update()
{
    // Events
    mInstance.mEvents.clear();

    // Renderables
    for (std::size_t i = 0; i < mInstance.mRenderablesAdditions.size(); i++)
    {
        mInstance.mRenderables.add(mInstance.mRenderablesAdditions[i]);
    }
    mInstance.mRenderablesAdditions.clear();
    for (std::size_t i = 0; i < mInstance.mRenderablesDeletions.size(); i++)
    {
        mInstance.mRenderables.remove(mInstance.mRenderablesDeletions[i]);
    }
    mInstance.mRenderablesDeletions.clear();

    // Tickables
    for (std::size_t i = 0; i < mInstance.mTickablesAdditions.size(); i++)
    {
        mInstance.mTickables.add(mInstance.mTickablesAdditions[i]);
    }
    mInstance.mTickablesAdditions.clear();
    for (std::size_t i = 0; i < mInstance.mTickablesDeletions.size(); i++)
    {
        mInstance.mTickables.remove(mInstance.mTickablesDeletions[i]);
    }
    mInstance.mTickablesDeletions.clear();

    // Actors
    for (std::size_t i = 0; i < mInstance.mActorsAdditions.size(); i++)
    {
        mInstance.mActors.add(mInstance.mActorsAdditions[i]);
    }
    mInstance.mActorsAdditions.clear();
    for (std::size_t i = 0; i < mInstance.mActorsDeletions.size(); i++)
    {
        mInstance.mActors.remove(mInstance.mActorsDeletions[i]);
    }
    mInstance.mActorsDeletions.clear();
}

void NWorld::removeActor(NActor::Ptr actor)
{
    mInstance.mActorsDeletions.add(actor);
}

NActor::Ptr NWorld::getActor(std::size_t index)
{
    if (index >= 0 && index < mInstance.mActors.size())
    {
        return mInstance.mActors[index];
    }
    return nullptr;
}

bool NWorld::load(std::string const& filename)
{
    pugi::xml_document doc;
    if (!doc.load_file(filename.c_str()))
    {
        return false;
    }
    pugi::xml_node actors = doc.child("Actors");
    if (!actors)
    {
        return false;
    }
    for (pugi::xml_node actor = actors.first_child(); actor; actor = actor.next_sibling())
    {
        std::string type = actor.attribute("type").value();
        if (mInstance.mActorFactory.contains(type))
        {
            auto a = mInstance.mActorFactory[type]();
            a->load(actor);
            a->NRootComponent::load(actor);
            mInstance.mActorsAdditions.add(a);
        }
    }

    return true;
}

bool NWorld::save(std::string const& filename)
{
    pugi::xml_document doc;
    pugi::xml_node actors = doc.append_child("Actors");
    std::size_t size = getActorCount();
    for (std::size_t i = 0; i < size; i++)
    {
        pugi::xml_node actor = actors.append_child("Actor");
        if (actor)
        {
            auto a = getActor(i);
            if (a != nullptr)
            {
                a->save(actor);
                a->NRootComponent::save(actor);
            }
        }
    }
    doc.save_file(filename.c_str());
    return true;
}

NCameraManager& NWorld::getCameraManager()
{
    return mInstance.mCameraManager;
}

std::size_t NWorld::getActorCount()
{
    return mInstance.mActors.size();
}

std::size_t NWorld::getRenderableCount()
{
    return mInstance.mRenderables.size();
}

std::size_t NWorld::getTickableCount()
{
    return mInstance.mTickables.size();
}

NVector NWorld::getMousePositionScreen()
{
    return NVector::SFML2FToN(getWindow().getMousePosition());
}

NVector NWorld::getMousePositionView()
{
    return NVector::SFML2FToN(getWindow().getMousePositionView(mInstance.mCameraManager.getActiveView()));
}

ah::ResourceManager& NWorld::getResources()
{
    return ah::Application::getResources();
}

ah::Window& NWorld::getWindow()
{
    return ah::Application::getWindow();
}

void NWorld::addRenderable(NSceneComponent* renderable)
{
    mRenderablesAdditions.add(renderable);
}

void NWorld::removeRenderable(NSceneComponent* renderable)
{
    mRenderablesDeletions.add(renderable);
}

void NWorld::addTickable(NTickable* tickable)
{
    mTickablesAdditions.add(tickable);
}

void NWorld::removeTickable(NTickable* tickable)
{
    mTickablesDeletions.add(tickable);
}

NWorld::NWorld()
{
}

NWorld::~NWorld()
{
}
