#include <absl/container/flat_hash_map.h>
#include <bitset>
#include <cstdint>
#include <queue>
#include <spdlog/spdlog.h>
#pragma ONCE

namespace ntg::viz {
using entity_t = uint32_t;
const entity_t MAX_ENTITIES = 10000;

using component_t = uint32_t;
const component_t MAX_COMPONENTS = 100;

using signature_t = std::bitset<MAX_COMPONENTS>;

class IComponentArray {
public:
  virtual ~IComponentArray() = default;
  virtual void entityDestroyed(entity_t entity) = 0;
};

template <typename T> class ComponentArray : IComponentArray {
public:
  std::array<T, MAX_ENTITIES> mComponentArray{};

  absl::flat_hash_map<entity_t, size_t> mEntityToIndexMap{};

  absl::flat_hash_map<size_t, entity_t> mIndexToEntityMap{};

  size_t mSize;

public:
  // inline ComponentArray()
  //  : mComponentArray(std::array<T, MAX_ENTITIES>{})
  // {};

  inline void insertData(entity_t entity, T component) {
    if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end()) {
      spdlog::error("Tried adding same component to entity multiple times - "
                    "adding nothing");
      return;
    }

    size_t newIndex = mSize;
    mEntityToIndexMap[entity] = newIndex;
    mIndexToEntityMap[newIndex] = entity;
    mComponentArray[newIndex] = component;
    ++mSize;
  }

  inline void removeData(entity_t entity) {
    if (mEntityToIndexMap.find(entity) == mEntityToIndexMap.end()) {
      spdlog::error("Tried removing non-existent entity - removing nothing");
      return;
    }

    size_t indexOfRemovedEntity = mEntityToIndexMap[entity];
    size_t indexOfLastElement = mSize - 1;
    mComponentArray[indexOfRemovedEntity] = mComponentArray[indexOfLastElement];

    entity_t entityOfLastElement = mIndexToEntityMap[indexOfLastElement];
    mEntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
    mIndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

    mEntityToIndexMap.erase(entity);
    mIndexToEntityMap.erase(indexOfLastElement);

    --mSize;
  }

  inline T &getData(entity_t entity) {
    if (mEntityToIndexMap.find(entity) == mEntityToIndexMap.end()) {
      spdlog::error("Tried to retrieve data of non-existent entity");
      assert(false);
    }

    return mComponentArray[mEntityToIndexMap[entity]];
  }

  inline void entityDestroyed(entity_t entity) override {
    if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end()) {
      removeData(entity);
    }
  }
};

class ComponentManager {
public:
  template <typename T> inline void registerComponent() {
    const char *typeName = typeid(T).name();

    if (mComponentTypes.find(typeName) != mComponentTypes.end()) {
      spdlog::error("Tried to register already registered component type - not "
                    "registering anything");
      return;
    }

    mComponentTypes.insert({typeName, mNextComponentType});

    mComponentArrays.insert({typeName, reinterpret_cast<IComponentArray *>(
                                           new ComponentArray<T>())});

    ++mNextComponentType;
  }

  template <typename T> inline component_t getComponentType() {
    const char *typeName = typeid(T).name();

    if (mComponentTypes.find(typeName) == mComponentTypes.end()) {
      spdlog::error("Tried to access unregistered component!");
      assert(false);
    }

    return mComponentTypes[typeName];
  }

  template <typename T> inline void addComponent(entity_t entity, T component) {
    getComponentArray<T>()->insertData(entity, component);
  }

  template <typename T> inline void removeComponent(entity_t entity) {
    getComponentArray<T>()->removeData(entity);
  }

  template <typename T> inline T &getComponent(entity_t entity) {
    return getComponentArray<T>()->getData(entity);
  }

  inline void entityDestroyed(entity_t entity) {
    for (auto const &pair : mComponentArrays) {
      auto const &component = pair.second;
      component->entityDestroyed(entity);
    }
  }

  inline ~ComponentManager() {
    for (auto const &pair : mComponentArrays) {
      delete pair.second;
    }
  }

public:
  absl::flat_hash_map<const char *, component_t> mComponentTypes{};

  absl::flat_hash_map<const char *, IComponentArray *> mComponentArrays{};

  component_t mNextComponentType{};

  template <typename T> ComponentArray<T> *getComponentArray() {
    const char *typeName = typeid(T).name();

    if (mComponentTypes.find(typeName) == mComponentTypes.end()) {
      spdlog::error("Tried to use unregistered component!");
      assert(false);
    }

    // return static_cast<ComponentArray<T>*>(mComponentArrays[typeName]);
    // ComponentArray<T>* temp = mComponentArrays[typeName];
    // return temp;
    return reinterpret_cast<ComponentArray<T> *>(mComponentArrays[typeName]);
  }
};

class EntityManager {
public:
  inline EntityManager() {
    for (entity_t e = 0; e < MAX_ENTITIES; e++) {
      mAvailableEntities.push(e);
    }
  }

  inline entity_t createEntity() {
    if (mLivingEntityCount >= MAX_ENTITIES) {
      spdlog::error(
          "Tried to create new entity, when no more entities are available");
      assert(false);
    }
    entity_t id = mAvailableEntities.front();
    mAvailableEntities.pop();
    ++mLivingEntityCount;
    mExistingEntities.insert(id);
    return id;
  }

  inline void destroyEntity(entity_t entity) {
    if (entity >= MAX_ENTITIES) {
      spdlog::error("Tried to delete out-of-range entity - deleting nothing");
      return;
    }

    mSignatures[entity].reset();
    mExistingEntities.erase(entity);

    // May want to add check if entity is alive
    mAvailableEntities.push(entity);
    --mLivingEntityCount;
  }

  inline void setSignature(entity_t entity, signature_t signature) {
    if (entity >= MAX_ENTITIES) {
      spdlog::error("Tried to change signature of out-of-range entity - "
                    "changing nothing");
      return;
    }

    mSignatures[entity] = signature;
  }

  inline signature_t getSignature(entity_t entity) {
    if (entity >= MAX_ENTITIES) {
      spdlog::error("Tried to get signature of out-of-range entity");
      assert(true);
    }
    return mSignatures[entity];
  }

public:
  std::queue<entity_t> mAvailableEntities{}; // Unused entity ID's
  std::set<entity_t> mExistingEntities{};    // Uesd entity ID's
  std::array<signature_t, MAX_ENTITIES>
      mSignatures{}; // Signatures corresponding to Entities
  uint32_t mLivingEntityCount{};
};

class System {
public:
  std::set<entity_t> mEntities;

  virtual void entityRegistered(entity_t entity) {}

  virtual void entityErased(entity_t entity) {}
};

class SystemManager {
public:
  template <typename T, typename... Args>
  inline T *registerSystem(Args... args) {
    const char *typeName = typeid(T).name();

    if (mSystems.find(typeName) != mSystems.end()) {
      spdlog::error("Tried registering a system multiple times!");
      assert(false);
    }

    T *system = new T(args...);
    mSystems.insert({typeName, (System *)(system)}); // ? delete cast
    return system;
  }

  template <typename T> inline void setSignature(signature_t signature) {
    const char *typeName = typeid(T).name();
    if (mSystems.find(typeName) == mSystems.end()) {
      spdlog::error(
          "Tried setting Signature for unregistered System - setting nothing");
      return;
    }

    mSignatures.insert({typeName, signature});
  }

  inline void entityDestroyed(entity_t entity) {
    for (auto const &pair : mSystems) {
      auto const &system = pair.second;

      system->mEntities.erase(entity);
    }
  }

  inline void entitySignatureChanged(entity_t entity, signature_t signature) {
    for (auto const &pair : mSystems) {
      auto const &type = pair.first;
      auto const &system = pair.second;
      auto const &systemSignature = mSignatures[type];

      if ((signature & systemSignature) == systemSignature) {
        system->mEntities.insert(entity);
        system->entityRegistered(entity);
      } else {
        system->mEntities.erase(entity);
        system->entityErased(entity);
      }
    }
  }

  inline ~SystemManager() {
    for (auto const &pair : mSystems) {
      delete pair.second;
    }
  }

public:
  absl::flat_hash_map<const char *, signature_t> mSignatures{};
  absl::flat_hash_map<const char *, System *> mSystems{};
};

class IResourceArray {};

template <typename T> class ResourceArray : IResourceArray {
public:
  inline T *&getResource(const char *key) {
    absl::string_view view = key;
    return data[view];
  }
  inline T *&getResource(std::string key) { return getResource(key.c_str()); }

  inline void setResource(const char *key, T *value) {
    absl::string_view view = key;
    data[view] = value;
  }
  inline void setResource(std::string key, T *value) {
    setResource(key.c_str(), value);
  }

  inline void deleteResource(const char *key) {
    absl::string_view view = key;
    // delete data[view];
    data.erase(view);
  }
  inline void deleteResource(std::string key) { deleteResource(key.c_str()); }

  inline void deleteAll() {
    // for (auto& d : data)
    // {
    //   // T*& temp;
    //   if(d.second) delete d.second;
    //   // absl::string_view view = d.first;
    //   // data.erase(view);
    //   // delete temp;
    // }
    data.clear();
  }

  inline T *&operator[](const char *key) {
    absl::string_view view = key;
    return data[view];
  }
  inline T *&operator[](std::string key) { return this[key.c_str()]; }

private:
  absl::flat_hash_map<std::string, T *> data;
};

class ResourceManager {
public:
  template <typename T> inline void registerResourceType() {
    const char *name = typeid(T).name();
    if (resourceArrays.find(name) != resourceArrays.end()) {
      spdlog::error(
          "Tried to register resource multiple times - registering nothing");
      return;
    }
    resourceArrays.insert(
        {name, reinterpret_cast<IResourceArray *>(new ResourceArray<T>())});
    // resourceArrays[name] = reinterpret_cast<IResourceArray*>(new
    // ResourceArray<T>());
  }

  template <typename T> inline T *&getResource(const char *key) {
    const char *name = typeid(T).name();
    return getResourceArray<T>()->getResource(key);
  }
  template <typename T> inline T *&getResource(std::string key) {
    return getResource<T>(key.c_str());
  }

  template <typename T> inline void setResource(const char *key, T *value) {
    const char *name = typeid(T).name();
    getResourceArray<T>()->setResource(key, value);
  }
  template <typename T> inline void setResource(std::string key, T *value) {
    setResource<T>(key.c_str(), value);
  }

  template <typename T> inline void deleteResource(const char *key) {
    getResourceArray<T>()->deleteResource(key);
  }
  template <typename T> inline void deleteResource(std::string key) {
    deleteResource<T>(key.c_str());
  }

  template <typename... Args> void deleteAll() { deleteAllImpl<Args...>(); }

  template <typename First, typename... Rest> void deleteAllImpl() {
    getResourceArray<First>()->deleteAll();
    if constexpr (sizeof...(Rest) > 0) {
      deleteAllImpl<Rest...>();
    }
  }

  inline ~ResourceManager() {
    for (auto &a : resourceArrays) {
      delete a.second;
    }
  }

private:
  absl::flat_hash_map<const char *, IResourceArray *> resourceArrays;

  template <typename T> ResourceArray<T> *getResourceArray() {
    const char *typeName = typeid(T).name();

    if (resourceArrays.find(typeName) == resourceArrays.end()) {
      spdlog::error("Tried to use unregistered component!");
      assert(false);
    }

    // return static_cast<ComponentArray<T>*>(mComponentArrays[typeName]);
    // ComponentArray<T>* temp = mComponentArrays[typeName];
    // return temp;
    return reinterpret_cast<ResourceArray<T> *>(resourceArrays[typeName]);
  }
};

class Coordinator {
public:
  inline void init() {
    pComponentManager = new ComponentManager();
    pEntityManager = new EntityManager();
    pSystemManager = new SystemManager();
    pResourceManager = new ResourceManager();
  }

  inline entity_t createEntity() { return pEntityManager->createEntity(); }

  inline void destroyEntity(entity_t entity) {
    pEntityManager->destroyEntity(entity);
    pComponentManager->entityDestroyed(entity);
    pSystemManager->entityDestroyed(entity);
  }

  template <typename T> inline void registerComponent() {
    pComponentManager->registerComponent<T>();
  }

  template <typename T> inline void addComponent(entity_t entity, T component) {
    pComponentManager->addComponent<T>(entity, component);

    auto signature = pEntityManager->getSignature(entity);
    signature.set(pComponentManager->getComponentType<T>(), true);
    pEntityManager->setSignature(entity, signature);

    pSystemManager->entitySignatureChanged(entity, signature);
  }

  template <typename T> inline void removeComponent(entity_t entity) {
    pComponentManager->removeComponent<T>(entity);

    auto signature = pEntityManager->getSignature(entity);
    signature.set(pComponentManager->getComponentType<T>(), false);
    pEntityManager->setSignature(entity, signature);

    pSystemManager->entitySignatureChanged(entity, signature);
  }

  template <typename T> inline T &getComponent(entity_t entity) {
    return pComponentManager->getComponent<T>(entity);
  }

  template <typename T> inline component_t getComponentType() {
    return pComponentManager->getComponentType<T>();
  }

  template <typename T, typename... Args>
  inline T *registerSystem(Args... args) {
    return pSystemManager->registerSystem<T>(args...);
  }

  template <typename T> inline void setSystemSignature(signature_t signature) {
    pSystemManager->setSignature<T>(signature);
  }

  template <typename T> inline void registerResourceType() {
    pResourceManager->registerResourceType<T>();
  }

  template <typename T> inline T *&getResource(const char *key) {
    return pResourceManager->getResource<T>(key);
  }
  template <typename T> inline T *&getResource(std::string key) {
    return getResource<T>(key.c_str());
  }

  template <typename T> inline void setResource(const char *key, T *value) {
    pResourceManager->setResource<T>(key, value);
  }
  template <typename T> inline void setResource(std::string key, T *value) {
    setResource<T>(key.c_str(), value);
  }

  template <typename T> inline void deleteResource(const char *key) {
    pResourceManager->deleteResource<T>(key);
  }
  template <typename T> inline void deleteResource(std::string key) {
    deleteResource<T>(key.c_str());
  }
  template <typename... Args> inline void deleteAllResources() {
    pResourceManager->deleteAll<Args...>();
  }

  inline ~Coordinator() {
    delete pComponentManager;
    delete pEntityManager;
    delete pSystemManager;
    delete pResourceManager;
  }

public:
  ComponentManager *pComponentManager;
  EntityManager *pEntityManager;
  SystemManager *pSystemManager;
  ResourceManager *pResourceManager;
};
} // namespace ntg::viz
