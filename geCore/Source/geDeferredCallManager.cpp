/*****************************************************************************/
/**
 * @file    geDeferredCallManager.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/25
 * @brief   Allows you to queue calls that can get executed later.
 *
 * Allows you to queue calls that can get executed later.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geDeferredCallManager.h"

namespace geEngineSDK {
  void
  DeferredCallManager::queueDeferredCall(function<void()> func) {
    m_callbacks.push_back(func);
  }

  void
  DeferredCallManager::_update() {
    while (!m_callbacks.empty()) {
      //Copy because callbacks can be queued within callbacks
      Vector<function<void()>> callbackCopy = m_callbacks;
      m_callbacks.clear();
      for (auto& call : callbackCopy) {
        call();
      }
    }
  }

  void
  deferredCall(function<void()> callback) {
    DeferredCallManager::instance().queueDeferredCall(callback);
  }
}
