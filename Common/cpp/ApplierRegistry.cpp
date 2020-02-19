//
// Created by Szymon Kapala on 2020-02-12.
//

#include<ApplierRegistry.h>

#include <android/log.h>
#define APPNAME "NATIVE_REANIMATED"

void ApplierRegistry::registerApplierForRender(int id, std::shared_ptr<Applier> applier) {
  renderAppliers[id] = applier;
}

void ApplierRegistry::unregisterApplierFromRender(int id) {
  if (renderAppliers.find(id) != renderAppliers.end()) {
    renderAppliers.erase(id);
  }
}

void ApplierRegistry::registerApplierForEvent(int id, std::string eventName, std::shared_ptr<Applier> applier) {
  __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "rejestruje: %s  id: %d", eventName.c_str(), id);
  eventMapping[id] = eventName;
  eventAppliers[eventName][id] = applier;
}

void ApplierRegistry::unregisterApplierFromEvent(int id) {
  __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "odrejestro: %d", id);
  if (eventMapping.find(id) == eventMapping.end()) return;
  std::string eventName = eventMapping[id];
  eventMapping.erase(id);
  eventAppliers[eventName].erase(id);
}

bool ApplierRegistry::notEmpty() {
  return renderAppliers.size() > 0;
}

void ApplierRegistry::render(jsi::Runtime &rt, std::shared_ptr<jsi::HostObject> module) {
  std::vector<int> idsToRemove;
  for (auto & p : renderAppliers) {
    int id = p.first;
    auto & applier = p.second;
    if (applier->apply(rt, module)) {
      idsToRemove.push_back(id);
    }
  }

  for (auto id : idsToRemove) {
    unregisterApplierFromRender(id);
  }
}

void ApplierRegistry::event(jsi::Runtime &rt, std::string eventName, std::shared_ptr<jsi::HostObject> module) {
  __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "jest3");
  std::vector<int> idsToRemove;
  for (auto & p : eventAppliers[eventName]) {
    int id = p.first;
    auto & applier = p.second;
    if (applier->apply(rt, module)) {
      idsToRemove.push_back(id);
    }
  }

  for (auto id : idsToRemove) {
    unregisterApplierFromEvent(id);
  }
};

bool ApplierRegistry::anyApplierRegisteredForEvent(std::string eventName) {
  __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "czy jest: %s %d", eventName.c_str(), (int)eventAppliers[eventName].size());
  return eventAppliers[eventName].size() > 0;
}


