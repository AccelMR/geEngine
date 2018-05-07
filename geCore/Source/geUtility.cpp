/*****************************************************************************/
/**
 * @file    geUtility.cpp
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

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geUtility.h"
#include "geRTTIType.h"
#include "geSceneObject.h"

namespace geEngineSDK {
  Vector<ResourceDependency>
  Utility::findResourceDependencies(IReflectable& obj, bool recursive) {
    Map<UUID, ResourceDependency> dependencies;
    findResourceDependenciesInternal(obj, recursive, dependencies);

    Vector<ResourceDependency> dependencyList(dependencies.size());
    uint32 i = 0;
    for (auto& entry : dependencies) {
      dependencyList[i] = entry.second;
      ++i;
    }

    return dependencyList;
  }

  uint32
  Utility::getSceneObjectDepth(const HSceneObject& so) {
    HSceneObject parent = so->getParent();

    uint32 depth = 0;
    while (nullptr != parent) {
      ++depth;
      parent = parent->getParent();
    }

    return depth;
  }

  void
  Utility::findResourceDependenciesInternal(IReflectable& obj,
                                            bool recursive,
                                            Map<UUID, ResourceDependency>& dependencies) {
    static const UnorderedMap<String, uint64> dummyParams;

    RTTITypeBase* rtti = obj.getRTTI();
    rtti->onSerializationStarted(&obj, dummyParams);

    uint32 numFields = rtti->getNumFields();
    for (uint32 i = 0; i < numFields; ++i) {
      RTTIField* field = rtti->getField(i);
      if ((field->getFlags() & RTTI_FIELD_FLAG::kSkipInReferenceSearch) != 0) {
        continue;
      }

      if (field->isReflectableType()) {
        RTTIReflectableFieldBase*
          reflectableField = static_cast<RTTIReflectableFieldBase*>(field);

        if (TYPEID_CORE::kID_ResourceHandle == reflectableField->getType()->getRTTIId()) {
          if (reflectableField->isArray()) {
            uint32 numElements = reflectableField->getArraySize(&obj);
            for (uint32 j = 0; j < numElements; ++j) {
              HResource resource = reinterpret_cast<HResource&>(
                                      reflectableField->getArrayValue(&obj, j));
              if (!resource.getUUID().empty()) {
                ResourceDependency& dependency = dependencies[resource.getUUID()];
                dependency.resource = resource;
                ++dependency.numReferences;
              }
            }
          }
          else {
            HResource resource = reinterpret_cast<HResource&>
                                    (reflectableField->getValue(&obj));
            if (!resource.getUUID().empty()) {
              ResourceDependency& dependency = dependencies[resource.getUUID()];
              dependency.resource = resource;
              ++dependency.numReferences;
            }
          }
        }
        else if (recursive) {
          //Optimization, no need to retrieve its value and go deeper if it has
          //no reflectable children that may hold the reference.
          if (hasReflectableChildren(reflectableField->getType())) {
            if (reflectableField->isArray()) {
              uint32 numElements = reflectableField->getArraySize(&obj);
              for (uint32 j = 0; j < numElements; ++j) {
                IReflectable& childObj = reflectableField->getArrayValue(&obj, j);
                findResourceDependenciesInternal(childObj, true, dependencies);
              }
            }
            else {
              IReflectable& childObj = reflectableField->getValue(&obj);
              findResourceDependenciesInternal(childObj, true, dependencies);
            }
          }
        }
      }
      else if (field->isReflectablePtrType() && recursive) {
        RTTIReflectablePtrFieldBase*
          reflectablePtrField = static_cast<RTTIReflectablePtrFieldBase*>(field);

        //Optimization, no need to retrieve its value and go deeper if it has
        //no reflectable children that may hold the reference.
        if (hasReflectableChildren(reflectablePtrField->getType())) {
          if (reflectablePtrField->isArray()) {
            uint32 numElements = reflectablePtrField->getArraySize(&obj);
            for (uint32 j = 0; j < numElements; ++j) {
              SPtr<IReflectable> childObj = reflectablePtrField->getArrayValue(&obj, j);

              if (nullptr != childObj) {
                findResourceDependenciesInternal(*childObj, true, dependencies);
              }
            }
          }
          else {
            SPtr<IReflectable> childObj = reflectablePtrField->getValue(&obj);

            if (nullptr != childObj) {
              findResourceDependenciesInternal(*childObj, true, dependencies);
            }
          }
        }
      }
    }

    rtti->onSerializationEnded(&obj, dummyParams);
  }

  Vector<HComponent>
  Utility::findComponents(const HSceneObject& object, uint32 typeId) {
    Vector<HComponent> output;

    Stack<HSceneObject> todo;
    todo.push(object);

    while (!todo.empty()) {
      HSceneObject curSO = todo.top();
      todo.pop();

      const Vector<HComponent>& components = curSO->getComponents();
      for (auto& entry : components) {
        if (entry->getRTTI()->getRTTIId() == typeId) {
          output.push_back(entry);
        }
      }

      uint32 numChildren = curSO->getNumChildren();
      for (uint32 i = 0; i < numChildren; ++i) {
        todo.push(curSO->getChild(i));
      }
    }

    return output;
  }

  bool
  Utility::hasReflectableChildren(RTTITypeBase* type) {
    uint32 numFields = type->getNumFields();
    for (uint32 i = 0; i < numFields; ++i) {
      RTTIField* field = type->getField(i);
      if (field->isReflectableType() || field->isReflectablePtrType()) {
        return true;
      }
    }

    const Vector<RTTITypeBase*>& derivedClasses = type->getDerivedClasses();
    for (auto& derivedClass : derivedClasses) {
      numFields = derivedClass->getNumFields();
      for (uint32 i = 0; i < numFields; ++i) {
        RTTIField* field = derivedClass->getField(i);
        if (field->isReflectableType() || field->isReflectablePtrType()) {
          return true;
        }
      }
    }

    return false;
  }
}
