/*****************************************************************************/
/**
 * @file    geUtility.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/12/01
 * @brief   Static class containing various utility methods that do not fit
 *          anywhere else.
 *
 * Static class containing various utility methods that do not fit anywhere
 * else.
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

namespace geEngineSDK {
  /**
   * @brief Contains information about a resource dependency, including the
   *        dependant resource and number of references to it.
   */
  struct ResourceDependency
  {
    ResourceDependency() : numReferences(0) {}
    HResource resource;
    uint32 numReferences;
  };

  /**
   * @brief Static class containing various utility methods that do not fit
   *        anywhere else.
   */
  class GE_CORE_EXPORT Utility
  {
   public:
    /**
     * @brief Finds all resources referenced by the specified object.
     * @param[in] object    Object to search for resource dependencies.
     * @param[in] recursive Determines whether or not child objects will also
     *            be searched (if the object has any children).
     * @return  A list of unique, non-null resources.
     */
    static Vector<ResourceDependency>
    findResourceDependencies(IReflectable& object, bool recursive = true);

    /**
     * @brief Finds all components of a specific type on a scene object and any
     *        of its children.
     * @param[in] object  Object which to search for components. All children
     *            will be searched as well.
     * @param[in] typeId  RTTI type ID of the component type to search for.
     * @return  A list of all components of the specified type.
     */
    static Vector<HComponent>
    findComponents(const HSceneObject& object, uint32 typeId);

    /**
     * @brief Calculates how deep in the scene object hierarchy is the provided
              object. Zero means root.
     */
    static uint32
    getSceneObjectDepth(const HSceneObject& so);

   private:
    /**
     * @brief Helper method for recursion when finding resource dependencies.
     * @see findDependencies
     */
    static void
    findResourceDependenciesInternal(IReflectable& object,
                                     bool recursive,
                                     Map<UUID, ResourceDependency>& dependencies);

    /**
     * @brief Checks if the specified type (or any of its derived classes) have
     *        any IReflectable pointer or value types as their fields.
     */
    static bool
    hasReflectableChildren(RTTITypeBase* type);
  };
}
