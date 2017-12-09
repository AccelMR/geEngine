/*****************************************************************************/
/**
 * @file    geGameObjectManager.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/12/06
 * @brief   Tracks GameObject creation and destructions.
 *
 * Tracks GameObject creation and destructions. Also resolves GameObject
 * references from GameObject handles.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesCore.h"
#include "geGameObject.h"
#include <geModule.h>

namespace geEngineSDK {
  /**
   * @brief Possible modes to use when deserializing games objects.
   */
  enum GameObjectHandleDeserializationMode
  {
    /**
     * All handles will point to old ID that were restored from the
     * deserialized file.
     */
    GODM_UseOriginalIds = 0x01,

    /**
     * All handles will point to new IDs that were given to the deserialized
     * GameObjects.
     */
    GODM_UseNewIds = 0x02,

    /**
     * Handles pointing to GameObjects outside of the currently deserialized
     * set will attempt to be restored in case those objects are still active.
     */
    GODM_RestoreExternal = 0x04,

    /**
     * Handles pointing to GameObjects outside of the currently deserialized
     * set will be broken.
     */
    GODM_BreakExternal = 0x08,

    /**
     * Handles pointing to GameObjects that cannot be found will not be set to
     * null.
     */
    GODM_KeepMissing = 0x10
  };

  /**
   * @brief Tracks GameObject creation and destructions. Also resolves
   *        GameObject references from GameObject handles.
   * @note  Sim thread only.
   */
  class GE_CORE_EXPORT GameObjectManager : public Module<GameObjectManager>
  {
    /**
     * @brief Contains data for an yet unresolved game object handle.
     */
    struct UnresolvedHandle
    {
      uint64 originalInstanceId;
      GameObjectHandleBase handle;
    };

   public:
    GameObjectManager();
    ~GameObjectManager();

    /**
     * @brief Registers a new GameObject and returns the handle to the object.
     * @param[in] object      Constructed GameObject to wrap in the handle and
     *            initialize.
     * @param[in] originalId  If the object is being created due to
     *            deserialization you must provide the original object's ID so
     *            that deserialized handles can map to it properly.
     * @return  Handle to the GameObject.
     */
    GameObjectHandleBase
    registerObject(const SPtr<GameObject>& object, uint64 originalId = 0);

    /**
     * @brief Unregisters a GameObject. Handles to this object will no longer
     *        be valid after this call. This should be called whenever a
     *        GameObject is destroyed.
     */
    void
    unregisterObject(GameObjectHandleBase& object);

    /**
     * @brief Attempts to find a GameObject handle based on the GameObject
     *        instance ID. Returns empty handle if ID cannot be found.
     */
    GameObjectHandleBase
    getObject(uint64 id) const;

    /**
     * @brief Attempts to find a GameObject handle based on the GameObject
     *        instance ID. Returns true if object with the specified ID is
     *        found, false otherwise.
     */
    bool
    tryGetObject(uint64 id, GameObjectHandleBase& object) const;

    /**
     * @brief Checks if the GameObject with the specified instance ID exists.
     */
    bool
    objectExists(uint64 id) const;

    /**
     * @brief Changes the instance ID by which an object can be retrieved by.
     * @note  Caller is required to update the object itself with the new ID.
     */
    void
    remapId(uint64 oldId, uint64 newId);

    /**
     * @brief Queues the object to be destroyed at the end of a GameObject
     *        update cycle.
     */
    void
    queueForDestroy(const GameObjectHandleBase& object);

    /**
     * @brief Destroys any GameObjects that were queued for destruction.
     */
    void
    destroyQueuedObjects();

    /**
     * @brief Triggered when a game object is being destroyed.
     */
    Event<void(const HGameObject&)> onDestroyed;

    /*************************************************************************/
    /**
     * Deserialization
     * Note:  GameObjects need a bit of special handling when it comes to
     *        deserialization, which is what this part of the code is used for.
     *        It performs two main actions:
     *        - 1. Resolves all GameObjectHandles on deserialization
     *        - We can't just resolve them as we go because during
     *          deserialization not all objects have necessarily been created.
     *        - 2. Maps serialized IDs to actual in-engine IDs.
     */
    /*************************************************************************/

    /**
     * @brief Needs to be called whenever GameObject deserialization starts.
     *        Must be followed by endDeserialization() call.
     */
    void
    startDeserialization();

    /**
     * @brief Needs to be called whenever GameObject deserialization ends.
     *        Must be preceded by startDeserialization() call.
     */
    void endDeserialization();

    /**
     * @brief Returns true if GameObject deserialization is currently in
     *        progress.
     */
    bool
    isGameObjectDeserializationActive() const {
      return m_isDeserializationActive;
    }

    /**
     * @brief Queues the specified handle and resolves it when deserialization
     *        ends.
     */
    void
    registerUnresolvedHandle(uint64 originalId, GameObjectHandleBase& object);

    /**
     * @brief Registers a callback that will be triggered when GameObject
     *        serialization ends.
     */
    void
    registerOnDeserializationEndCallback(std::function<void()> callback);

    /**
     * @brief Changes the deserialization mode for any following GameObject
     *        handle.
     * @param[in] gameObjectDeserializationMode	Mode that controls how are
     *            GameObjects handles resolved when being deserialized.
     */
    void
    setDeserializationMode(uint32 gameObjectDeserializationMode);

    /**
     * @brief Attempts to update the ID of the provided handle by mapping its
     *        old ID to the newly deserialized object and its new ID. Game
     *        object deserialization must be active.
     */
    void
    resolveDeserializedHandle(UnresolvedHandle& data, uint32 flags);

    /**
     * @brief Gets the currently active flags that control how are game object
     *        handles deserialized.
     */
    uint32
    getDeserializationFlags() const {
      return m_goDeserializationMode;
    }

   private:
    uint64 m_nextAvailableID; //0 is not a valid ID
    Map<uint64, GameObjectHandleBase> m_objects;
    Map<uint64, GameObjectHandleBase> m_queuedForDestroy;

    GameObject* m_activeDeserializedObject;
    bool m_isDeserializationActive;
    Map<uint64, uint64> m_idMapping;
    Map<uint64, SPtr<GameObjectHandleData>> m_unresolvedHandleData;
    Vector<UnresolvedHandle> m_unresolvedHandles;
    Vector<std::function<void()>> m_endCallbacks;
    uint32 m_goDeserializationMode;
  };
}
